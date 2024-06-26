//
//  CompresserJPG.hpp
//  JPEGSingleBlockEncoding
//
//  Created by Norbert Szawloga on 25/06/2024.
//

#pragma once
#include <iostream>
#include <bitset>
#include <vector>
#include <fstream>
#include <cmath>

using namespace std;

struct code{
    bitset<8> huffmanSym;
    bitset<8> codeInBits;
    string code;
    int branchDepth;
};

struct node{
    int freq;
    const bitset<8> huffmanSymbol;
    unique_ptr<node>  left = nullptr;
    unique_ptr<node> right = nullptr;
    
    node(int f, const bitset<8>& symbol)
            : freq(f), huffmanSymbol(symbol) {}
};

struct Block {
    int8_t data[64];
};


class CompresserJPG{
    
private:

    
    
    void readBlock(int8_t block[64]);
    int getNumOfBits(int8_t num);
    string intToBitstring(int8_t num);
    bitset<8> getHuffmanSymbol(int zerosCount, int numOfBits);
    void encodeBlockNoHuffman(int8_t block[64], vector<unique_ptr<node>>& huffmanSymbols);
    void joinTheLowest(vector<unique_ptr<node>>& nodes);
    void calculateCodes(vector<code>& codes, unique_ptr<node>& node, string code, int branchNum);
    bool compareByBranchDepth(const code& a, const code& b);
    
    bool readFile(string imageName, vector<uint8_t>& pixelData);
    
    void arraysToBlock(int width, int height,int8_t* array, vector<Block>& blockStorage);
    
    void performDCT(Block b, int quantTable[64]);
    
    int imageWidth;
    int imageHeight;
    
    uint8_t *channelY;
    uint8_t *channelCr;
    uint8_t *channelCb;
    
    vector<Block> blocksY;
    vector<Block> blocksCb;
    vector<Block> blocksCr;
    
    vector<Block> blocksYDCT;
    vector<Block> blocksCbDCT;
    vector<Block> blocksCrDCT;
    
    
    const int zigzagMap[64] = {
        //map here
    };
    
    int quantTableY[64] = 
    {16,12,14,14,18,24,49,72,
        11,12,13,17,22,35,64,92,
        10,14,16,22,37,55,78,95,
        16,19,24,29,56,64,87,98,
        24,26,40,51,68,81,103,112,
        40,58,57,87,109,104,121,100,
        51,60,69,80,103,113,120,103,
        61,55,56,62,77,92,101,99};
    
    
    /*
    {
        16, 11, 10, 16, 24, 40, 51, 61,
        12, 12, 14, 19, 26, 58, 60, 55,
        14, 13, 16, 24, 40, 57, 69, 56,
        14, 17, 22, 29, 51, 87, 80, 62,
        18, 22, 37, 56, 68, 109, 103, 77,
        24, 35, 55, 64, 81, 104, 113, 92,
        49, 64, 78, 87, 103, 121, 120, 101,
        72, 92, 95, 98, 112, 100, 103, 99
    };
    */
    vector<code> codesAC;
    vector<code> codesDC;
    
    
    
    
    
    
public:
    CompresserJPG(string imageName);
    
    
    
};


#pragma pack(push,1)
struct BMPFileHeader {
    uint16_t fileType;       // File type, always 4D42h ("BM")
    uint32_t fileSize;       // Size of the file in bytes
    uint16_t reserved1;      // Always 0
    uint16_t reserved2;      // Always 0
    uint32_t offsetData;     // Start position of pixel data (bytes from the beginning of the file)
};

// Bitmap info header (40 bytes)
struct BMPInfoHeader {
    uint32_t size;           // Size of this header (40 bytes)
    int32_t width;           // Width of the bitmap in pixels
    int32_t height;          // Height of the bitmap in pixels
    uint16_t planes;         // Number of color planes, must be 1
    uint16_t bitCount;       // Number of bits per pixel
    uint32_t compression;    // Compression type (0 = uncompressed)
    uint32_t sizeImage;      // Size of the image data
    int32_t xPelsPerMeter;   // Horizontal resolution
    int32_t yPelsPerMeter;   // Vertical resolution
    uint32_t colorsUsed;     // Number of colors in the color palette
    uint32_t colorsImportant;// Number of important colors
};
#pragma pack(pop)
