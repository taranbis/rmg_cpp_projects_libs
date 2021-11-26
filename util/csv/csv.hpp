#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include <fstream>
#include <iostream>
#include <boost/lexical_cast.hpp>

namespace io
{
class CSVParser
{
public:
    CSVParser(const std::string& filename, std::string delimiter = ",");
    // CSVParser(const CSVParser &) = delete;
    // CSVParser& operator=(const CSVParser&) = delete;

    void        readHeader();
    bool        hasColumn() const;
    void        parseFile();
    friend void generateMockData(CSVParser& parser);

    CSVParser& operator<<(const std::string& columnName);
    CSVParser& operator<<(std::string&& columnName);

    template <template <class, class> class V, class T, class A>
    CSVParser& operator>>(V<T, A>& container)
    {
        V<T, A> rv;
        for (const auto& x : columns_[outputIdx_]) {
            try {
                rv.push_back(boost::lexical_cast<T>(x));
            } catch (boost::bad_lexical_cast& err) {
                std::cout << "couldn't do the casting" << err.what() << std::endl;
            }
        }
        std::swap(container, rv);
        outputIdx_++;
        return *this;
    }

private:
    std::vector<std::vector<std::string>>     columns_;
    std::unordered_map<std::string, uint64_t> columnIdx_;
    uint64_t                                  inputIdx_ = 0;
    uint64_t                                  outputIdx_ = 0;
    std::ifstream                             file_;
    const std::string                         delimiter_;
    bool                                      headerRead_ = false;
};

void generateMockData(CSVParser& parser);
} // namespace io
