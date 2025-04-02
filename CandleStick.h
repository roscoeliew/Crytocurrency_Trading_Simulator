// Candlestick.h
#pragma once
#include <vector>
#include <map>
#include <fstream>
#include <string>

struct Averages {
    double avgOpen;
    double avgHigh;
    double avgLow;
    double avgClose;
};

class Candlestick {
public:
    // Constructor declaration
    Candlestick(const std::string& timestamp, double open, double high, double low, double close);

    // Getter method declarations
    std::string getTimestamp() const;
    double getOpen() const;
    double getHigh() const;
    double getLow() const;
    double getClose() const;

    static void printCandlestickPlot(const std::vector<Candlestick>& candlesticks, const std::string& product);
    static double getMinPrice(const std::vector<Candlestick>& candlesticks);
    static double getMaxPrice(const std::vector<Candlestick>& candlesticks);
    static double calculateScale(const std::vector<Candlestick>& candlesticks);
    static double findNearestPoint(const Candlestick& candle, double priceLevel);

    static std::string formatPrice(double price, const std::string& product);

    struct Averages {
    double avgOpen;
    double avgHigh;
    double avgLow;
    double avgClose;
    };

    static std::vector<Candlestick> readFromCSV(const std::string& filename);
    static std::map<int, Averages> calculateYearlyAverages(const std::vector<Candlestick>& candlesticks);

    static void printLineGraph(const std::vector<Candlestick>& candlesticks); 


private:
    std::string timestamp;
    double open, high, low, close;
};

