#include "csv.hpp"

int main()
{
    io::CSVParser csv("AMZN.csv");

    // Columns to be read can also be specified like this: csv << "Date" << "Open"  <<
    // "High"

    std::vector<std::string> stockDates;
    std::vector<double> openPrices;
    std::vector<double> highPrices;
    std::vector<double> lowPrices;
    std::vector<double> closePrices;

    // io::generateMockData(csv);
    csv.readHeader();
    csv.parseFile();

    csv >> stockDates >> openPrices >> highPrices >> lowPrices >> closePrices;

    for (auto& price : closePrices) { price += 1.1; }

    for (const auto& price : closePrices) { std::cout << price << " "; }
    std::cout << std::endl;

    return 0;
}