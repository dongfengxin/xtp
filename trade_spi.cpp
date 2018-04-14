#include "trade_spi.h"
#include <iostream>
#include <cstring>
#include <iomanip>
#include <stdio.h>
#include <sys/timeb.h>
#include <time.h>
#ifndef _WIN32
#include <sys/time.h>
#include <unistd.h>
#else
#include <windows.h>

#endif
#include "FileUtils.h"
#include <map>
#include <sstream>

using namespace std;

//extern XTP::API::TraderApi* pUserApi;
extern bool is_connected_;
extern std::string trade_server_ip;
extern int trade_server_port;
extern uint64_t session_id_;
extern std::map<uint64_t, uint64_t> map_session;
extern uint64_t* session_arrary;
extern FileUtils* fileUtils;
extern XTPOrderInsertInfo* orderList;

MyTraderSpi::MyTraderSpi()
{
    order_num = 0;
    trade_num = 0;
    insert_order_num = 0;
    cancel_order_num = 0;

#ifndef _WIN32
    std::string orderName = "log/";
    std::string tradeName = "log/";
    std::string fundName = "log/";
    std::string positonName = "log/";
    std::string qryOrderName = "log/";
    std::string qryTradeName = "log/";
    std::string cancelOrderName = "log/";
    std::string positionCountName = "log/";
#else
    std::string orderName = "log\\";
    std::string tradeName = "log\\";
    std::string fundName = "log\\";
    std::string positonName = "log\\";
    std::string qryOrderName = "log\\";
    std::string qryTradeName = "log\\";
    std::string cancelOrderName = "log\\";
    std::string positionCountName = "log\\";
#endif // !_WIN32

#ifndef _WIN32
    struct timeval start;
    gettimeofday(&start, 0);
    stringstream ss;
    ss << start.tv_sec;

    std::string pre;
    ss >> pre;

    orderName.append(pre);
    tradeName.append(pre);
    fundName.append(pre);
    positonName.append(pre);
    qryOrderName.append(pre);
    qryTradeName.append(pre);
    cancelOrderName.append(pre);
    positionCountName.append(pre);
#else

    SYSTEMTIME wtm;
    GetLocalTime(&wtm);

    stringstream ss;
    ss << wtm.wDay << wtm.wHour << wtm.wMinute << wtm.wSecond;

    std::string pre;
    ss >> pre;

    orderName.append(pre);
    tradeName.append(pre);
    fundName.append(pre);
    positonName.append(pre);
    qryOrderName.append(pre);
    qryTradeName.append(pre);
    cancelOrderName.append(pre);
    positionCountName.append(pre);

#endif

    orderName.append("order.txt");
    tradeName.append("tradeReport.txt");
    fundName.append("fund.txt");
    positonName.append("stock_position.txt");
    qryOrderName.append("qry_order.txt");
    qryTradeName.append("qry_tradeReport.txt");
    cancelOrderName.append("cancel_order.txt");
    positionCountName.append("qry_positionCount.txt");

    fout_order.open(orderName.c_str());
    fout_trade.open(tradeName.c_str());
    fout_fund.open(fundName.c_str());
    fout_position.open(positonName.c_str());
    fout_qry_order.open(qryOrderName.c_str());
    fout_qry_trade.open(qryTradeName.c_str());
    fout_cancel_order.open(cancelOrderName.c_str());
    fout_position_count.open(positionCountName.c_str());

    outCount = 1;

    m_iquestID = 1;

    save_to_file_ = false;
}

MyTraderSpi::~MyTraderSpi()
{
    fout_order.close();
    fout_trade.close();
    fout_fund.close();
    fout_position.close();
    fout_qry_order.close();
    fout_qry_trade.close();
    fout_cancel_order.close();
    fout_position_count.close();
}

void MyTraderSpi::OnDisconnected(uint64_t session_id, int reason)
{
    cerr << "-------------------- OnDisconnected -------------------------" << endl;
    uint64_t aid = 0;
    std::map<uint64_t, uint64_t>::iterator pos = map_session.find(session_id);
    if (pos != map_session.end()) {
	aid = pos->second;
	map_session.erase(pos);
    }

    //���ߺ���������
    is_connected_ = false;
    uint64_t temp_session_id_ = 0;
    do {
#ifdef _WIN32
	Sleep(1000);
#else
	sleep(1);
#endif // _WIN32
	std::string account_name = fileUtils->stdStringForKey("account[%d].user", aid);
	std::string account_pw = fileUtils->stdStringForKey("account[%d].password", aid);
	temp_session_id_ = pUserApi->Login(trade_server_ip.c_str(), trade_server_port, account_name.c_str(), account_pw.c_str(), XTP_PROTOCOL_TCP);

    } while (temp_session_id_ == 0);

    //���µ�¼�ɹ������ӳ���ϵ
    map_session.insert(std::make_pair(temp_session_id_, aid));
    session_id_ = temp_session_id_;
    is_connected_ = true;
    session_arrary[aid] = temp_session_id_;
}

void MyTraderSpi::OnError(XTPRI* error_info)
{
    cerr << "--->>>" << __FUNCTION__ << endl;
    IsErrorRspInfo(error_info);
}

void MyTraderSpi::OnOrderEvent(XTPOrderInfo* order_info, XTPRI* error_info, uint64_t session_id)
{
/* #if _IS_LOG_SHOW_
    cerr << "------------------- OnOrderEvent-----------" << endl;
    cerr << "Order_Client_ID:" << pOrder->order_client_id << endl;
    cerr << "Order_XTP_ID:" << pOrder->order_xtp_id << endl;
    cerr << "Order_status:" << pOrder->order_status << endl;
    cerr << "Order_submit_status:" << pOrder->order_submit_status << endl;
    cerr << "update time:" << pOrder->update_time << endl;
    if (pRspInfo) {
	cerr << "Error:" << pRspInfo->error_id << endl;
    }
#endif
    updateOrderNum();
    if (order_num % outCount == 0) {
	// 		cerr << "Order_XTP_ID:" << order_info->order_xtp_id << endl;
	// 		cerr << "Order_status:" << order_info->order_status << endl;
	//
	// 		int32_t client = pUserApi->GetClientIDByXTPID(order_info->order_xtp_id);
	// 		cerr << "Client id:" << client << ", Account:" << pUserApi->GetAccountByXTPID(order_info->order_xtp_id) << endl;

	cerr << "!!!!!!!!!!!!!!!!!!!!! OnOrderEvent total count:" << order_num << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
    }

    if (save_to_file_) {
#ifdef _WIN32
	SYSTEMTIME wtm;
	GetLocalTime(&wtm);
	fout_order << "[" << wtm.wHour << ":" << wtm.wMinute << ":" << wtm.wSecond << "." << wtm.wMilliseconds << "],";
#else
	struct timeval start;
	gettimeofday(&start, 0);
	//		fout_order << "time:" << start.tv_sec << "." << start.tv_usec << ",";
	struct tm* ptr;
	time_t lt;
	lt = time(NULL);
	ptr = localtime(&lt);
	fout_order << "[" << ptr->tm_hour << ":" << ptr->tm_min << ":" << ptr->tm_sec << "." << start.tv_usec << "],";
#endif // _WIN32

	fout_order << "xtp_id:" << order_info->order_xtp_id << ",client_id:" << order_info->order_client_id << ",status:" << order_info->order_status << ",cancel_xtp_id:" << order_info->order_cancel_xtp_id << ",cancel_client_id:" << order_info->order_cancel_client_id;
	fout_order << ",order_submit_status:" << order_info->order_submit_status << ",ticker:" << order_info->ticker << ",market:" << order_info->market << ",price:" << order_info->price;
	fout_order << ",quantity:" << order_info->quantity << ",price_type:" << order_info->price_type << ",side:" << order_info->side << ",qty_traded:" << order_info->qty_traded << ",qty_left:" << order_info->qty_left;
	fout_order << ",insert_time:" << order_info->insert_time << ",update_time:" << order_info->update_time << ",cancel_time:" << order_info->cancel_time << ",trade_amount:" << order_info->trade_amount;
	fout_order << ",order_local_id:" << order_info->order_local_id << ",order_type:" << order_info->order_type << ",error_id:" << error_info->error_id << ",error_msg:" << error_info->error_msg << endl;
    }

    //���ݱ�����Ӧ�������������������״̬ʱ�����𳷵������������״̬��������һ��
    switch (order_info->order_status) {
    case XTP_ORDER_STATUS_NOTRADEQUEUEING: {
	pUserApi->CancelOrder(order_info->order_xtp_id, session_arrary[order_info->order_client_id]);
	break;
    }
    case XTP_ORDER_STATUS_ALLTRADED:
    case XTP_ORDER_STATUS_PARTTRADEDNOTQUEUEING:
    case XTP_ORDER_STATUS_CANCELED:
    case XTP_ORDER_STATUS_REJECTED: {
	int i = order_info->order_client_id;
	pUserApi->InsertOrder(&(orderList[i]), session_arrary[i]);
	break;
    }

    default:
	break;
    } */
    cerr << "--->>>" << __FUNCTION__ << endl;
    IsErrorRspInfo(error_info);
    ///XTPϵͳ����ID����XTPϵͳ��Ψһ
    cerr << "order_xtp_id=" << order_info->order_xtp_id << endl;
    ///�������ã��û��Զ���
    cerr << "order_client_id=" << order_info->order_client_id << endl;
    ///�����������ã��û��Զ��壨��δʹ�ã�
    cerr << "order_cancel_client_id=" << order_info->order_cancel_client_id << endl;
    ///������XTPϵͳ�е�id����XTPϵͳ��Ψһ
    cerr << "order_cancel_xtp_id=" << order_info->order_cancel_xtp_id << endl;
    ///��Լ����
    cerr << "ticker=" << order_info->ticker << endl;
    ///�����г�
    cerr << "market=" << order_info->market << endl;
    ///�۸�
    cerr << "price=" << order_info->price << endl;
    ///�������˶����ı�������
    cerr << "quantity=" << order_info->quantity << endl;
    ///�����۸�����
    cerr << "price_type=" << order_info->price_type << endl;
    ///��������
    cerr << "side=" << order_info->side << endl;
    ///ҵ������
    cerr << "business_type=" << order_info->business_type << endl;
    ///��ɽ�������Ϊ�˶����ۼƳɽ�����
    cerr << "qty_traded=" << order_info->qty_traded << endl;
    ///ʣ���������������ɹ�ʱ����ʾ��������
    cerr << "qty_left=" << order_info->qty_left << endl;
    ///ί��ʱ�䣬��ʽΪYYYYMMDDHHMMSSsss
    cerr << "insert_time=" << order_info->insert_time << endl;
    ///����޸�ʱ�䣬��ʽΪYYYYMMDDHHMMSSsss
    cerr << "update_time=" << order_info->update_time << endl;
    ///����ʱ�䣬��ʽΪYYYYMMDDHHMMSSsss
    cerr << "cancel_time=" << order_info->cancel_time << endl;
    ///�ɽ���Ϊ�˶����ĳɽ��ܽ��
    cerr << "trade_amount=" << order_info->trade_amount << endl;
    ///���ر������ OMS���ɵĵ��ţ�����ͬ��order_xtp_id��Ϊ�������������̵ĵ���
    cerr << "order_local_id=" << order_info->order_local_id << endl;
    ///����״̬��������Ӧ��û�в��ֳɽ�״̬�����ͣ��ڲ�ѯ��������У����в��ֳɽ�״̬
    cerr << "order_status=" << order_info->order_status << endl;
    ///�����ύ״̬��OMS�ڲ�ʹ�ã��û��������
    cerr << "order_submit_status=" << order_info->order_submit_status << endl;
    ///��������
    cerr << "order_type=" << order_info->order_type << endl;
}

void MyTraderSpi::OnTradeEvent(XTPTradeReport* trade_info, uint64_t session_id)
{
    cerr << "--->>>" << __FUNCTION__ << endl;
    ///XTPϵͳ����ID���˳ɽ��ر���صĶ���ID����XTPϵͳ��Ψһ
    cerr << "order_xtp_ip=" << trade_info->order_xtp_id << endl;
    ///��������
    cerr << "order_client_id=" << trade_info->order_client_id << endl;
    ///��Լ����
    cerr << "ticker=" << trade_info->ticker << endl;
    ///�����г�
    cerr << "market=" << trade_info->market << endl;
    ///�����ţ�����XTPID�󣬸��ֶ�ʵ�ʺ�order_xtp_id�ظ����ӿ�����ʱ������
    cerr << "local_order_id=" << trade_info->local_order_id << endl;
    ///�ɽ���ţ����Ψһ���Ͻ���ÿ�ʽ���Ψһ��������2�ʳɽ��ر�ӵ����ͬ��exec_id���������Ϊ�˱ʽ����Գɽ�
    cerr << "exec_id=" << trade_info->exec_id<< endl;
    ///�۸񣬴˴γɽ��ļ۸�
    cerr << "price=" << trade_info->price << endl;
    ///�������˴γɽ��������������ۼ�����
    cerr << "quantity=" << trade_info->quantity << endl;
    ///�ɽ�ʱ�䣬��ʽΪYYYYMMDDHHMMSSsss
    cerr << "trade_time=" << trade_info->trade_time << endl;
    ///�ɽ����˴γɽ����ܽ�� = price*quantity
    cerr << "trade_amount=" << trade_info->trade_amount << endl;
    ///�ɽ���� --�ر���¼�ţ�ÿ��������Ψһ,report_index+market�ֶο������Ψһ��ʶ��ʾ�ɽ��ر�
    cerr << "report_index=" << trade_info->report_index << endl;
    ///������� --���������ţ��Ͻ���Ϊ�գ�����д��ֶ�
    cerr << "order_exch_id=" << trade_info->order_exch_id << endl;
    ///�ɽ�����  --�ɽ��ر��е�ִ������
    cerr << "trade_type=" << trade_info->trade_type << endl;
    ///��������
    cerr << "side=" << trade_info->side << endl;
    ///ҵ������
    cerr << "business_type=" << trade_info->business_type << endl;
    ///����������Ա����
    cerr << "branch_pbu=" << trade_info->branch_pbu << endl;
/* #if _IS_LOG_SHOW_
    cerr << "------------------- OnTradeEvent-----------" << endl;
    cerr << "Order_Client_ID:" << pTrade->order_client_id << endl;
    cerr << "Local_Order_ID:" << pTrade->local_order_id << endl;
    cerr << "Order_Exch_ID:" << pTrade->order_exch_id << endl;
    cerr << "Order_xtp_id:" << pTrade->order_xtp_id << endl;
    cerr << "Side:" << pTrade->side << endl;
    cerr << "Price:" << pTrade->price << endl;
    cerr << "Quantity:" << pTrade->quantity << endl;
    cerr << "Trade amount" << pTrade->trade_amount << endl;
    cerr << "Trade Time:" << pTrade->trade_time << endl;
    cerr << "Report Index:" << pTrade->report_index << endl;
#endif

    updateTradeNum();

    if (trade_num % outCount == 0) {
	cerr << "!!!!!!!!!!!!!!!!!!!!! OnTradeEvent total count:" << trade_num << "!!!!!!!!!!!!!!!!!!!!!" << endl;
    }

    if (save_to_file_) {
#ifdef _WIN32
	SYSTEMTIME wtm;
	GetLocalTime(&wtm);
	fout_trade << "[" << wtm.wHour << ":" << wtm.wMinute << ":" << wtm.wSecond << "." << wtm.wMilliseconds << "],";
#else
	struct timeval start;
	gettimeofday(&start, 0);
	//		fout_trade << "time:" << start.tv_sec << "." << start.tv_usec << ",";
	struct tm* ptr;
	time_t lt;
	lt = time(NULL);
	ptr = localtime(&lt);
	fout_trade << "[" << ptr->tm_hour << ":" << ptr->tm_min << ":" << ptr->tm_sec << "." << start.tv_usec << "],";
#endif // _WIN32
	fout_trade << "xtp_id:" << trade_info->order_xtp_id << ",client_id:" << trade_info->order_client_id;
	fout_trade << ",ticker:" << trade_info->ticker << ",market:" << trade_info->market << ",price:" << trade_info->price;
	fout_trade << ",quantity:" << trade_info->quantity << ",side:" << trade_info->side << ",trade_time:" << trade_info->trade_time << ",trade_amount:" << trade_info->trade_amount;
	fout_trade << ",exec_id:" << trade_info->exec_id << ",report_index:" << trade_info->report_index << ",order_exch_id:" << trade_info->order_exch_id;
	fout_trade << ",trade_type:" << trade_info->trade_type << ",branch_pbu:" << trade_info->branch_pbu << endl;
    } */
}

void MyTraderSpi::OnCancelOrderError(XTPOrderCancelInfo* cancel_info, XTPRI* error_info, uint64_t session_id)
{
    cerr << "--->>>" << __FUNCTION__ << endl;
    IsErrorRspInfo(error_info);
    ///����XTPID
    cerr << "order_cancel_xtp_id=" << cancel_info->order_cancel_xtp_id << endl;
    ///ԭʼ����XTPID
    cerr << "order_xtp_id=" << cancel_info->order_xtp_id << endl;
    // 	cerr << "-----------------OnCancelOrderError---------------------" << endl;
    // 	cerr << "cancel_order_xtp_id:" << cancel_info->order_cancel_xtp_id << "   order_xtp_id:" << cancel_info->order_xtp_id << "  errorcode:" << error_info->error_id << "  msg:" << error_info->error_msg << endl;
    /* if (save_to_file_) {
	fout_cancel_order << "cancel_order_xtp_id:" << cancel_info->order_cancel_xtp_id << ",order_xtp_id:" << cancel_info->order_xtp_id << ",error_id:" << error_info->error_id << ",msg:" << error_info->error_msg << endl;
    } */
}

void MyTraderSpi::OnQueryOrder(XTPQueryOrderRsp* order_info, XTPRI* error_info, int request_id, bool is_last, uint64_t session_id)
{
    cerr << "--->>>" << __FUNCTION__ << endl;
    IsErrorRspInfo(error_info);
    ///XTPϵͳ����ID����XTPϵͳ��Ψһ
    cerr << "order_xtp_id=" << order_info->order_xtp_id << endl;
    ///�������ã��û��Զ���
    cerr << "order_client_id=" << order_info->order_client_id << endl;
    ///�����������ã��û��Զ��壨��δʹ�ã�
    cerr << "order_cancel_client_id=" << order_info->order_cancel_client_id << endl;
    ///������XTPϵͳ�е�id����XTPϵͳ��Ψһ
    cerr << "order_cancel_xtp_id=" << order_info->order_cancel_xtp_id << endl;
    ///��Լ����
    cerr << "ticker=" << order_info->ticker << endl;
    ///�����г�
    cerr << "market=" << order_info->market << endl;
    ///�۸�
    cerr << "price=" << order_info->price << endl;
    ///�������˶����ı�������
    cerr << "quantity=" << order_info->quantity << endl;
    ///�����۸�����
    cerr << "price_type=" << order_info->price_type << endl;
    ///��������
    cerr << "side=" << order_info->side << endl;
    ///ҵ������
    cerr << "business_type=" << order_info->business_type << endl;
    ///��ɽ�������Ϊ�˶����ۼƳɽ�����
    cerr << "qty_traded=" << order_info->qty_traded << endl;
    ///ʣ���������������ɹ�ʱ����ʾ��������
    cerr << "qty_left=" << order_info->qty_left << endl;
    ///ί��ʱ�䣬��ʽΪYYYYMMDDHHMMSSsss
    cerr << "insert_time=" << order_info->insert_time << endl;
    ///����޸�ʱ�䣬��ʽΪYYYYMMDDHHMMSSsss
    cerr << "update_time=" << order_info->update_time << endl;
    ///����ʱ�䣬��ʽΪYYYYMMDDHHMMSSsss
    cerr << "cancel_time=" << order_info->cancel_time << endl;
    ///�ɽ���Ϊ�˶����ĳɽ��ܽ��
    cerr << "trade_amount=" << order_info->trade_amount << endl;
    ///���ر������ OMS���ɵĵ��ţ�����ͬ��order_xtp_id��Ϊ�������������̵ĵ���
    cerr << "order_local_id=" << order_info->order_local_id << endl;
    ///����״̬��������Ӧ��û�в��ֳɽ�״̬�����ͣ��ڲ�ѯ��������У����в��ֳɽ�״̬
    cerr << "order_status=" << order_info->order_status << endl;
    ///�����ύ״̬��OMS�ڲ�ʹ�ã��û��������
    cerr << "order_submit_status=" << order_info->order_submit_status << endl;
    ///��������
    cerr << "order_type=" << order_info->order_type << endl;
    // 	cerr << "------------------- OnQueryOrder-----------" << endl;
    // 	cerr << "OnQueryOrder: " << order_info->ticker << ":" << order_info->order_xtp_id  << ":" << request_id << ":" << is_last << endl;

    /* if (save_to_file_) {
#ifdef _WIN32
	SYSTEMTIME wtm;
	GetLocalTime(&wtm);
	fout_qry_order << "[" << wtm.wHour << ":" << wtm.wMinute << ":" << wtm.wSecond << "." << wtm.wMilliseconds << "],";
#else
	struct timeval start;
	gettimeofday(&start, 0);
	//		fout_trade << "time:" << start.tv_sec << "." << start.tv_usec << ",";
	struct tm* ptr;
	time_t lt;
	lt = time(NULL);
	ptr = localtime(&lt);
	fout_qry_order << "[" << ptr->tm_hour << ":" << ptr->tm_min << ":" << ptr->tm_sec << "." << start.tv_usec << "],";
#endif // _WIN32
	fout_qry_order << "xtp_id:" << order_info->order_xtp_id << ",client_id:" << order_info->order_client_id << ",status:" << order_info->order_status << ",cancel_xtp_id:" << order_info->order_cancel_xtp_id << ",cancel_client_id:" << order_info->order_cancel_client_id;
	fout_qry_order << ",order_submit_status:" << order_info->order_submit_status << ",ticker:" << order_info->ticker << ",market:" << order_info->market << ",price:" << order_info->price;
	fout_qry_order << ",quantity:" << order_info->quantity << ",price_type:" << order_info->price_type << ",side:" << order_info->side << ",qty_traded:" << order_info->qty_traded << ",qty_left:" << order_info->qty_left;
	fout_qry_order << ",insert_time:" << order_info->insert_time << ",update_time:" << order_info->update_time << ",cancel_time:" << order_info->cancel_time << ",trade_amount:" << order_info->trade_amount;
	fout_qry_order << ",order_local_id:" << order_info->order_local_id << ",order_type:" << order_info->order_type << ",error_id:" << error_info->error_id << ",error_msg:" << error_info->error_msg << endl;
    } */
}

void MyTraderSpi::OnQueryTrade(XTPQueryTradeRsp* trade_info, XTPRI* error_info, int request_id, bool is_last, uint64_t session_id)
{
    cerr << "--->>>" << __FUNCTION__ << endl;
    IsErrorRspInfo(error_info);
    ///XTPϵͳ����ID���˳ɽ��ر���صĶ���ID����XTPϵͳ��Ψһ
    cerr << "order_xtp_id=" << trade_info->order_xtp_id << endl;
    ///��������
    cerr << "order_client_id=" << trade_info->order_client_id << endl;
    ///��Լ����
    cerr << "ticker=" << trade_info->ticker << endl;
    ///�����г�
    cerr << "market=" << trade_info->market << endl;
    ///�����ţ�����XTPID�󣬸��ֶ�ʵ�ʺ�order_xtp_id�ظ����ӿ�����ʱ������
    cerr << "local_order_id=" << trade_info->local_order_id << endl;
    ///�ɽ���ţ����Ψһ���Ͻ���ÿ�ʽ���Ψһ��������2�ʳɽ��ر�ӵ����ͬ��exec_id���������Ϊ�˱ʽ����Գɽ�
    cerr << "exec_id=" << trade_info->exec_id << endl;
    ///�۸񣬴˴γɽ��ļ۸�
    cerr << "price=" << trade_info->price << endl;
    ///�������˴γɽ��������������ۼ�����
    cerr << "quantity=" << trade_info->quantity << endl;
    ///�ɽ�ʱ�䣬��ʽΪYYYYMMDDHHMMSSsss
    cerr << "trade_time=" << trade_info->trade_time << endl;
    ///�ɽ����˴γɽ����ܽ�� = price*quantity
    cerr << "trade_amount=" << trade_info->trade_amount << endl;
    ///�ɽ���� --�ر���¼�ţ�ÿ��������Ψһ,report_index+market�ֶο������Ψһ��ʶ��ʾ�ɽ��ر�
    cerr << "report_index=" << trade_info->report_index << endl;
    ///������� --���������ţ��Ͻ���Ϊ�գ�����д��ֶ�
    cerr << "order_exch_id=" << trade_info->order_exch_id << endl;
    ///�ɽ�����  --�ɽ��ر��е�ִ������
    cerr << "trade_type=" << trade_info->trade_type << endl;
    ///��������
    cerr << "side=" << trade_info->side << endl;
    ///ҵ������
    cerr << "business_type=" << trade_info->business_type << endl;
    ///����������Ա����
    cerr << "branch_pbu=" << trade_info->branch_pbu << endl;
    // 	cerr << "------------------- OnQueryTrade-----------" << endl;
    // 	cerr << "OnQueryTrade: " << trade_info->ticker << ":" << trade_info->order_xtp_id << ":" << request_id << ":" << is_last << endl;

    /* if (save_to_file_) {
	fout_qry_trade << "xtp_id:" << trade_info->order_xtp_id << ",client_id:" << trade_info->order_client_id;
	fout_qry_trade << ",ticker:" << trade_info->ticker << ",market:" << trade_info->market << ",price:" << trade_info->price;
	fout_qry_trade << ",quantity:" << trade_info->quantity << ",side:" << trade_info->side << ",trade_time:" << trade_info->trade_time << ",trade_amount:" << trade_info->trade_amount;
	fout_qry_trade << ",exec_id:" << trade_info->exec_id << ",report_index:" << trade_info->report_index << ",order_exch_id:" << trade_info->order_exch_id;
	fout_qry_trade << ",trade_type:" << trade_info->trade_type << ",branch_pbu:" << trade_info->branch_pbu << endl;
    } */
}

void MyTraderSpi::OnQueryPosition(XTPQueryStkPositionRsp* investor_position, XTPRI* error_info, int request_id, bool is_last, uint64_t session_id)
{
    cerr << "--->>>" << __FUNCTION__ << endl;
    IsErrorRspInfo(error_info);
    ///֤ȯ����
    cerr << "ticker=" << investor_position->ticker << endl;
    ///֤ȯ����
    cerr << "ticker_name=" << investor_position->ticker_name << endl;
    ///�����г�
    cerr << "market=" << investor_position->market << endl;
    ///�ֲܳ�
    cerr << "total_qty=" << investor_position->total_qty << endl;
    ///�����ֲ�
    cerr << "sellable_qty=" << investor_position->sellable_qty << endl;
    ///�ֲֳɱ�
    cerr << "avg_price=" << investor_position->avg_price << endl;
    ///����ӯ���������ֶΣ�
    cerr << "unrealized_pnl=" << investor_position->unrealized_pnl << endl;
    ///���ճֲ�
    cerr << "yesterday_position=" << investor_position->yesterday_position << endl;
    ///�����깺����������깺���������������ͬʱ���ڣ���˿��Թ���һ���ֶΣ�
    cerr << "purchase_redeemable_qty=" << investor_position->purchase_redeemable_qty << endl;
    ///(�����ֶ�)
    cerr << "unknown=" << investor_position->unknown << endl;

    /* if (save_to_file_) {
#ifdef _WIN32
	SYSTEMTIME wtm;
	GetLocalTime(&wtm);
	fout_position << "time:" << wtm.wSecond << "." << wtm.wMilliseconds << ",";

#else
	struct timeval start;
	gettimeofday(&start, 0);

	struct tm* ptr;
	time_t lt;
	lt = time(NULL);
	ptr = localtime(&lt);
	fout_position << "[" << ptr->tm_hour << ":" << ptr->tm_min << ":" << ptr->tm_sec << "." << start.tv_usec << "],";

#endif // _WIN32

	fout_position << "request_id:" << request_id << ",is_last:" << is_last << ",";
	fout_position << "ticker:" << investor_position->ticker << ",ticker_name:" << investor_position->ticker_name;
	fout_position << ",total_qty:" << investor_position->total_qty << ",sellable_qty:" << investor_position->sellable_qty << ",avg_price:" << investor_position->avg_price;
	fout_position << ",unrealized_pnl:" << investor_position->unrealized_pnl << endl;
    } */
}

void MyTraderSpi::OnQueryAsset(XTPQueryAssetRsp* trading_account, XTPRI* error_info, int request_id, bool is_last, uint64_t session_id)
{
    cerr << "--->>>" << __FUNCTION__ << endl;
    IsErrorRspInfo(error_info);
    ///���ʲ�(=�����ʽ� + ֤ȯ�ʲ���ĿǰΪ0��+ Ԥ�۵��ʽ�)
    cerr << "total_asset=" << trading_account->total_asset << endl;
    ///�����ʽ�
    cerr << "buying_power=" << trading_account->buying_power << endl;
    ///֤ȯ�ʲ��������ֶΣ�ĿǰΪ0��
    cerr << "security_asset=" << trading_account->security_asset << endl;
    ///�ۼ�����ɽ�֤ȯռ���ʽ�
    cerr << "fund_buy_amount=" << trading_account->fund_buy_amount << endl;
    ///�ۼ�����ɽ����׷���
    cerr << "fund_buy_fee=" << trading_account->fund_buy_fee << endl;
    ///�ۼ������ɽ�֤ȯ�����ʽ�
    cerr << "fund_sell_amount=" << trading_account->fund_sell_amount << endl;
    ///�ۼ������ɽ����׷���
    cerr << "fund_sell_fee=" << trading_account->fund_sell_fee << endl;
    ///XTPϵͳԤ�۵��ʽ𣨰�����������ƱʱԤ�۵Ľ����ʽ�+Ԥ�������ѣ�
    cerr << "withholding_amount=" << trading_account->withholding_amount << endl;
    ///�˻�����
    cerr << "account_type=" << trading_account->account_type << endl;
    ///(�����ֶ�)
    cerr << "unknown=" << trading_account->unknown << endl;
    // 	cerr << "------------------- OnQueryTradingAccount-----------" << endl;
    // 	cerr << "OnQueryTradingAccount: " << trading_account->buying_power << ":" << trading_account->fund_buy_amount << ":" << request_id << ":" << is_last << endl;

    /* if (save_to_file_) {
#ifdef _WIN32
	SYSTEMTIME wtm;
	GetLocalTime(&wtm);
	fout_fund << "time:" << wtm.wSecond << "." << wtm.wMilliseconds << ",";
#else
	struct timeval start;
	gettimeofday(&start, 0);
	struct tm* ptr;
	time_t lt;
	lt = time(NULL);
	ptr = localtime(&lt);
	fout_fund << "[" << ptr->tm_hour << ":" << ptr->tm_min << ":" << ptr->tm_sec << "." << start.tv_usec << "],";
#endif // _WIN32
	fout_fund << "request_id:" << request_id << ",total_asset:" << setprecision(14) << trading_account->total_asset << ",security_asset:" << setprecision(16) << trading_account->security_asset;
	fout_fund << ",buying_power:" << setprecision(16) << trading_account->buying_power << ",fund_buy_amount:" << setprecision(16) << trading_account->fund_buy_amount;
	fout_fund << ",fund_buy_fee:" << setprecision(16) << trading_account->fund_buy_fee << ",fund_sell_amount:" << setprecision(16) << trading_account->fund_sell_amount << ",fund_sell_fee:" << setprecision(16) << trading_account->fund_sell_fee << endl;
    } */
}

void MyTraderSpi::updateOrderNum()
{
    order_num++;
}

int MyTraderSpi::getOrderNum()
{
    return order_num;
}

void MyTraderSpi::resetOrderNum()
{
    order_num = 0;
}

void MyTraderSpi::updateTradeNum()
{
    trade_num++;
}

int MyTraderSpi::getTradeNum()
{
    return trade_num;
}

void MyTraderSpi::resetTradeNum()
{
    trade_num = 0;
}

void MyTraderSpi::updateInsertOrderNum()
{
    insert_order_num++;
}

int MyTraderSpi::getInsertOrderNum()
{
    return insert_order_num;
}

void MyTraderSpi::resetInsertOrderNum()
{
    insert_order_num = 0;
}

void MyTraderSpi::updateCancelOrderNum()
{
    cancel_order_num++;
}

int MyTraderSpi::getCancelOrderNum()
{
    return cancel_order_num;
}

void MyTraderSpi::resetCancelOrderNum()
{
    cancel_order_num = 0;
}

bool MyTraderSpi::IsErrorRspInfo(XTPRI* pRspInfo)
{
    // ���ErrorID != 0, ˵���յ��˴������Ӧ
    bool bResult = ((pRspInfo) && (pRspInfo->error_id != 0));
    if (pRspInfo)
	cerr << "--->>>ErrorID=" << pRspInfo->error_id << ", ErrorMsg=" << pRspInfo->error_msg << endl;
    return bResult;
}

int MyTraderSpi::getCurrQuestID()
{
    return m_iquestID++;
}

