#pragma once
#include ".\api\xtp_trader_api.h"
#include <fstream>

#define _IS_LOG_SHOW_ 0
#define MAX_REQUEST_ID_NUM 10000

using namespace XTP::API;

class MyTraderSpi : public TraderSpi {
public:
    MyTraderSpi();
    ~MyTraderSpi();

    virtual void OnDisconnected(uint64_t session_id, int reason);

    ///����Ӧ��
    virtual void OnError(XTPRI* error_info);

    ///����֪ͨ
    virtual void OnOrderEvent(XTPOrderInfo* order_info, XTPRI* error_info, uint64_t session_id);

    ///�ɽ�֪ͨ
    virtual void OnTradeEvent(XTPTradeReport* trade_info, uint64_t session_id);

    virtual void OnCancelOrderError(XTPOrderCancelInfo* cancel_info, XTPRI* error_info, uint64_t session_id);

    virtual void OnQueryOrder(XTPQueryOrderRsp* order_info, XTPRI* error_info, int request_id, bool is_last, uint64_t session_id);

    virtual void OnQueryTrade(XTPQueryTradeRsp* trade_info, XTPRI* error_info, int request_id, bool is_last, uint64_t session_idt);

    virtual void OnQueryPosition(XTPQueryStkPositionRsp* investor_position, XTPRI* error_info, int request_id, bool is_last, uint64_t session_id);

    virtual void OnQueryAsset(XTPQueryAssetRsp* trading_account, XTPRI* error_info, int request_id, bool is_last, uint64_t session_id);

    void updateOrderNum();
    int getOrderNum();
    void resetOrderNum();

    void updateTradeNum();
    int getTradeNum();
    void resetTradeNum();

    void updateInsertOrderNum();
    int getInsertOrderNum();
    void resetInsertOrderNum();

    void updateCancelOrderNum();
    int getCancelOrderNum();
    void resetCancelOrderNum();

    void OutCount(int val) { outCount = val; }

    int getCurrQuestID();

    void setUserAPI(XTP::API::TraderApi* pApi) { pUserApi = pApi; }

    inline void set_save_to_file(bool save) { save_to_file_ = save; }

private:
    bool IsErrorRspInfo(XTPRI* pRspInfo);

    std::ofstream fout_order;
    std::ofstream fout_trade;
    std::ofstream fout_fund;
    std::ofstream fout_position;
    std::ofstream fout_qry_order;
    std::ofstream fout_qry_trade;
    std::ofstream fout_cancel_order;
    std::ofstream fout_position_count;

    int order_num;
    int trade_num;
    int insert_order_num;
    int cancel_order_num;

    int outCount;

    int m_iquestID;

    bool save_to_file_;

    XTP::API::TraderApi* pUserApi;
};

