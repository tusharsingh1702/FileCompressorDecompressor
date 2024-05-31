#include <iostream>
#include <unordered_map>
#include <fstream>
#include <queue>
#include <sstream>
#include <bitset>
#include "Node.h"

using namespace std;

#ifndef HUFFMANCODING_HUFFER_H
#define HUFFMANCODING_HUFFER_H

#define INTERNAL_NODE_CHARACTER char(188)
#define PSEUDO_EOF char(129)
#define CHARACTER_CODE_SEPARATOR char(189)
#define HEADER_ENTRY_SEPARATOR char(190)
#define HEADER_TEXT_SEPARATOR char(191)

class Huffman 
{
private:
    unordered_map<char, string> codeMap; // Map to store Huffman codes

    void generateCodes(Node *rootNode, string codeString)
    {
        if (!rootNode) return;

        if (rootNode->getCharacter() != INTERNAL_NODE_CHARACTER) 
        {
            codeMap[rootNode->getCharacter()] = codeString;
        }
        generateCodes(rootNode->getLeft(), codeString + "0");
        generateCodes(rootNode->getRight(), codeString + "1");
    }

    void writeHeader(ofstream &outputStream)
    {
        for (const auto &item : codeMap)
            outputStream << item.first << CHARACTER_CODE_SEPARATOR << item.second << HEADER_ENTRY_SEPARATOR;
        outputStream << HEADER_TEXT_SEPARATOR;
    }

    void readHeader(ifstream &inputStream)
    {
        codeMap.clear();
        char character;
        inputStream.get(character);
        char key = character;
        while (character != HEADER_TEXT_SEPARATOR) 
        {
            if (character == CHARACTER_CODE_SEPARATOR) 
            {
                inputStream.get(character);
                while (character != HEADER_ENTRY_SEPARATOR) 
                {
                    codeMap[key] += character;
                    inputStream.get(character);
                }
            } 
            else
                key = character;
            inputStream.get(character);
        }
    }

    Node* buildDecodingTree(unordered_map<char, string> encodingMap)
    {
        Node *rootNode = new Node(INTERNAL_NODE_CHARACTER);
        Node *currentNode;

        for (const auto &item : encodingMap) 
        {
            currentNode = rootNode;
            Node *newNode = new Node(item.first);
            string characterCode = item.second;
            for (int i = 0; i < characterCode.size(); ++i) 
            {
                if (characterCode[i] == '0') 
                {
                    if (i == characterCode.size() - 1)
                        currentNode->setLeft(newNode);
                    else 
                    {
                        if (!currentNode->getLeft()) 
                        {
                            currentNode->setLeft(new Node(INTERNAL_NODE_CHARACTER));
                            currentNode = currentNode->getLeft();
                        } 
                        else currentNode = currentNode->getLeft();
                    }
                }
                else 
                {
                    if (i == characterCode.size() - 1)
                        currentNode->setRight(newNode);
                    else 
                    {
                        if (!currentNode->getRight()) 
                        {
                            currentNode->setRight(new Node(INTERNAL_NODE_CHARACTER));
                            currentNode = currentNode->getRight();
                        } 
                        else currentNode = currentNode->getRight();
                    }
                }
            }
        }
        return rootNode;
    }

    class CompareNodes
    {
    public:
        bool operator()(Node* node1, Node* node2)
        {
            return node1->getFrequency() > node2->getFrequency();
        }
    };

public:
    void buildHuffmanTree(unordered_map<char, int> frequencyMap)
    {
        priority_queue<Node*, vector<Node*>, CompareNodes> huffmanQueue;
        string tempString;
        Node *leftNode, *rightNode, *newNode;

        for (const auto &item : frequencyMap)
            huffmanQueue.push(new Node(item.first, item.second));
        
        huffmanQueue.push(new Node(PSEUDO_EOF, 1));

        while (huffmanQueue.size() != 1) 
        {
            leftNode = huffmanQueue.top();
            huffmanQueue.pop();
            rightNode = huffmanQueue.top();
            huffmanQueue.pop();
            newNode = new Node(INTERNAL_NODE_CHARACTER, leftNode->getFrequency() + rightNode->getFrequency());
            huffmanQueue.push(newNode);
            newNode->setLeft(leftNode);
            newNode->setRight(rightNode);
        }

        generateCodes(huffmanQueue.top(), tempString);
    }

    void decompressFromFile(string compressedFileName, string decompressedFileName)
    {
        char character;
        string codeString;
        ifstream inputStream;
        inputStream.open(compressedFileName, ios_base::in | ios::binary);
        readHeader(inputStream);
        while (inputStream.get(character))
        {
            if (character == char_traits<wchar_t>::eof())
                break;
            bitset<8> bits(character);
            codeString += bits.to_string();
        }

        Node *rootNode = buildDecodingTree(codeMap);
        writeDecompressedFile(codeString, rootNode, decompressedFileName);
    }

    void compressToFile(string inputFileName, string outputFileName)
    {
        char character;
        string file;
        ifstream inputStream;
        ofstream outputStream;
        outputStream.open(outputFileName, ios::out);
        inputStream.open(inputFileName, ios::in);
        writeHeader(outputStream);

        while (inputStream.get(character))
            file += codeMap[character];
        
        inputStream.close();
        file += codeMap[PSEUDO_EOF];
        unsigned long remainder = (file.size() - 1) % 8;

        for (int i = 0; i < 8 - remainder; ++i)
            file += '0';
        
        stringstream stringStream(file);
        while (stringStream.good()) 
        {
            bitset<8> bits;
            stringStream >> bits;
            char c = char(bits.to_ulong());
            outputStream << c;
        }

        outputStream.flush();
        outputStream.close();
    }

    void writeDecompressedFile(string codeString, Node *rootNode, string decompressedFileName)
    {
        ofstream outputStream;
        outputStream.open(decompressedFileName, ios::out);
        Node *currentNode = rootNode;

        for (int i = 0; i < codeString.size() + 1; ++i) 
        {
            if (codeString[i] == '0')
                currentNode = currentNode->getLeft();
            else
                currentNode = currentNode->getRight();

            if (currentNode->getCharacter() != INTERNAL_NODE_CHARACTER) 
            {
                if (currentNode->getCharacter() == PSEUDO_EOF)
                    break;
                outputStream << currentNode->getCharacter();
                currentNode = rootNode;
            }
        }

        outputStream.flush();
        outputStream.close();
    }
};

#endif
