//OrderBook.cpp
#include "OrderBook.h"
#include "CSVReader.h"
#include "CandleStick.h"
#include <map>
#include <algorithm>
#include <iostream>
#include <chrono>
#include <sstream>
#include <iomanip>


/** construct, reading a csv data file */
OrderBook::OrderBook(std::string filename)
{
    orders = CSVReader::readCSV(filename);
}

/** return vector of all know products in the dataset*/
std::vector<std::string> OrderBook::getKnownProducts()
{
    std::vector<std::string> products;

    std::map<std::string,bool> prodMap;

    for (OrderBookEntry& e : orders)
    {
        prodMap[e.product] = true;
    }
    
    // now flatten the map to a vector of strings
    for (auto const& e : prodMap)
    {
        products.push_back(e.first);
    }

    return products;
}
/** return vector of Orders according to the sent filters*/
std::vector<OrderBookEntry> OrderBook::getOrders(OrderBookType type, 
                                        std::string product, 
                                        std::string timestamp)
{
    std::vector<OrderBookEntry> orders_sub;
    for (OrderBookEntry& e : orders)
    {
        if (e.orderType == type && 
            e.product == product && 
            e.timestamp == timestamp )
            {
                orders_sub.push_back(e);
            }
    }
    return orders_sub;
}

double OrderBook::getHighPrice(std::vector<OrderBookEntry>& orders)
{
    double max = orders[0].price;
    for (OrderBookEntry& e : orders)
    {
        if (e.price > max)max = e.price;
    }
    return max;
}


double OrderBook::getLowPrice(std::vector<OrderBookEntry>& orders)
{
    double min = orders[0].price;
    for (OrderBookEntry& e : orders)
    {
        if (e.price < min)min = e.price;
    }
    return min;
}

std::string OrderBook::getEarliestTime()
{
    return orders[0].timestamp;
}

std::string OrderBook::getNextTime(const std::string& timestamp)
{
    std::string next_timestamp = "";
    for (OrderBookEntry& e : orders)
    {
        if (e.timestamp > timestamp) 
        {
            next_timestamp = e.timestamp;
            break;
        }
    }
    if (next_timestamp == "")
    {
        //next_timestamp = orders[0].timestamp;
        return next_timestamp;
    }
    return next_timestamp;
}

void OrderBook::insertOrder(OrderBookEntry& order)
{
    orders.push_back(order);
    std::sort(orders.begin(), orders.end(), OrderBookEntry::compareByTimestamp);
}

std::vector<OrderBookEntry> OrderBook::matchAsksToBids(std::string product, std::string timestamp)
{
// asks = orderbook.asks
    std::vector<OrderBookEntry> asks = getOrders(OrderBookType::ask, 
                                                 product, 
                                                 timestamp);
// bids = orderbook.bids
    std::vector<OrderBookEntry> bids = getOrders(OrderBookType::bid, 
                                                 product, 
                                                    timestamp);

    // sales = []
    std::vector<OrderBookEntry> sales; 

    // I put in a little check to ensure we have bids and asks
    // to process.
    if (asks.size() == 0 || bids.size() == 0)
    {
        std::cout << " OrderBook::matchAsksToBids no bids or asks" << std::endl;
        return sales;
    }

    // sort asks lowest first
    std::sort(asks.begin(), asks.end(), OrderBookEntry::compareByPriceAsc);
    // sort bids highest first
    std::sort(bids.begin(), bids.end(), OrderBookEntry::compareByPriceDesc);
    // for ask in asks:
    std::cout << "max ask " << asks[asks.size()-1].price << std::endl;
    std::cout << "min ask " << asks[0].price << std::endl;
    std::cout << "max bid " << bids[0].price << std::endl;
    std::cout << "min bid " << bids[bids.size()-1].price << std::endl;
    
    for (OrderBookEntry& ask : asks)
    {
    //     for bid in bids:
        for (OrderBookEntry& bid : bids)
        {
    //         if bid.price >= ask.price # we have a match
            if (bid.price >= ask.price)
            {
    //             sale = new order()
    //             sale.price = ask.price
            OrderBookEntry sale{ask.price, 0, timestamp, 
                product, 
                OrderBookType::asksale};

                if (bid.username == "simuser")
                {
                    sale.username = "simuser";
                    sale.orderType = OrderBookType::bidsale;
                }
                if (ask.username == "simuser")
                {
                    sale.username = "simuser";
                    sale.orderType =  OrderBookType::asksale;
                }
            
    //             # now work out how much was sold and 
    //             # create new bids and asks covering 
    //             # anything that was not sold
    //             if bid.amount == ask.amount: # bid completely clears ask
                if (bid.amount == ask.amount)
                {
    //                 sale.amount = ask.amount
                    sale.amount = ask.amount;
    //                 sales.append(sale)
                    sales.push_back(sale);
    //                 bid.amount = 0 # make sure the bid is not processed again
                    bid.amount = 0;
    //                 # can do no more with this ask
    //                 # go onto the next ask
    //                 break
                    break;
                }
    //           if bid.amount > ask.amount:  # ask is completely gone slice the bid
                if (bid.amount > ask.amount)
                {
    //                 sale.amount = ask.amount
                    sale.amount = ask.amount;
    //                 sales.append(sale)
                    sales.push_back(sale);
    //                 # we adjust the bid in place
    //                 # so it can be used to process the next ask
    //                 bid.amount = bid.amount - ask.amount
                    bid.amount =  bid.amount - ask.amount;
    //                 # ask is completely gone, so go to next ask                
    //                 break
                    break;
                }


    //             if bid.amount < ask.amount # bid is completely gone, slice the ask
                if (bid.amount < ask.amount && 


                   bid.amount > 0)
                {
    //                 sale.amount = bid.amount
                    sale.amount = bid.amount;
    //                 sales.append(sale)
                    sales.push_back(sale);
    //                 # update the ask
    //                 # and allow further bids to process the remaining amount
    //                 ask.amount = ask.amount - bid.amount
                    ask.amount = ask.amount - bid.amount;
    //                 bid.amount = 0 # make sure the bid is not processed again
                    bid.amount = 0;
    //                 # some ask remains so go to the next bid
    //                 continue
                    continue;
                }
            }
        }
    }
    return sales;             
}

//Retreive the orders for a given product and timestamp
std::vector<OrderBookEntry> OrderBook::getOrdersCandlesticks(OrderBookType type, std::string product, std::string timestamp) {
    std::vector<OrderBookEntry> orders_sub;
    
    for (size_t i = lastProcessedIndex; i < orders.size(); ++i) {
        OrderBookEntry& e = orders[i];

        if (e.orderType == type && e.product == product && e.timestamp == timestamp) {
            orders_sub.push_back(e);
        } else if (e.timestamp != timestamp) {
            lastProcessedIndex = i; // Update lastProcessedIndex when timestamp changes
            break; // Stop iterating as soon as the timestamp is different
        }
    }

    return orders_sub;
}

//Calculate previous Open Price for current Close Price
double OrderBook::getClosePrice(std::vector<OrderBookEntry>& orders)
{
    double cumulativeTotal = 0.0;
    double cumulativeAmount = 0.0;
    
    for (OrderBookEntry& e : orders)
    {
        double totalPrice = e.price * e.amount;
        cumulativeTotal += totalPrice;
        cumulativeAmount += e.amount;
    }

    // Calculate the weighted average for the close price
    if (cumulativeAmount != 0.0) {
        return cumulativeTotal / cumulativeAmount;
    } else {
        return 0.0; // If no transactions, use previous close
    }

}

// Round down timestamp to HH:MM in 30 minute interval
std::string OrderBook::roundDownTimestamp(const std::string& timestamp) {
    std::tm tm = {};
    std::istringstream ss(timestamp);
    ss >> std::get_time(&tm, "%Y/%m/%d %H:%M:%S");

    // Calculate minutes since 11:57 AM
    int minutesSince1157 = (tm.tm_hour - 11) * 60 + tm.tm_min - 57;

    // Calculate the number of 30-minute intervals
    int intervals = minutesSince1157 / 10;

    // Calculate the new timestamp based on intervals
    int newMinutes = 57 + intervals * 10;
    int newHours = 11 + newMinutes / 60;
    newMinutes %= 60;

    // Set the new hour and minute values
    tm.tm_hour = newHours;
    tm.tm_min = newMinutes;
    tm.tm_sec = 0; // Reset seconds to zero

    // Format back into a string
    std::ostringstream result;
    result << std::put_time(&tm, "%Y/%m/%d %H:%M:%S");
    return result.str();
}

//Filter orders by type and product
std::vector<OrderBookEntry> OrderBook::filteredOrders(OrderBookType type, const std::string& product) {
    std::vector<OrderBookEntry> filteredOrders;

    for (auto& order : orders) {
        if (order.product == product  && order.orderType == type) {
            filteredOrders.push_back(order);
        }
    }

    return filteredOrders;
}

//Aggregate candlestick data for 30-minute intervals into candlestick vector
std::vector<Candlestick> OrderBook::aggregateCandlesticks30Min(std::vector<OrderBookEntry>& orders) {
    std::map<std::string, std::vector<OrderBookEntry>> groupedOrders;
    std::vector<Candlestick> aggregatedCandlesticks;
    double previousClose = 0.0;
    bool isFirstCandle = true;

    // Group orders by rounded-down 30-minute intervals
    for (auto& order : orders) {
        std::string roundedTimestamp = roundDownTimestamp(order.timestamp);
        groupedOrders[roundedTimestamp].push_back(order);
    }

    // Calculate candlestick data for each interval
    for (auto& pair : groupedOrders) {
        auto& intervalOrders = pair.second;

        double openPrice = previousClose;
        double closePrice = getClosePrice(intervalOrders);
        double highPrice = getHighPrice(intervalOrders);
        double lowPrice = getLowPrice(intervalOrders);

        if (isFirstCandle && openPrice == 0.0) {
            openPrice = lowPrice;
            isFirstCandle = false;
        }

        //Push to candlestick vector
        aggregatedCandlesticks.emplace_back(pair.first, openPrice, highPrice, lowPrice, closePrice);

        previousClose = closePrice;
    }

    return aggregatedCandlesticks;
}
