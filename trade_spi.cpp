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

    //断线后，重新连接
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

    //重新登录成功后更新映射关系
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

    //根据报单响应情况来处理，当不是最终状态时，发起撤单，如果是最终状态，就再下一单
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
    ///XTP系统订单ID，在XTP系统中唯一
    cerr << "order_xtp_id=" << order_info->order_xtp_id << endl;
    ///报单引用，用户自定义
    cerr << "order_client_id=" << order_info->order_client_id << endl;
    ///报单操作引用，用户自定义（暂未使用）
    cerr << "order_cancel_client_id=" << order_info->order_cancel_client_id << endl;
    ///撤单在XTP系统中的id，在XTP系统中唯一
    cerr << "order_cancel_xtp_id=" << order_info->order_cancel_xtp_id << endl;
    ///合约代码
    cerr << "ticker=" << order_info->ticker << endl;
    ///交易市场
    cerr << "market=" << order_info->market << endl;
    ///价格
    cerr << "price=" << order_info->price << endl;
    ///数量，此订单的报单数量
    cerr << "quantity=" << order_info->quantity << endl;
    ///报单价格条件
    cerr << "price_type=" << order_info->price_type << endl;
    ///买卖方向
    cerr << "side=" << order_info->side << endl;
    ///业务类型
    cerr << "business_type=" << order_info->business_type << endl;
    ///今成交数量，为此订单累计成交数量
    cerr << "qty_traded=" << order_info->qty_traded << endl;
    ///剩余数量，当撤单成功时，表示撤单数量
    cerr << "qty_left=" << order_info->qty_left << endl;
    ///委托时间，格式为YYYYMMDDHHMMSSsss
    cerr << "insert_time=" << order_info->insert_time << endl;
    ///最后修改时间，格式为YYYYMMDDHHMMSSsss
    cerr << "update_time=" << order_info->update_time << endl;
    ///撤销时间，格式为YYYYMMDDHHMMSSsss
    cerr << "cancel_time=" << order_info->cancel_time << endl;
    ///成交金额，为此订单的成交总金额
    cerr << "trade_amount=" << order_info->trade_amount << endl;
    ///本地报单编号 OMS生成的单号，不等同于order_xtp_id，为服务器传到报盘的单号
    cerr << "order_local_id=" << order_info->order_local_id << endl;
    ///报单状态，订单响应中没有部分成交状态的推送，在查询订单结果中，会有部分成交状态
    cerr << "order_status=" << order_info->order_status << endl;
    ///报单提交状态，OMS内部使用，用户无需关心
    cerr << "order_submit_status=" << order_info->order_submit_status << endl;
    ///报单类型
    cerr << "order_type=" << order_info->order_type << endl;
}

void MyTraderSpi::OnTradeEvent(XTPTradeReport* trade_info, uint64_t session_id)
{
    cerr << "--->>>" << __FUNCTION__ << endl;
    ///XTP系统订单ID，此成交回报相关的订单ID，在XTP系统中唯一
    cerr << "order_xtp_ip=" << trade_info->order_xtp_id << endl;
    ///报单引用
    cerr << "order_client_id=" << trade_info->order_client_id << endl;
    ///合约代码
    cerr << "ticker=" << trade_info->ticker << endl;
    ///交易市场
    cerr << "market=" << trade_info->market << endl;
    ///订单号，引入XTPID后，该字段实际和order_xtp_id重复。接口中暂时保留。
    cerr << "local_order_id=" << trade_info->local_order_id << endl;
    ///成交编号，深交所唯一，上交所每笔交易唯一，当发现2笔成交回报拥有相同的exec_id，则可以认为此笔交易自成交
    cerr << "exec_id=" << trade_info->exec_id<< endl;
    ///价格，此次成交的价格
    cerr << "price=" << trade_info->price << endl;
    ///数量，此次成交的数量，不是累计数量
    cerr << "quantity=" << trade_info->quantity << endl;
    ///成交时间，格式为YYYYMMDDHHMMSSsss
    cerr << "trade_time=" << trade_info->trade_time << endl;
    ///成交金额，此次成交的总金额 = price*quantity
    cerr << "trade_amount=" << trade_info->trade_amount << endl;
    ///成交序号 --回报记录号，每个交易所唯一,report_index+market字段可以组成唯一标识表示成交回报
    cerr << "report_index=" << trade_info->report_index << endl;
    ///报单编号 --交易所单号，上交所为空，深交所有此字段
    cerr << "order_exch_id=" << trade_info->order_exch_id << endl;
    ///成交类型  --成交回报中的执行类型
    cerr << "trade_type=" << trade_info->trade_type << endl;
    ///买卖方向
    cerr << "side=" << trade_info->side << endl;
    ///业务类型
    cerr << "business_type=" << trade_info->business_type << endl;
    ///交易所交易员代码
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
    ///撤单XTPID
    cerr << "order_cancel_xtp_id=" << cancel_info->order_cancel_xtp_id << endl;
    ///原始订单XTPID
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
    ///XTP系统订单ID，在XTP系统中唯一
    cerr << "order_xtp_id=" << order_info->order_xtp_id << endl;
    ///报单引用，用户自定义
    cerr << "order_client_id=" << order_info->order_client_id << endl;
    ///报单操作引用，用户自定义（暂未使用）
    cerr << "order_cancel_client_id=" << order_info->order_cancel_client_id << endl;
    ///撤单在XTP系统中的id，在XTP系统中唯一
    cerr << "order_cancel_xtp_id=" << order_info->order_cancel_xtp_id << endl;
    ///合约代码
    cerr << "ticker=" << order_info->ticker << endl;
    ///交易市场
    cerr << "market=" << order_info->market << endl;
    ///价格
    cerr << "price=" << order_info->price << endl;
    ///数量，此订单的报单数量
    cerr << "quantity=" << order_info->quantity << endl;
    ///报单价格条件
    cerr << "price_type=" << order_info->price_type << endl;
    ///买卖方向
    cerr << "side=" << order_info->side << endl;
    ///业务类型
    cerr << "business_type=" << order_info->business_type << endl;
    ///今成交数量，为此订单累计成交数量
    cerr << "qty_traded=" << order_info->qty_traded << endl;
    ///剩余数量，当撤单成功时，表示撤单数量
    cerr << "qty_left=" << order_info->qty_left << endl;
    ///委托时间，格式为YYYYMMDDHHMMSSsss
    cerr << "insert_time=" << order_info->insert_time << endl;
    ///最后修改时间，格式为YYYYMMDDHHMMSSsss
    cerr << "update_time=" << order_info->update_time << endl;
    ///撤销时间，格式为YYYYMMDDHHMMSSsss
    cerr << "cancel_time=" << order_info->cancel_time << endl;
    ///成交金额，为此订单的成交总金额
    cerr << "trade_amount=" << order_info->trade_amount << endl;
    ///本地报单编号 OMS生成的单号，不等同于order_xtp_id，为服务器传到报盘的单号
    cerr << "order_local_id=" << order_info->order_local_id << endl;
    ///报单状态，订单响应中没有部分成交状态的推送，在查询订单结果中，会有部分成交状态
    cerr << "order_status=" << order_info->order_status << endl;
    ///报单提交状态，OMS内部使用，用户无需关心
    cerr << "order_submit_status=" << order_info->order_submit_status << endl;
    ///报单类型
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
    ///XTP系统订单ID，此成交回报相关的订单ID，在XTP系统中唯一
    cerr << "order_xtp_id=" << trade_info->order_xtp_id << endl;
    ///报单引用
    cerr << "order_client_id=" << trade_info->order_client_id << endl;
    ///合约代码
    cerr << "ticker=" << trade_info->ticker << endl;
    ///交易市场
    cerr << "market=" << trade_info->market << endl;
    ///订单号，引入XTPID后，该字段实际和order_xtp_id重复。接口中暂时保留。
    cerr << "local_order_id=" << trade_info->local_order_id << endl;
    ///成交编号，深交所唯一，上交所每笔交易唯一，当发现2笔成交回报拥有相同的exec_id，则可以认为此笔交易自成交
    cerr << "exec_id=" << trade_info->exec_id << endl;
    ///价格，此次成交的价格
    cerr << "price=" << trade_info->price << endl;
    ///数量，此次成交的数量，不是累计数量
    cerr << "quantity=" << trade_info->quantity << endl;
    ///成交时间，格式为YYYYMMDDHHMMSSsss
    cerr << "trade_time=" << trade_info->trade_time << endl;
    ///成交金额，此次成交的总金额 = price*quantity
    cerr << "trade_amount=" << trade_info->trade_amount << endl;
    ///成交序号 --回报记录号，每个交易所唯一,report_index+market字段可以组成唯一标识表示成交回报
    cerr << "report_index=" << trade_info->report_index << endl;
    ///报单编号 --交易所单号，上交所为空，深交所有此字段
    cerr << "order_exch_id=" << trade_info->order_exch_id << endl;
    ///成交类型  --成交回报中的执行类型
    cerr << "trade_type=" << trade_info->trade_type << endl;
    ///买卖方向
    cerr << "side=" << trade_info->side << endl;
    ///业务类型
    cerr << "business_type=" << trade_info->business_type << endl;
    ///交易所交易员代码
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
    ///证券代码
    cerr << "ticker=" << investor_position->ticker << endl;
    ///证券名称
    cerr << "ticker_name=" << investor_position->ticker_name << endl;
    ///交易市场
    cerr << "market=" << investor_position->market << endl;
    ///总持仓
    cerr << "total_qty=" << investor_position->total_qty << endl;
    ///可卖持仓
    cerr << "sellable_qty=" << investor_position->sellable_qty << endl;
    ///持仓成本
    cerr << "avg_price=" << investor_position->avg_price << endl;
    ///浮动盈亏（保留字段）
    cerr << "unrealized_pnl=" << investor_position->unrealized_pnl << endl;
    ///昨日持仓
    cerr << "yesterday_position=" << investor_position->yesterday_position << endl;
    ///今日申购赎回数量（申购和赎回数量不可能同时存在，因此可以共用一个字段）
    cerr << "purchase_redeemable_qty=" << investor_position->purchase_redeemable_qty << endl;
    ///(保留字段)
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
    ///总资产(=可用资金 + 证券资产（目前为0）+ 预扣的资金)
    cerr << "total_asset=" << trading_account->total_asset << endl;
    ///可用资金
    cerr << "buying_power=" << trading_account->buying_power << endl;
    ///证券资产（保留字段，目前为0）
    cerr << "security_asset=" << trading_account->security_asset << endl;
    ///累计买入成交证券占用资金
    cerr << "fund_buy_amount=" << trading_account->fund_buy_amount << endl;
    ///累计买入成交交易费用
    cerr << "fund_buy_fee=" << trading_account->fund_buy_fee << endl;
    ///累计卖出成交证券所得资金
    cerr << "fund_sell_amount=" << trading_account->fund_sell_amount << endl;
    ///累计卖出成交交易费用
    cerr << "fund_sell_fee=" << trading_account->fund_sell_fee << endl;
    ///XTP系统预扣的资金（包括购买卖股票时预扣的交易资金+预扣手续费）
    cerr << "withholding_amount=" << trading_account->withholding_amount << endl;
    ///账户类型
    cerr << "account_type=" << trading_account->account_type << endl;
    ///(保留字段)
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
    // 如果ErrorID != 0, 说明收到了错误的响应
    bool bResult = ((pRspInfo) && (pRspInfo->error_id != 0));
    if (pRspInfo)
	cerr << "--->>>ErrorID=" << pRspInfo->error_id << ", ErrorMsg=" << pRspInfo->error_msg << endl;
    return bResult;
}

int MyTraderSpi::getCurrQuestID()
{
    return m_iquestID++;
}

