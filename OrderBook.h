//OrderBook.h
#pragma once
#include "OrderBookEntry.h"
#include "CSVReader.h"
#include <string>
#include <vector>
#include "CandleStick.h"

class OrderBook
{
    public:
    /** construct, reading a csv data file */
        OrderBook(std::string filename);
    /** return vector of all know products in the dataset*/
        std::vector<std::string> getKnownProducts();
    /** return vector of Orders according to the sent filters*/
        std::vector<OrderBookEntry> getOrders(OrderBookType type, 
                                              std::string product, 
                                              std::string timestamp);
        
        std::vector<OrderBookEntry> getOrdersCandlesticks(OrderBookType type, 
                                              std::string product, 
                                              std::string timestamp);


        /** returns the earliest time in the orderbook*/
        std::string getEarliestTime();
        /** returns the next time after the 
         * sent time in the orderbook  
         * If there is no next timestamp, wraps around to the start
         * */
        //std::string getNextTime(std::string timestamp);
        std::string getNextTime(const std::string& timestamp);


        void insertOrder(OrderBookEntry& order);

        std::vector<OrderBookEntry> matchAsksToBids(std::string product, std::string timestamp);

        static double getHighPrice(std::vector<OrderBookEntry>& orders);
        static double getLowPrice(std::vector<OrderBookEntry>& orders);
        static double getClosePrice(std::vector<OrderBookEntry>& orders);

        static std::string roundDownTimestamp(const std::string& timestamp);

        std::vector<OrderBookEntry> filteredOrders(OrderBookType type, const std::string& product);
        std::vector<Candlestick> aggregateCandlesticks30Min(std::vector<OrderBookEntry>& orders);

    private:
        std::vector<OrderBookEntry> orders;

        size_t lastProcessedIndex = 0; // Instance variable to keep track of the last processed index

};
