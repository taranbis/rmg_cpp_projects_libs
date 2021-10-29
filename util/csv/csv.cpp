#include "csv.hpp"

#include <boost/lexical_cast.hpp>
#include <iostream>
#include <fstream>

#include <util.h>

void io::generateMockData(CSVParser &parser)
{
    parser.columns_[0].emplace_back("1999-01-22");
    parser.columns_[0].emplace_back("1999-01-25");
    parser.columns_[0].emplace_back("1999-01-26");

    parser.columns_[1].emplace_back("1.75");
    parser.columns_[1].emplace_back("1.770833");
    parser.columns_[1].emplace_back("1.833333");
}

io::CSVParser::CSVParser(const std::string &fileName, std::string delimiter)
        : file_(fileName), delimiter_(delimiter)
{
}

void io::CSVParser::readHeader()
{
    headerRead_ = true;
    std::string line;

    std::getline(file_, line);
    size_t      pos = 0;
    std::string headerValue;
    while ((pos = line.find(delimiter_)) != std::string::npos) {
        headerValue = line.substr(0, pos);
        *this << headerValue;
        line.erase(0, pos + delimiter_.length());
    }
}

void io::CSVParser::parseFile()
{
    std::string line;
    if (!headerRead_) { std::getline(file_, line); }

    while (file_.good()) {
        std::getline(file_, line);

        std::size_t pos   = 0;
        std::size_t index = 0;
        std::string value;
        while ((pos = line.find(delimiter_)) != std::string::npos) {
            value = line.substr(0, pos);
            if (index < inputIdx_) { columns_[index].emplace_back(value); }
            ++index;
            line.erase(0, pos + delimiter_.length());
        }
    }
}

io::CSVParser &io::CSVParser::operator<<(const std::string &columnName)
{
    columns_.emplace_back(std::vector<std::string>{});
    columnIdx_.emplace(columnName, inputIdx_);
    inputIdx_++;
    return *this;
}

io::CSVParser &io::CSVParser::operator<<(std::string &&columnName)
{
    columns_.emplace_back(std::vector<std::string>{});
    columnIdx_.emplace(std::move(columnName), inputIdx_);
    inputIdx_++;
    return *this;
}

// template <template <class, class> class V, class T, class A>
// io::CSVParser &io::CSVParser::operator>>(V<T, A> &container)
// {
//     V<T, A> rv;
//     for (const auto &x : columns_[outputIdx_]) {
//         try {
//             rv.push_back(boost::lexical_cast<T>(x));
//         } catch (boost::bad_lexical_cast &err) {
//             std::cout << "couldn't do the casting" << err.what() << std::endl;
//         }
//     }
//     std::swap(container, rv);
//     outputIdx_++;
//     return *this;
// }