#include "quote_spi.h"
#include <iostream>
#include <stdio.h>
using namespace std;

void MyQuoteSpi::OnError(XTPRI* error_info, bool is_last)
{
    cerr << "--->>>" << __FUNCTION__ << endl;
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
    cerr << "--->>>" << __FUNCTION__ << endl;
    cerr << "--->>>Reason = " << reason << endl;
    //断线后，可以重新连接
    //重新连接成功后，需要重新向服务器发起订阅请求
}

void MyQuoteSpi::OnSubMarketData(XTPST* ticker, XTPRI* error_info, bool is_last)
{
    cerr << "--->>>" << __FUNCTION__ << endl;
    IsErrorRspInfo(error_info,is_last);
    ///交易所代码
    cerr << "exchange_id=" << ticker->exchange_id << endl;
    ///合约代码（不包含交易所信息）例如"600000"，不带空格，以'\0'结尾
    cerr << "ticker=" << ticker->ticker << endl;
}

void MyQuoteSpi::OnUnSubMarketData(XTPST* ticker, XTPRI* error_info, bool is_last)
{
    cerr << "--->>>" << __FUNCTION__ << endl;
    IsErrorRspInfo(error_info,is_last);
    ///交易所代码
    cerr << "exchange_id=" << ticker->exchange_id << endl;
    ///合约代码（不包含交易所信息）例如"600000"，不带空格，以'\0'结尾
    cerr << "ticker=" << ticker->ticker << endl;
}

void MyQuoteSpi::OnDepthMarketData(XTPMD* market_data, int64_t bid1_qty[], int32_t bid1_count, int32_t max_bid1_count, int64_t ask1_qty[], int32_t ask1_count, int32_t max_ask1_count)
{
    cerr << "--->>>" << __FUNCTION__ << endl;
    cerr << "bid1_qty=" << bid1_qty << endl;
    cerr << "bid1_count=" << bid1_count << endl;
    cerr << "max_bid1_count=" << max_bid1_count << endl;
    cerr << "ask1_qty=" << ask1_qty << endl;
    cerr << "ask1_count=" << ask1_count << endl;
    cerr << "max_ask1_count=" << max_ask1_count << endl;
    // 代码
    ///交易所代码
    cerr << "exchange_id=" << market_data->exchange_id << endl;
    ///合约代码（不包含交易所信息），不带空格，以'\0'结尾
    cerr << "ticker=" << market_data->ticker << endl;

    // 股票等价格
    ///最新价
    cerr << "last_price=" << market_data->last_price << endl;
    ///昨收盘
    cerr << "pre_close_price=" << market_data->pre_close_price << endl;
    ///今开盘
    cerr << "open_price=" << market_data->open_price << endl;
    ///最高价
    cerr << "high_price=" << market_data->high_price << endl;
    ///最低价
    cerr << "low_price=" << market_data->low_price << endl;
    ///今收盘
    cerr << "close_price=" << market_data->close_price << endl;

    // 期货等数据
    ///昨持仓量（目前未填写）
    cerr << "pre_open_interest=" << market_data->pre_open_interest << endl;
    ///持仓量（目前未填写）
    cerr << "open_interest=" << market_data->open_interest << endl;
    ///上次结算价（目前未填写）
    cerr << "pre_settlement_price=" << market_data->pre_settlement_price << endl;
    ///本次结算价（目前未填写）
    cerr << "settlement_price=" << market_data->settlement_price << endl;

    ///涨停板价（目前未填写）
    cerr << "upper_limit_price=" << market_data->upper_limit_price << endl;
    ///跌停板价（目前未填写）
    cerr << "lower_limit_price=" << market_data->lower_limit_price << endl;
    ///昨虚实度（目前未填写）
    cerr << "pre_delta=" << market_data->pre_delta << endl;
    ///今虚实度（目前未填写）
    cerr << "curr_delta=" << market_data->curr_delta << endl;

    /// 时间类，格式为YYYYMMDDHHMMSSsss
    cerr << "data_time=" << market_data->data_time << endl;

    // 量额数据
    ///数量，为总成交量（单位股，与交易所一致）
    cerr << "qty=" << market_data->qty << endl;
    ///成交金额，为总成交金额（单位元，与交易所一致）
    cerr << "turnover=" << market_data->turnover << endl;
    ///当日均价=(turnover/qty)
    cerr << "avg_price=" << market_data->avg_price << endl;

    // 买卖盘
    ///十档申买价
    // TODO: 输出的是地址,需要循环输出具体数据
    cerr << "bid=" << market_data->bid << endl;
    ///十档申卖价
    cerr << "ask=" << market_data->ask << endl;
    ///十档申买量
    cerr << "bid_qty=" << market_data->bid_qty << endl;
    ///十档申卖量
    cerr << "ask_qty=" << market_data->ask_qty << endl;

    // 额外数据（以下数据目前未填写）
    ///成交笔数
    cerr << "trades_count=" << market_data->trades_count << endl;
    ///当前交易状态说明
    cerr << "ticker_status=" << market_data->ticker_status << endl;
    ///委托买入总量
    cerr << "total_bid_qty=" << market_data->total_bid_qty << endl;
    ///委托卖出总量
    cerr << "total_ask_qty=" << market_data->total_ask_qty << endl;
    ///加权平均委买价格
    cerr << "ma_bid_price=" << market_data->ma_bid_price << endl;
    ///加权平均委卖价格
    cerr << "ma_ask_price=" << market_data->ma_ask_price << endl;
    ///债券加权平均委买价格
    cerr << "ma_bond_bid_price=" << market_data->ma_bond_bid_price << endl;
    ///债券加权平均委卖价格
    cerr << "ma_bond_ask_price=" << market_data->ma_bond_ask_price << endl;
    ///债券到期收益率
    cerr << "yield_to_maturity=" << market_data->yield_to_maturity << endl;
    ///ETF净值估值
    cerr << "iopv=" << market_data->iopv << endl;
    ///ETF申购笔数
    cerr << "etf_buy_count=" << market_data->etf_buy_count << endl;
    ///ETF赎回笔数
    cerr << "etf_sell_count=" << market_data->etf_sell_count << endl;
    ///ETF申购数量
    cerr << "etf_buy_qty=" << market_data->etf_buy_qty << endl;
    ///ETF申购金额
    cerr << "etf_buy_money=" << market_data->etf_buy_money << endl;
    ///ETF赎回数量
    cerr << "etf_sell_qty=" << market_data->etf_sell_qty << endl;
    ///ETF赎回金额
    cerr << "etf_sell_money=" << market_data->etf_sell_money << endl;
    ///权证执行的总数量
    cerr << "total_warrant_exec_qty=" << market_data->total_warrant_exec_qty << endl;
    ///权证跌停价格（元）
    cerr << "warrant_lower_price=" << market_data->warrant_lower_price << endl;
    ///权证涨停价格（元）
    cerr << "warrant_upper_price=" << market_data->warrant_upper_price << endl;
    ///买入撤单笔数
    cerr << "cancel_buy_count=" << market_data->cancel_buy_count << endl;
    ///卖出撤单笔数
    cerr << "cancel_sell_count=" << market_data->cancel_sell_count << endl;
    ///买入撤单数量
    cerr << "cancel_buy_qty=" << market_data->cancel_buy_qty << endl;
    ///卖出撤单数量
    cerr << "cancel_sell_qty=" << market_data->cancel_sell_qty << endl;
    ///买入撤单金额
    cerr << "cancel_buy_money=" << market_data->cancel_buy_money << endl;
    ///卖出撤单金额
    cerr << "cancel_sell_money=" << market_data->cancel_sell_money << endl;
    ///买入总笔数
    cerr << "total_buy_count=" << market_data->total_buy_count << endl;
    ///卖出总笔数
    cerr << "total_sell_count=" << market_data->total_sell_count << endl;
    ///买入委托成交最大等待时间
    cerr << "duration_after_buy=" << market_data->duration_after_buy << endl;
    ///卖出委托成交最大等待时间
    cerr << "duration_after_sell=" << market_data->duration_after_sell << endl;
    ///买方委托价位数
    cerr << "num_bid_orders=" << market_data->num_bid_orders << endl;
    ///卖方委托价位数
    cerr << "num_ask_orders=" << market_data->num_ask_orders << endl;
    ///成交时间（UA3113）
    cerr << "exec_time=" << market_data->exec_time << endl;
    ///闭市标志（UA103/UA104）
    cerr << "is_market_closed=" << market_data->is_market_closed << endl;
    ///合约持仓量（UA103）
    cerr << "total_position=" << market_data->total_position << endl;
    ///市盈率1（UA103）
    cerr << "pe_ratio1=" << market_data->pe_ratio1 << endl;
    ///市盈率2（UA103）
    cerr << "pe_ratio2=" << market_data->pe_ratio2 << endl;
}

void MyQuoteSpi::OnSubOrderBook(XTPST* ticker, XTPRI* error_info, bool is_last)
{
    cerr << "--->>>" << __FUNCTION__ << endl;
    IsErrorRspInfo(error_info,is_last);
    ///交易所代码
    cerr << "exchange_id=" << ticker->exchange_id << endl;
    ///合约代码（不包含交易所信息）例如"600000"，不带空格，以'\0'结尾
    cerr << "ticker=" << ticker->ticker << endl;
}

void MyQuoteSpi::OnUnSubOrderBook(XTPST* ticker, XTPRI* error_info, bool is_last)
{
    cerr << "--->>>" << __FUNCTION__ << endl;
    IsErrorRspInfo(error_info,is_last);
    ///交易所代码
    cerr << "exchange_id=" << ticker->exchange_id << endl;
    ///合约代码（不包含交易所信息）例如"600000"，不带空格，以'\0'结尾
    cerr << "ticker=" << ticker->ticker << endl;
}

void MyQuoteSpi::OnSubTickByTick(XTPST* ticker, XTPRI* error_info, bool is_last)
{
    cerr << "--->>>" << __FUNCTION__ << endl;
    IsErrorRspInfo(error_info,is_last);
    ///交易所代码
    cerr << "exchange_id=" << ticker->exchange_id << endl;
    ///合约代码（不包含交易所信息）例如"600000"，不带空格，以'\0'结尾
    cerr << "ticker=" << ticker->ticker << endl;
}

void MyQuoteSpi::OnUnSubTickByTick(XTPST* ticker, XTPRI* error_info, bool is_last)
{
    cerr << "--->>>" << __FUNCTION__ << endl;
    IsErrorRspInfo(error_info,is_last);
    ///交易所代码
    cerr << "exchange_id=" << ticker->exchange_id << endl;
    ///合约代码（不包含交易所信息）例如"600000"，不带空格，以'\0'结尾
    cerr << "ticker=" << ticker->ticker << endl;
}

void MyQuoteSpi::OnOrderBook(XTPOB* order_book)
{
    cerr << "--->>>" << __FUNCTION__ << endl;

    ///最新价
    cerr << "last_price=" << order_book->last_price << endl;
    ///数量，为总成交量
    cerr << "qty=" << order_book->qty << endl;
    ///成交金额，为总成交金额
    cerr << "turnover=" << order_book->turnover << endl;
    ///成交笔数
    cerr << "trades_count=" << order_book->trades_count << endl;

    // 买卖盘
    ///十档申买价
    cerr << "bid=" << order_book->bid << endl;
    ///十档申卖价
    cerr << "ask=" << order_book->ask << endl;
    ///十档申买量
    cerr << "bid_qty=" << order_book->bid_qty << endl;
    ///十档申卖量
    cerr << "ask_qty=" << order_book->ask_qty << endl;
    /// 时间类
    cerr << "data_time=" << order_book->data_time << endl;
}

void MyQuoteSpi::OnTickByTick(XTPTBT* tbt_data)
{
    cerr << "--->>>" << __FUNCTION__ << endl;
    cerr << "ticker=" << tbt_data->ticker << endl;
    ///预留
    cerr << "seq=" << tbt_data->seq << endl;
    ///委托时间 or 成交时间
    cerr << "data_time=" << tbt_data->data_time << endl;
    ///委托 or 成交
    cerr << "type=" << tbt_data->type << endl;

    // union {
    //     XTPTickByTickEntrust entrust;
    //     XTPTickByTickTrade trade;
    // };
}

void MyQuoteSpi::OnQueryAllTickers(XTPQSI* ticker_info, XTPRI* error_info, bool is_last)
{
    cerr << "--->>>" << __FUNCTION__ << endl;
    IsErrorRspInfo(error_info,is_last);
    ///交易所代码
    cerr << "exchage_id=" << ticker_info->exchange_id << endl;
    ///合约代码（不包含交易所信息），不带空格，以'\0'结尾
    cerr << "ticker=" << ticker_info->ticker << endl;
    /// 合约名称
    cerr << "ticker_name=" << ticker_info->ticker_name << endl;
    /// 合约类型
    cerr << "ticker_type=" << ticker_info->ticker_type << endl;
    ///昨收盘
    cerr << "pre_close_price=" << ticker_info->pre_close_price << endl;
    ///涨停板价
    cerr << "upper_limit_price=" << ticker_info->upper_limit_price << endl;
    ///跌停板价
    cerr << "lower_limit_price=" << ticker_info->lower_limit_price << endl;
    ///最小变动价位
    cerr << "price_tick=" << ticker_info->price_tick << endl;
    /// 合约最小交易量(买)
    cerr << "buy_qty_unit=" << ticker_info->buy_qty_unit << endl;
    /// 合约最小交易量(卖)
    cerr << "sell_qty_unit=" << ticker_info->sell_qty_unit << endl;
}

void MyQuoteSpi::OnSubscribeAllMarketData(XTPRI* error_info)
{
    cerr << "--->>>" << __FUNCTION__ << endl;
    IsErrorRspInfo(error_info);
}

void MyQuoteSpi::OnUnSubscribeAllMarketData(XTPRI* error_info)
{
    cerr << "--->>>" << __FUNCTION__ << endl;
    IsErrorRspInfo(error_info);
}

void MyQuoteSpi::OnSubscribeAllOrderBook(XTPRI* error_info)
{
    cerr << "--->>>" << __FUNCTION__ << endl;
    IsErrorRspInfo(error_info);
}

void MyQuoteSpi::OnUnSubscribeAllOrderBook(XTPRI* error_info)
{
    cerr << "--->>>" << __FUNCTION__ << endl;
    IsErrorRspInfo(error_info);
}

void MyQuoteSpi::OnSubscribeAllTickByTick(XTPRI* error_info)
{
    cerr << "--->>>" << __FUNCTION__ << endl;
    IsErrorRspInfo(error_info);
}

void MyQuoteSpi::OnUnSubscribeAllTickByTick(XTPRI* error_info)
{
    cerr << "--->>>" << __FUNCTION__ << endl;
    IsErrorRspInfo(error_info);
}

void MyQuoteSpi::OnQueryTickersPriceInfo(XTPTPI* ticker_info, XTPRI* error_info, bool is_last)
{
    cerr << "--->>>" << __FUNCTION__ << endl;
    IsErrorRspInfo(error_info,is_last);
    ///交易所代码
    cerr << "exchange_id=" << ticker_info->exchange_id << endl;
    ///合约代码（不包含交易所信息），不带空格，以'\0'结尾
    // cerr << "ticker=" << ticker_info->ticker[XTP_TICKER_LEN] << endl;
    ///最新价
    cerr << "last_price=" << ticker_info->last_price << endl;
}

bool MyQuoteSpi::IsErrorRspInfo(XTPRI* pRspInfo,bool is_last)
{
    // 如果ErrorID != 0, 说明收到了错误的响应
    bool bResult = ((pRspInfo) && (pRspInfo->error_id != 0));
    if (pRspInfo){
	cerr << "ErrorID=" << pRspInfo->error_id << ", ErrorMsg=" << pRspInfo->error_msg << endl;
	cerr << "is_last=" << is_last << endl;
    }
    return bResult;
}

bool MyQuoteSpi::IsErrorRspInfo(XTPRI* pRspInfo)
{
    // 如果ErrorID != 0, 说明收到了错误的响应
    bool bResult = ((pRspInfo) && (pRspInfo->error_id != 0));
    if (bResult){
	cerr << "ErrorID=" << pRspInfo->error_id << ", ErrorMsg=" << pRspInfo->error_msg << endl;
    }
    return bResult;
}

