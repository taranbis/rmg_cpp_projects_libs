#include "util.hpp"

int main()
{
    std::ifstream in_file("bricks.png", std::ios::binary | std::ios::ate);

    in_file.seekg(0, std::ios::end);
    std::streamsize size = in_file.tellg();
    std::cout << "file size: " << size << std::endl;
    in_file.seekg(0, std::ios::beg);

    // std::stringstream ss;
    // ss << f.rdbuf();

    // std::string s = ss.str();

    // std::cout << s << std::endl;

    in_file.seekg(8);

    // char* data = new char[size];

    //! need to initialize it here to have the default constructed values. With .reserve [data(), data() + size()]
    //! is not a valid range
    std::vector<char> buffer(size - 8);

    try {
        // if (in_file.read((char*)(buffer.data()), size - 8).good()) {
        //     std::cout << "file read" << std::endl;
        //     std::cout << "buffer size: " << buffer.size() << std::endl;
        //     // for (auto it = buffer.begin(); it != buffer.end(); ++it) std::cout << (int)*it << " ";

        // 	std::cout << (int)buffer[4] << std::endl;
        // 	std::cout << (int)buffer[5] << std::endl;
        // 	std::cout << (int)buffer[6] << std::endl;

        // 	std::array<char, 4> chars;
        // 	// std::string s{};
        // 	chars[0]=buffer[4];
        // 	chars[1]=buffer[5];
        // 	chars[2]=buffer[6];
        // 	chars[3]=buffer[7];
        // 	// std::cout << s << std::endl;
        // 	// DEB_BINARY(s);
        // 	// NEWLINE();

        // 	// std::bitset<32> bits{chars.data()};
        // 	std::bitset<32> baz (std::string("001010101"));
        //     std::cout << "bits: " << baz << std::endl;
        // } else {
        //     std::cerr << "Failed to read " << std::endl;
        // }

        char chunkLength{};
        if (in_file.read((char*)(&chunkLength), sizeof(chunkLength)).good()) {
            std::cout << "chunkLength: " << (int)chunkLength << std::endl;
        }
        DEB_BINARY(chunkLength);
        NEWLINE();
        std::cout << (int)chunkLength << std::endl;
        if (in_file.read((char*)(&chunkLength), sizeof(chunkLength)).good()) {
            std::cout << "chunkLength: " << (int)chunkLength << std::endl;
        }
        // DEB_BINARY(chunkLength);
        NEWLINE();
        std::cout << (int)chunkLength << std::endl;
        if (in_file.read((char*)(&chunkLength), sizeof(chunkLength)).good()) {
            std::cout << "chunkLength: " << (int)chunkLength << std::endl;
        }
        // DEB_BINARY(chunkLength);
        NEWLINE();
        std::cout << (int)chunkLength << std::endl;
        if (in_file.read((char*)(&chunkLength), sizeof(chunkLength)).good()) {
            std::cout << "chunkLength: " << (int)chunkLength << std::endl;
        }
        // DEB_BINARY(chunkLength);
        NEWLINE();
        std::cout << (int)chunkLength << std::endl;
        // std::bitset<32> bits;
        // if (in_file.read((char*)(bits), 4).good()){
        // 	std::cout << "bits: " << bits << std::endl;
        // }

        // char c;
        // while (f.get(c))
        // {
        // 	for (int i = 7; i >= 0; i--) // or (int i = 0; i < 8; i++)  if you want reverse bit order in bytes
        // 		cout << ((c >> i) & 1);
        // }
    } catch (const std::exception& e) {
        std::cerr << "Exception! Failed not read: " << e.what() << std::endl;
        return 1;
    }

    // size_t data_size = f.readsome(data, size);
    // std::cout << "Data size: " << data_size << std::endl;

    return EXIT_SUCCESS;
}