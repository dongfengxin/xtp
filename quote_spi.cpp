#include "quote_spi.h"
#include <iostream>
#include <stdio.h>
using namespace std;

void MyQuoteSpi::OnError(XTPRI* error_info, bool is_last)
{
    cout << "--->>> "
	 << "OnRspError" << endl;
    IsErrorRspInfo(error_info,is_last);
}

MyQuoteSpi::MyQuoteSpi()
{
}

MyQuoteSpi::~MyQuoteSpi()
{
}

void MyQuoteSpi::OnDisconnected(int reason)
{
    cout << "--->>> "
	 << "OnDisconnected quote" << endl;
    cout << "--->>> Reason = " << reason << endl;
    //���ߺ󣬿�����������
    //�������ӳɹ�����Ҫ���������������������
}

void MyQuoteSpi::OnSubMarketData(XTPST* ticker, XTPRI* error_info, bool is_last)
{
    cout << "OnRspSubMarketData -----" << endl;
    IsErrorRspInfo(error_info,is_last);
    ///����������
    cout << "exchange_id" << ticker->exchange_id << endl;
    ///��Լ���루��������������Ϣ������"600000"�������ո���'\0'��β
    cout << "exchange_id" << ticker->ticker << endl;
}

void MyQuoteSpi::OnUnSubMarketData(XTPST* ticker, XTPRI* error_info, bool is_last)
{
    cout << "OnRspUnSubMarketData -----------" << endl;
    IsErrorRspInfo(error_info,is_last);
    ///����������
    cout << "exchange_id=" << ticker->exchange_id << endl;
    ///��Լ���루��������������Ϣ������"600000"�������ո���'\0'��β
    cout << "ticker=" << ticker->ticker << endl;
}

void MyQuoteSpi::OnDepthMarketData(XTPMD* market_data, int64_t bid1_qty[], int32_t bid1_count, int32_t max_bid1_count, int64_t ask1_qty[], int32_t ask1_count, int32_t max_ask1_count)
{
    cout << "OnDepthMarketData-----------" << endl;
    // ����
    ///����������
    cout << "exchange_id=" << market_data->exchange_id << endl;
    ///��Լ���루��������������Ϣ���������ո���'\0'��β
    cout << "ticker=" << market_data->ticker << endl;

    // ��Ʊ�ȼ۸�
    ///���¼�
    cout << "last_price=" << market_data->last_price << endl;
    ///������
    cout << "pre_close_price=" << market_data->pre_close_price << endl;
    ///����
    cout << "open_price=" << market_data->open_price << endl;
    ///��߼�
    cout << "high_price=" << market_data->high_price << endl;
    ///��ͼ�
    cout << "low_price=" << market_data->low_price << endl;
    ///������
    cout << "close_price=" << market_data->close_price << endl;

    // �ڻ�������
    ///��ֲ�����Ŀǰδ��д��
    cout << "pre_open_interest=" << market_data->pre_open_interest << endl;
    ///�ֲ�����Ŀǰδ��д��
    cout << "open_interest=" << market_data->open_interest << endl;
    ///�ϴν���ۣ�Ŀǰδ��д��
    cout << "pre_settlement_price=" << market_data->pre_settlement_price << endl;
    ///���ν���ۣ�Ŀǰδ��д��
    cout << "settlement_price=" << market_data->settlement_price << endl;

    ///��ͣ��ۣ�Ŀǰδ��д��
    cout << "upper_limit_price=" << market_data->upper_limit_price << endl;
    ///��ͣ��ۣ�Ŀǰδ��д��
    cout << "lower_limit_price=" << market_data->lower_limit_price << endl;
    ///����ʵ�ȣ�Ŀǰδ��д��
    cout << "pre_delta=" << market_data->pre_delta << endl;
    ///����ʵ�ȣ�Ŀǰδ��д��
    cout << "curr_delta=" << market_data->curr_delta << endl;

    /// ʱ���࣬��ʽΪYYYYMMDDHHMMSSsss
    cout << "data_time=" << market_data->data_time << endl;

    // ��������
    ///������Ϊ�ܳɽ�������λ�ɣ��뽻����һ�£�
    cout << "qty=" << market_data->qty << endl;
    ///�ɽ���Ϊ�ܳɽ�����λԪ���뽻����һ�£�
    cout << "turnover=" << market_data->turnover << endl;
    ///���վ���=(turnover/qty)
    cout << "avg_price=" << market_data->avg_price << endl;

    // ������
    ///ʮ�������
    cout << "bid=" << market_data->bid << endl;
    ///ʮ��������
    cout << "ask=" << market_data->ask << endl;
    ///ʮ��������
    cout << "bid_qty=" << market_data->bid_qty << endl;
    ///ʮ��������
    cout << "ask_qty=" << market_data->ask_qty << endl;

    // �������ݣ���������Ŀǰδ��д��
    ///�ɽ�����
    cout << "trades_count=" << market_data->trades_count << endl;
    ///��ǰ����״̬˵��
    cout << "ticker_status=" << market_data->ticker_status << endl;
    ///ί����������
    cout << "total_bid_qty=" << market_data->total_bid_qty << endl;
    ///ί����������
    cout << "total_ask_qty=" << market_data->total_ask_qty << endl;
    ///��Ȩƽ��ί��۸�
    cout << "ma_bid_price=" << market_data->ma_bid_price << endl;
    ///��Ȩƽ��ί���۸�
    cout << "ma_ask_price=" << market_data->ma_ask_price << endl;
    ///ծȯ��Ȩƽ��ί��۸�
    cout << "ma_bond_bid_price=" << market_data->ma_bond_bid_price << endl;
    ///ծȯ��Ȩƽ��ί���۸�
    cout << "ma_bond_ask_price=" << market_data->ma_bond_ask_price << endl;
    ///ծȯ����������
    cout << "yield_to_maturity=" << market_data->yield_to_maturity << endl;
    ///ETF��ֵ��ֵ
    cout << "iopv=" << market_data->iopv << endl;
    ///ETF�깺����
    cout << "etf_buy_count=" << market_data->etf_buy_count << endl;
    ///ETF��ر���
    cout << "etf_sell_count=" << market_data->etf_sell_count << endl;
    ///ETF�깺����
    cout << "etf_buy_qty=" << market_data->etf_buy_qty << endl;
    ///ETF�깺���
    cout << "etf_buy_money=" << market_data->etf_buy_money << endl;
    ///ETF�������
    cout << "etf_sell_qty=" << market_data->etf_sell_qty << endl;
    ///ETF��ؽ��
    cout << "etf_sell_money=" << market_data->etf_sell_money << endl;
    ///Ȩִ֤�е�������
    cout << "total_warrant_exec_qty=" << market_data->total_warrant_exec_qty << endl;
    ///Ȩ֤��ͣ�۸�Ԫ��
    cout << "warrant_lower_price=" << market_data->warrant_lower_price << endl;
    ///Ȩ֤��ͣ�۸�Ԫ��
    cout << "warrant_upper_price=" << market_data->warrant_upper_price << endl;
    ///���볷������
    cout << "cancel_buy_count=" << market_data->cancel_buy_count << endl;
    ///������������
    cout << "cancel_sell_count=" << market_data->cancel_sell_count << endl;
    ///���볷������
    cout << "cancel_buy_qty=" << market_data->cancel_buy_qty << endl;
    ///������������
    cout << "cancel_sell_qty=" << market_data->cancel_sell_qty << endl;
    ///���볷�����
    cout << "cancel_buy_money=" << market_data->cancel_buy_money << endl;
    ///�����������
    cout << "cancel_sell_money=" << market_data->cancel_sell_money << endl;
    ///�����ܱ���
    cout << "total_buy_count=" << market_data->total_buy_count << endl;
    ///�����ܱ���
    cout << "total_sell_count=" << market_data->total_sell_count << endl;
    ///����ί�гɽ����ȴ�ʱ��
    cout << "duration_after_buy=" << market_data->duration_after_buy << endl;
    ///����ί�гɽ����ȴ�ʱ��
    cout << "duration_after_sell=" << market_data->duration_after_sell << endl;
    ///��ί�м�λ��
    cout << "num_bid_orders=" << market_data->num_bid_orders << endl;
    ///����ί�м�λ��
    cout << "num_ask_orders=" << market_data->num_ask_orders << endl;
    ///�ɽ�ʱ�䣨UA3113��
    cout << "exec_time=" << market_data->exec_time << endl;
    ///���б�־��UA103/UA104��
    cout << "is_market_closed=" << market_data->is_market_closed << endl;
    ///��Լ�ֲ�����UA103��
    cout << "total_position=" << market_data->total_position << endl;
    ///��ӯ��1��UA103��
    cout << "pe_ratio1=" << market_data->pe_ratio1 << endl;
    ///��ӯ��2��UA103��
    cout << "pe_ratio2=" << market_data->pe_ratio2 << endl;
}

void MyQuoteSpi::OnSubOrderBook(XTPST* ticker, XTPRI* error_info, bool is_last)
{
    cout << "<<-----" << __FUNCTION__ << endl;
    IsErrorRspInfo(error_info,is_last);
    ///����������
    cout << "exchange_id=" << ticker->exchange_id << endl;
    ///��Լ���루��������������Ϣ������"600000"�������ո���'\0'��β
    cout << "ticker=" << ticker->ticker << endl;
}

void MyQuoteSpi::OnUnSubOrderBook(XTPST* ticker, XTPRI* error_info, bool is_last)
{
    cout << "<<-----" << __FUNCTION__ << endl;
    IsErrorRspInfo(error_info,is_last);
    ///����������
    cout << "exchange_id=" << ticker->exchange_id << endl;
    ///��Լ���루��������������Ϣ������"600000"�������ո���'\0'��β
    cout << "ticker=" << ticker->ticker << endl;
}

void MyQuoteSpi::OnSubTickByTick(XTPST* ticker, XTPRI* error_info, bool is_last)
{
    cout << "<<-----" << __FUNCTION__ << endl;
    IsErrorRspInfo(error_info,is_last);
    ///����������
    cout << "exchange_id=" << ticker->exchange_id << endl;
    ///��Լ���루��������������Ϣ������"600000"�������ո���'\0'��β
    cout << "ticker=" << ticker->ticker << endl;
}

void MyQuoteSpi::OnUnSubTickByTick(XTPST* ticker, XTPRI* error_info, bool is_last)
{
    cout << "<<-----" << __FUNCTION__ << endl;
    IsErrorRspInfo(error_info,is_last);
    ///����������
    cout << "exchange_id=" << ticker->exchange_id << endl;
    ///��Լ���루��������������Ϣ������"600000"�������ո���'\0'��β
    cout << "ticker=" << ticker->ticker << endl;
}

void MyQuoteSpi::OnOrderBook(XTPOB* order_book)
{
    cout << "<<-----" << __FUNCTION__ << endl;

    ///���¼�
    cout << "last_price=" << order_book->last_price << endl;
    ///������Ϊ�ܳɽ���
    cout << "qty=" << order_book->qty << endl;
    ///�ɽ���Ϊ�ܳɽ����
    cout << "turnover=" << order_book->turnover << endl;
    ///�ɽ�����
    cout << "trades_count=" << order_book->trades_count << endl;

    // ������
    ///ʮ�������
    cout << "bid=" << order_book->bid << endl;
    ///ʮ��������
    cout << "ask=" << order_book->ask << endl;
    ///ʮ��������
    cout << "bid_qty=" << order_book->bid_qty << endl;
    ///ʮ��������
    cout << "ask_qty=" << order_book->ask_qty << endl;
    /// ʱ����
    cout << "data_time=" << order_book->data_time << endl;
}

void MyQuoteSpi::OnTickByTick(XTPTBT* tbt_data)
{
    cout << "<<-----" << __FUNCTION__ << endl;
    cout << "ticker=" << tbt_data->ticker << endl;
    ///Ԥ��
    cout << "seq=" << tbt_data->seq << endl;
    ///ί��ʱ�� or �ɽ�ʱ��
    cout << "data_time=" << tbt_data->data_time << endl;
    ///ί�� or �ɽ�
    cout << "type=" << tbt_data->type << endl;

    // union {
    //     XTPTickByTickEntrust entrust;
    //     XTPTickByTickTrade trade;
    // };
}

void MyQuoteSpi::OnQueryAllTickers(XTPQSI* ticker_info, XTPRI* error_info, bool is_last)
{
    cout << "<<-----" << __FUNCTION__ << endl;
    IsErrorRspInfo(error_info,is_last);
    ///����������
    cout << "exchage_id=" << ticker_info->exchange_id << endl;
    ///��Լ���루��������������Ϣ���������ո���'\0'��β
    cout << "ticker=" << ticker_info->ticker << endl;
    /// ��Լ����
    cout << "ticker_name=" << ticker_info->ticker_name << endl;
    /// ��Լ����
    cout << "ticker_type=" << ticker_info->ticker_type << endl;
    ///������
    cout << "pre_close_price=" << ticker_info->pre_close_price << endl;
    ///��ͣ���
    cout << "upper_limit_price=" << ticker_info->upper_limit_price << endl;
    ///��ͣ���
    cout << "lower_limit_price=" << ticker_info->lower_limit_price << endl;
    ///��С�䶯��λ
    cout << "price_tick=" << ticker_info->price_tick << endl;
    /// ��Լ��С������(��)
    cout << "buy_qty_unit=" << ticker_info->buy_qty_unit << endl;
    /// ��Լ��С������(��)
    cout << "sell_qty_unit=" << ticker_info->sell_qty_unit << endl;
}

void MyQuoteSpi::OnSubscribeAllMarketData(XTPRI* error_info)
{
    cout << "<<-----" << __FUNCTION__ << endl;
    IsErrorRspInfo(error_info);
}

void MyQuoteSpi::OnUnSubscribeAllMarketData(XTPRI* error_info)
{
    cout << "<<-----" << __FUNCTION__ << endl;
    IsErrorRspInfo(error_info);
}

void MyQuoteSpi::OnSubscribeAllOrderBook(XTPRI* error_info)
{
    cout << "<<-----" << __FUNCTION__ << endl;
    IsErrorRspInfo(error_info);
}

void MyQuoteSpi::OnUnSubscribeAllOrderBook(XTPRI* error_info)
{
    cout << "<<-----" << __FUNCTION__ << endl;
    IsErrorRspInfo(error_info);
}

void MyQuoteSpi::OnSubscribeAllTickByTick(XTPRI* error_info)
{
    cout << "<<-----" << __FUNCTION__ << endl;
    IsErrorRspInfo(error_info);
}

void MyQuoteSpi::OnUnSubscribeAllTickByTick(XTPRI* error_info)
{
    cout << "<<-----" << __FUNCTION__ << endl;
    IsErrorRspInfo(error_info);
}

void MyQuoteSpi::OnQueryTickersPriceInfo(XTPTPI* ticker_info, XTPRI* error_info, bool is_last)
{
    cout << "<<-----" << __FUNCTION__ << endl;
    IsErrorRspInfo(error_info,is_last);
    ///����������
    cout << "exchange_id=" << ticker_info->exchange_id << endl;
    ///��Լ���루��������������Ϣ���������ո���'\0'��β
    // cout << "ticker=" << ticker_info->ticker[XTP_TICKER_LEN] << endl;
    ///���¼�
    cout << "last_price=" << ticker_info->last_price << endl;
}

bool MyQuoteSpi::IsErrorRspInfo(XTPRI* pRspInfo,bool is_last)
{
    // ���ErrorID != 0, ˵���յ��˴������Ӧ
    bool bResult = ((pRspInfo) && (pRspInfo->error_id != 0));
    if (bResult){
	cout << "--->>> ErrorID=" << pRspInfo->error_id << ", ErrorMsg=" << pRspInfo->error_msg << endl;
	cout << "is_last=" << is_last << endl;
    }
    return bResult;
}

bool MyQuoteSpi::IsErrorRspInfo(XTPRI* pRspInfo)
{
    // ���ErrorID != 0, ˵���յ��˴������Ӧ
    bool bResult = ((pRspInfo) && (pRspInfo->error_id != 0));
    if (bResult){
	cout << "--->>> ErrorID=" << pRspInfo->error_id << ", ErrorMsg=" << pRspInfo->error_msg << endl;
    }
    return bResult;
}
