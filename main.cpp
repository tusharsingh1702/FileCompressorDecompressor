#include "FrequencyCounter.h"
#include "Huffman.h"
#include <iostream>
#include <fstream>
#include <ctime>

std::ifstream::pos_type getFileSize(const char* filename);

int main()
{
    FrequencyCounter freqCounter;
    Huffman huffProcessor;
    std::cout << "Hello! Let's start the program!" << std::endl;

    std::string modeChoice;
    std::cout << "What would you like to do today?" << std::endl;
    std::cout << "1. Compress a file" << std::endl;
    std::cout << "2. Decompress a file" << std::endl;
    std::cout << "Please enter 'compress' or 'decompress': ";
    std::cin >> modeChoice;

    if (modeChoice == "Compress")
    {
        std::string inputFileName, outputFileName;
        std::cout << "Enter the name of the file to compress: ";
        std::cin >> inputFileName;
        std::cout << "Enter the name of the output compressed file: ";
        std::cin >> outputFileName;

        clock_t startTime = clock();
        freqCounter.readFile(inputFileName.c_str());
        huffProcessor.buildHuffmanTree(freqCounter.getFrequencyMap());
        huffProcessor.compressToFile(inputFileName.c_str(), outputFileName.c_str());

        std::cout << "Compression completed!" << std::endl;
        std::cout << "Time taken: " << (1.0 * (clock() - startTime) / CLOCKS_PER_SEC) << " seconds" << std::endl;
        std::cout << "Original File Size: " << getFileSize(inputFileName.c_str()) << " bytes" << std::endl;
        std::cout << "Compressed File Size: " << getFileSize(outputFileName.c_str()) << " bytes" << std::endl;
        std::cout << "Compression Ratio: " << (1.0 * getFileSize(outputFileName.c_str()) / getFileSize(inputFileName.c_str())) << std::endl;
    }
    else if (modeChoice == "Decompress")
    {
        std::string inputFileName, outputFileName;
        std::cout << "Enter the name of the compressed file: ";
        std::cin >> inputFileName;
        std::cout << "Enter the name of the output decompressed file: ";
        std::cin >> outputFileName;

        clock_t startTime = clock();
        huffProcessor.decompressFromFile(inputFileName.c_str(), outputFileName.c_str());

        std::cout << "Decompression completed!" << std::endl;
        std::cout << "Time taken: " << (1.0 * (clock() - startTime) / CLOCKS_PER_SEC) << " seconds" << std::endl;
        std::cout << "Compressed File Size: " << getFileSize(inputFileName.c_str()) << " bytes" << std::endl;
        std::cout << "Decompressed File Size: " << getFileSize(outputFileName.c_str()) << " bytes" << std::endl;
    }
    else
    {
        std::cout << "Invalid choice. Please restart the program and choose either 'compress' or 'decompress'." << std::endl;
    }

    return 0;
}

std::ifstream::pos_type getFileSize(const char* filename)
{
    std::ifstream inFile(filename, std::ifstream::ate | std::ifstream::binary);
    return inFile.tellg();
}
