#include <unordered_map>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <cstring> 
#include <map>
#include <bitset>

#include "util.h"

std::unordered_map<uint16_t, std::string> markerMapping{
  {0xFFD8, "Start of Image"},  
  {0xFFE0, "Application Default Header"},  
  {0xFFDB, "Quantization Table"},  
  {0xFFC0, "Start of Frame"},  
  {0xFFC4, "Define Huffman Table"},  
  {0xFFDA, "Start of Scan"},  
  {0xFFD9, "End of Image"},
};

class JPEGImage /*  : Image */
{
private:
   std::vector<uint8_t> imgData_;

   //The pair represents the length and the code to be used in the Huffman maps        
   typedef std::pair<int, uint16_t> HuffmanKeyType;

   std::map<HuffmanKeyType, uint8_t> huffmanTables_[32];

public:
    JPEGImage(const char *filename)
    {
        std::ifstream inFile(filename, std::ios::binary | std::ios::ate);
        if (!inFile.good()) { std::cerr << "File not found: " << filename << std::endl; }
        std::streamsize fileSize = inFile.tellg();
        inFile.seekg(0, std::ios::beg);
        imgData_.reserve(fileSize);

        // imgData_.insert(imgData_.begin(), std::istream_iterator<uint16_t>(inFile),
        // std::istream_iterator<uint16_t>());

        // this would have worked for a C style array. the data is still in the vector
        // inFile.read((char*) &imgData_[0], fileSize);

        // read accepts only char*
        // unsigned char data[2];
        unsigned char data;

        // don't put .eof() in while loop; read byte by byte:  while (inFile.good() &&
        // !inFile.eof()); do the read in the while
        while (inFile.read((char *)&data, sizeof(data))) {
            imgData_.emplace_back(data);
        }
    }
private:
    uint16_t ReadBitsFromFile(std::vector<uint8_t>::iterator &iter, int32_t len)
    {
        std::bitset<32> queueVal;
        uint32_t        queueLen;

        uint8_t  readByte;
        uint16_t output;

        if (len > queueLen) {
            do {
                readByte = *iter++;
                queueVal = queueVal | (readByte << (24 - queueLen));
                queueLen += 8;
            } while (len > queueLen)
        }

        output = ((queueVal >> (32 - len)) & ((1 << len) - 1));

        queueLen -= len;
        queueVal <<= len;

        return output;
    }

    int DecodeHuffmanTable(std::vector<uint8_t>::iterator & iter)
    {//pass iterator as reference to be able to move it
        auto tableBeginIter = iter;
        uint16_t code = 0;

        // First byte of a DHT segment is the table ID. between 0 and 31
        uint8_t tableId = *iter++;

        printf("tableId = %d \n", tableId);

        // Next sixteen bytes are the number of values for each code length
        uint8_t counts[16];
        for (int i = 0; i < 16; ++i, ++iter) { 
            counts[i] = *iter; 
        }

        //From now we are building the actual Huffman table used for decoding
        for (int i = 0; i < 16; ++i) {
            for (int j = 0; j < counts[i]; ++j, ++iter, ++code) {
                huffmanTables_[tableId][HuffmanKeyType(i + 1, code)] = (*iter);
            }
            code <<= 1;
        }

        std::map<HuffmanKeyType, uint8_t>::iterator huffmanMapIter;
        for (huffmanMapIter = huffmanTables_[tableId].begin();
             huffmanMapIter != huffmanTables_[tableId].end(); huffmanMapIter++) {
            std::bitset<16> y(huffmanMapIter->first.second);
            std::bitset<8>  z(huffmanMapIter->first.second);

            std::cout << y << " at length " << huffmanMapIter->first.first << " = " << z
                      << std::endl;
        }

        return (iter - tableBeginIter);
    }

public:
    void Decode()
    {
        static int counter = 0;
        auto iter = imgData_.begin();
        while (iter != imgData_.end()) {
            uint16_t marker = (*iter << 8 | *(iter + 1));

            log_hex(marker);
            std::cout << markerMapping[marker] << std::endl;

            if (marker == 0xFFD8) { // Start of Image
                std::advance(iter, 2);
            } else if (marker == 0xFFD9) { // End of Image
                return;
            } else if (marker == 0xFFC4) { // Define Huffman Table
                std::size_t huffmanSegmentLength = *(iter + 2) + *(iter + 3) - 2;
                std::advance(iter, 4);

                if(DecodeHuffmanTable(iter) != huffmanSegmentLength){
                    std::cerr << "Unexpected end of DHT segment" << std::endl;
                    return;
                }

            } else if (marker == 0xFFDA) { //Start of Scan
                std::advance(iter, (imgData_.end() - iter) - 2);
            } else {
                std::size_t chunkLength = *(iter + 2) + *(iter + 3);
                std::advance(iter, 2 + chunkLength);
            }

            // counter++;
            // if (counter == 10) { break; }
        }
    }
};

int main()
{
    JPEGImage img = JPEGImage("test.jpg");
    img.Decode();

    return 0;
}
