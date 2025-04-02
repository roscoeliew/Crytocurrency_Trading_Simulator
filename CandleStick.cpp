// Candlestick.cpp
#include "CandleStick.h"
#include <iomanip> // For std::setw
#include <algorithm> // For std::max_element and std::min_element
#include <iostream>
#include <sstream>


// Constructor implementation
Candlestick::Candlestick(const std::string& timestamp, double open, double high, double low, double close)
    : timestamp(timestamp), open(open), high(high), low(low), close(close) {}

// Implement getter methods
std::string Candlestick::getTimestamp() const {
    return timestamp;
}

double Candlestick::getOpen() const {
    return open;
}

double Candlestick::getHigh() const {
    return high;
}

double Candlestick::getLow() const {
    return low;
}

double Candlestick::getClose() const {
    return close;
}

//Get min value of candlesticks
double Candlestick::getMinPrice(const std::vector<Candlestick>& candlesticks) {
    double minPrice = candlesticks.front().getLow();
    for (const auto& candle : candlesticks) {
        minPrice = std::min(minPrice, candle.getLow());
    }
    return minPrice;
}

//Get max value of candlesticks
double Candlestick::getMaxPrice(const std::vector<Candlestick>& candlesticks) {
    double maxPrice = candlesticks.front().getHigh();
    for (const auto& candle : candlesticks) {
        maxPrice = std::max(maxPrice, candle.getHigh());
    }
    return maxPrice;
}

//Calculate scale for candlesticks chart
double Candlestick::calculateScale(const std::vector<Candlestick>& candlesticks) {
    double maxPrice = getMaxPrice(candlesticks);
    double minPrice = getMinPrice(candlesticks);
    return (maxPrice - minPrice) / 50;
}

//Written without any assistance
//Pair open,close,high,low values to nearest price level
double Candlestick::findNearestPoint(const Candlestick& candle, double priceLevel) {
    std::vector<double> points = {candle.getOpen(), candle.getClose(), candle.getHigh(), candle.getLow()};
    return *std::min_element(points.begin(), points.end(),
                             [priceLevel](double a, double b) {
                                 return std::abs(a - priceLevel) < std::abs(b - priceLevel);
                             });
}

//Find the nearest price level in Y-Axis
double findNearestLevel(double value, const std::vector<double>& levels) {
    return *std::min_element(levels.begin(), levels.end(), 
                             [value](double a, double b) {
                                return std::abs(a - value) < std::abs(b - value);
                             });
}

const std::string RED = "\033[31m";   // Red for close price
const std::string GREEN = "\033[32m"; // Green for open price
const std::string RESET = "\033[0m";  // Reset to default color

void Candlestick::printCandlestickPlot(const std::vector<Candlestick>& candlesticks, const std::string& product) {
    const int lines = 50; // Number of lines for the plot
    double maxPrice = getMaxPrice(candlesticks);
    double minPrice = getMinPrice(candlesticks);
    double scale = Candlestick::calculateScale(candlesticks);

     // Generate price levels
    std::vector<double> levels;
    for (int i = 0; i <= lines; ++i) {
        levels.push_back(minPrice + i * scale);
    }

    for (int i = lines; i >= 0; --i) {
        double priceLevel = minPrice + i * scale;
        std::string formattedPrice = formatPrice(priceLevel, product); // Format price to fit the column width
        std::cout << formattedPrice << " |";

        // Generate a list of price levels for the Y-axis
        for (const auto& candle : candlesticks) { 
        double nearestOpen = findNearestLevel(candle.getOpen(), levels);
        double nearestClose = findNearestLevel(candle.getClose(), levels);

        // Check if the price level is within the range of open and close
        bool isLevelInRange = (priceLevel >= std::min(nearestOpen,  nearestClose) && 
                               priceLevel <= std::max(nearestOpen,  nearestClose));
            
            if (priceLevel <= candle.getHigh() && priceLevel >= candle.getLow()) {
                // Print '-' for open or close level
                if (isLevelInRange)  {
                    bool closerToOpen = std::abs(priceLevel - nearestOpen) < std::abs(priceLevel - nearestClose);
                    std::string color = closerToOpen ? GREEN : RED;

                    std::cout << color << "  - -  " << RESET;
                } else {
                    // Print '|' otherwise (for high and low)
                    std::cout << "   |   ";
                }
            } else {
                std::cout << std::string(7, ' '); // Empty space
            }
        }
        std::cout << std::endl;
    }
        
    // Print the time axis
    std::cout << std::string(13, ' ') << std::string(125, '-') << "\n";
    std::cout << std::string(13, ' ');
    for (const auto& candle : candlesticks) {
        std::cout << " " << candle.getTimestamp().substr(11, 5) << " "; 
    }
    std::cout << std::endl;
}

// Formats the given price according to the precision required for the product
std::string Candlestick::formatPrice(double price, const std::string& product) {

    std::ostringstream stream;

    if (product == "DOGE/BTC") {
        // Use scientific notation for specific products
        double adjustedPrice = price * 10000000; // Shift decimal 7 places
        stream << std::fixed<< std::setw(11)  << std::setprecision(7) << adjustedPrice;

    } else if (product == "BTC/USDT") {

        stream << std::fixed << std::setw(11) << std::setprecision(2) << price; // 2 decimal places

    }else if (product == "DOGE/USDT") {

        stream << std::fixed << std::setw(11) << std::setprecision(9) << price; // 9 decimal places

    }else if (product == "ETH/BTC") {

        stream << std::fixed << std::setw(11) << std::setprecision(7) << price; // 7 decimal places

    }else if (product == "ETH/USDT") {

        stream << std::fixed << std::setw(11) << std::setprecision(3) << price; // 3 decimal places

    }

    return stream.str();
}

// Retrive data from CSV file
std::vector<Candlestick> Candlestick::readFromCSV(const std::string& filename) {
    std::vector<Candlestick> candlesticksData;
    std::ifstream file(filename);
    std::string line;

    // Skip the header line
    std::getline(file, line);

    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string timestamp, high, low, open, close;

        // Extract candlestick data by splitting the CSV line by commas
        std::getline(ss, timestamp, ',');
        std::getline(ss, high, ',');
        std::getline(ss, low, ',');
        std::getline(ss, open, ',');
        std::getline(ss, close, ',');

        candlesticksData.emplace_back(timestamp, std::stod(open), std::stod(high), std::stod(low), std::stod(close));
    }

    return candlesticksData;
}

// Calculate averages for each year
std::map<int, Candlestick::Averages> Candlestick::calculateYearlyAverages(const std::vector<Candlestick>& candlesticks) {
    std::map<int, std::vector<Candlestick>> groupedByYear;
    std::map<int, Averages> yearlyAverages;

    // Grouping the candlesticks by year
    for (const auto& candlestick : candlesticks) {
        std::string timestamp = candlestick.getTimestamp();
        int year = std::stoi(timestamp.substr(6, 4)); // Extracting year as YYYY from DD/MM/YYYY

        groupedByYear[year].push_back(candlestick);
    }

    // Calculating averages for each year
    for (const auto& pair : groupedByYear) {
        int year = pair.first;
        const auto& candles = pair.second;
        double totalOpen = 0.0, totalHigh = 0.0, totalLow = 0.0, totalClose = 0.0;

        for (const auto& candle : candles) {
            totalOpen += candle.getOpen();
            totalHigh += candle.getHigh();
            totalLow += candle.getLow();
            totalClose += candle.getClose();
        }

        Averages avg; // Store the averages
        avg.avgOpen = totalOpen / candles.size();
        avg.avgHigh = totalHigh / candles.size();
        avg.avgLow = totalLow / candles.size();
        avg.avgClose = totalClose / candles.size();

        yearlyAverages[year] = avg;
    }

    return yearlyAverages;
}

//Written without any assistance
// Print the yearly averages in Line Graph
void Candlestick::printLineGraph(const std::vector<Candlestick>& candlesticks) {
    const int lines = 20; // Number of lines for the plot
    double maxPrice = getMaxPrice(candlesticks);
    double minPrice = getMinPrice(candlesticks);
    double scale = (maxPrice - minPrice) / lines;

     // Generate price levels
    std::vector<double> levels;
    for (int i = 0; i <= lines; ++i) {
        levels.push_back(minPrice + i * scale);
    }

    // Print the Y-axis
    for (int i = lines; i >= 0; --i) {
        double priceLevel = minPrice + i * scale;
        std::cout << std::fixed << std::setw(11) << std::setprecision(7) << priceLevel << " |";

        for (const auto& candle : candlesticks) {
        double nearestOpen = findNearestLevel(candle.getOpen(), levels);
        double nearestClose = findNearestLevel(candle.getClose(), levels);
        double nearestHigh = findNearestLevel(candle.getHigh(), levels);
        double nearestLow = findNearestLevel(candle.getLow(), levels);
        
        // Print the price level symbol by index
        std::string priceSymbols = "      ";

        if (priceLevel == nearestOpen) {
            priceSymbols[1] = 'O'; //index 1
        }
        if (priceLevel == nearestClose) {
            priceSymbols[2] = 'C'; // index 2
        }
        if (priceLevel == nearestHigh) {
            priceSymbols[3] = 'H'; // index 3
        }
        if (priceLevel == nearestLow) {
            priceSymbols[4] = 'L'; // index 4
        }
        std::cout << priceSymbols; 
        }
        std::cout << "\033[0m" << std::endl;
    }
        
    // Print the time axis
    std::cout << std::string(13, ' ') << std::string(80, '-') << "\n";
    std::cout << std::string(13, ' ');
    for (const auto& candle : candlesticks) {
        std::cout << " " << candle.getTimestamp().substr(0, 4) << " "; // Assuming timestamp format is YYYY/MM/DD
    }
   std::cout << std::endl;
}