//MerkelMain.cpp
#include "MerkelMain.h"
#include "OrderBookEntry.h"
#include "CandleStick.h"
#include "CSVReader.h"
#include <iostream>
#include <vector>

MerkelMain::MerkelMain()
{

}

void MerkelMain::init()
{
    int input;
    currentTime = orderBook.getEarliestTime();

    wallet.insertCurrency("BTC", 10);

    while(true)
    {
        printMenu();
        input = getUserOption();
        processUserOption(input);
    }
}



void MerkelMain::printMenu()
{
    // 1 print help
    std::cout << "1: Print help " << std::endl;
    // 2 print exchange stats
    std::cout << "2: Print exchange stats" << std::endl;
    // 3 make an offer
    std::cout << "3: Make an offer " << std::endl;
    // 4 make a bid 
    std::cout << "4: Make a bid " << std::endl;
    // 5 print wallet
    std::cout << "5: Print wallet " << std::endl;
    // 6 continue   
    std::cout << "6: Continue " << std::endl;

    std::cout << "7: Print CandleStick " << std::endl;

    std::cout << "8: Print Litecoin Graph " << std::endl;

    std::cout << "============== " << std::endl;

    std::cout << "Current time is: " << currentTime << std::endl;
}

void MerkelMain::printHelp()
{
    std::cout << "Help - your aim is to make money. Analyse the market and make bids and offers. " << std::endl;
}

void MerkelMain::printMarketStats()
{
    for (std::string const& p : orderBook.getKnownProducts())
    {
        std::cout << "Product: " << p << std::endl;
        std::vector<OrderBookEntry> entries = orderBook.getOrders(OrderBookType::ask, 
                                                                p, currentTime);
        std::cout << "Asks seen: " << entries.size() << std::endl;
        std::cout << "Max ask: " << OrderBook::getHighPrice(entries) << std::endl;
        std::cout << "Min ask: " << OrderBook::getLowPrice(entries) << std::endl;



    }
}

void MerkelMain::enterAsk()
{
    std::cout << "Make an ask - enter the amount: product,price, amount, eg  ETH/BTC,200,0.5" << std::endl;
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');
    if (tokens.size() != 3)
    {
        std::cout << "MerkelMain::enterAsk Bad input! " << input << std::endl;
    }
    else {
        try {
            OrderBookEntry obe = CSVReader::stringsToOBE(
                tokens[1],
                tokens[2], 
                currentTime, 
                tokens[0], 
                OrderBookType::ask 
            );
            obe.username = "simuser";
            if (wallet.canFulfillOrder(obe))
            {
                std::cout << "Wallet looks good. " << std::endl;
                orderBook.insertOrder(obe);
            }
            else {
                std::cout << "Wallet has insufficient funds . " << std::endl;
            }
        }catch (const std::exception& e)
        {
            std::cout << " MerkelMain::enterAsk Bad input " << std::endl;
        }   
    }
}

void MerkelMain::enterBid()
{
    std::cout << "Make an bid - enter the amount: product,price, amount, eg  ETH/BTC,200,0.5" << std::endl;
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');
    if (tokens.size() != 3)
    {
        std::cout << "MerkelMain::enterBid Bad input! " << input << std::endl;
    }
    else {
        try {
            OrderBookEntry obe = CSVReader::stringsToOBE(
                tokens[1],
                tokens[2], 
                currentTime, 
                tokens[0], 
                OrderBookType::bid 
            );
            obe.username = "simuser";

            if (wallet.canFulfillOrder(obe))
            {
                std::cout << "Wallet looks good. " << std::endl;
                orderBook.insertOrder(obe);
            }
            else {
                std::cout << "Wallet has insufficient funds . " << std::endl;
            }
        }catch (const std::exception& e)
        {
            std::cout << " MerkelMain::enterBid Bad input " << std::endl;
        }   
    }
}

void MerkelMain::printWallet()
{
    std::cout << wallet.toString() << std::endl;
}
        
void MerkelMain::gotoNextTimeframe()
{
    std::cout << "Going to next time frame. " << std::endl;
    for (std::string p : orderBook.getKnownProducts())
    {
        std::cout << "matching " << p << std::endl;
        std::vector<OrderBookEntry> sales =  orderBook.matchAsksToBids(p, currentTime);
        std::cout << "Sales: " << sales.size() << std::endl;
        for (OrderBookEntry& sale : sales)
        {
            std::cout << "Sale price: " << sale.price << " amount " << sale.amount << std::endl; 
            if (sale.username == "simuser")
            {
                // update the wallet
                wallet.processSale(sale);
            }
        }
        
    }
    
    int timeFrameMinutes = 5;
    currentTime = orderBook.getNextTime(currentTime);
}
 
int MerkelMain::getUserOption()
{
    int userOption = 0;
    std::string line;
    std::cout << "Type in 1-8" << std::endl;
    std::getline(std::cin, line);
    try{
        userOption = std::stoi(line);
    }catch(const std::exception& e)
    {
        // 
    }
    std::cout << "You chose: " << userOption << std::endl;
    return userOption;
}

void MerkelMain::processUserOption(int userOption)
{
    if (userOption == 0) // bad input
    {
        std::cout << "Invalid choice. Choose 1-6" << std::endl;
    }
    if (userOption == 1) 
    {
        printHelp();
    }
    if (userOption == 2) 
    {
        printMarketStats();
    }
    if (userOption == 3) 
    {
        enterAsk();
    }
    if (userOption == 4) 
    {
        enterBid();
    }
    if (userOption == 5) 
    {
        printWallet();
    }
    if (userOption == 6) 
    {
        gotoNextTimeframe();
    }       
    if (userOption == 7) 
    {
        printCandlestick();
    }       
    if (userOption == 8) 
    {
        printLitecoinGraph();
    }  
}

// Written Code without assistance
void MerkelMain::printCandlestick()
{
    std::cout << "Enter Product and Ask/Bid eg, ETH/BTC,Bid" << std::endl;
    std::string input;
    std::getline(std::cin, input);

    std::vector<std::string> tokens = CSVReader::tokenise(input, ',');
    if (tokens.size() != 2)
    {
        std::cout << "MerkelMain::printCandlestick Bad input! " << input << std::endl;
        return;
    }

    std::string product = tokens[0];
    std::string orderTypeStr = tokens[1];
    OrderBookType orderType;

    // Determine if the user wants to plot bid or ask candlesticks
    if (orderTypeStr == "Bid")
    {
        orderType = OrderBookType::bid;
    }
    else if (orderTypeStr == "Ask")
    {
        orderType = OrderBookType::ask;
    }
    else
    {
        std::cout << "Invalid order type. Please specify 'Bid' or 'Ask'." << std::endl;
        return;
    }

    try 
    {
        OrderBook orderBook("20200601.csv"); 

        std::cout << "Calculation for " << orderTypeStr << " is computing for " << product << "\n";
        
        // Compute candlestick data for the selected order type
        std::vector<OrderBookEntry> candlestickOrders = orderBook.filteredOrders(orderType, product);
        
        // Assuming aggregateCandlesticks30Min is a member of OrderBook
        std::vector<Candlestick> aggregatedCandlesticks = orderBook.aggregateCandlesticks30Min(candlestickOrders);

        // Plot Candlestick Data
        Candlestick::printCandlestickPlot(aggregatedCandlesticks, product);

        std::cout << "Calculation for " << orderTypeStr << " is Done for " << product << "\n\n";
    }
    catch (const std::exception& e)
    {
        std::cout << "MerkelMain::printCandlestick error: " << e.what() << std::endl;
    }
}

// Written Code without assistance
void MerkelMain::printLitecoinGraph()
{
    std::string filename = "LTC_BTC.csv"; 

    // Reading the CSV file and storing the data in a vector of Candlesticks
    std::vector<Candlestick> candlesticks = Candlestick::readFromCSV(filename);

    //Calculating yearly averages
    std::map<int, Candlestick::Averages> yearlyAverages = Candlestick::calculateYearlyAverages(candlesticks);

    std::vector<Candlestick> yearlyCandlesticks;
    for (const auto& yearData : yearlyAverages) {
        std::string timestamp = std::to_string(yearData.first) + "-01-01 00:00:00"; // Yearly timestamp
        Candlestick yearlyCandle(
            timestamp,
            yearData.second.avgOpen,
            yearData.second.avgHigh,
            yearData.second.avgLow,
            yearData.second.avgClose
        );
        yearlyCandlesticks.push_back(yearlyCandle);
    }

    Candlestick::printLineGraph(yearlyCandlesticks);
}