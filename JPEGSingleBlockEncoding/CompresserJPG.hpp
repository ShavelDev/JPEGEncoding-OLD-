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
#include <assert.h>

using namespace std;

struct codeDC{
    int val;
    bitset<16> codeInBits;
    int branchDepth;
};

struct codeAC{
    bitset<8> huffmanSym;
    bitset<16> codeInBits;
    int branchDepth;
};

struct nodeAC{
    int freq;
    const bitset<8> huffmanSymbol;
    unique_ptr<nodeAC>  left = nullptr;
    unique_ptr<nodeAC> right = nullptr;
    
    nodeAC(int f, const bitset<8>& symbol)
            : freq(f), huffmanSymbol(symbol) {}
};

struct nodeDC{
    int freq;
    const int val;
    unique_ptr<nodeDC>  left = nullptr;
    unique_ptr<nodeDC> right = nullptr;
    
    nodeDC(int f, int v)
            : freq(f), val(v) {}
};

struct Block {
    int8_t data[64];
};




class CompresserJPG{
    
public:
    
    
    void writeComponents(ofstream& file, vector<bool> data);
    
    void writeToFile(vector<bool> data );
    void writeToFile2(vector<bool> data );
    void writeToFile3(vector<bool> data);
    
    vector<bool> getCodeBySymbol(bitset<8> symbol);
    vector<bool> getCodeByVal(bitset<8> val);
    
    //returns the number as bits ex: 10 => 1010, -10 => 0101
    vector<bool> getBitsOfVal(int val);
    
    vector<bool> blockToBits(int8_t prevDC,Block b);
    
    void readBlock(int8_t block[64]);
    
    //returns the number as bits ex: 10 => 4, -10 => 4
    int getNumOfBits(int num);
    string intToBitstring(int num);
    bitset<8> getHuffmanSymbol(int zerosCount, int numOfBits);
    void createCodesAC(vector<Block> component, vector<codeAC>& codes);
    void createComponentTreeAC(vector<Block>& component, vector<unique_ptr<nodeAC>>& huffmanSymbols);
    void createComponentTreeDC(vector<Block>& component, vector<unique_ptr<nodeDC>>& huffmanSymbols);
    
    void joinTheLowestAC(vector<unique_ptr<nodeAC>>& nodes);
    void joinTheLowestDC(vector<unique_ptr<nodeDC>>& nodes);
    
    void calculateCodesAC(vector<codeAC>& codes, unique_ptr<nodeAC>& node, int branchNum);
    static bool compareByBranchDepthAC(const codeAC& a, const codeAC& b);
    
    static bool compareByBranchDepthDC(const codeDC& a, const codeDC& b);
    bitset<16> getCodeBySymbol();
    void createCodesDC(vector<Block> component, vector<codeDC>& codes);
    void calculateCodesDC(vector<codeDC>& codes, unique_ptr<nodeDC>& node, int branchNum);
    
    
    bool readFile(string imageName, vector<uint8_t>& pixelData);
    
    void arraysToBlock(int width, int height,int8_t* array, vector<Block>& blockStorage);
    
    void performDCT(Block& b, uint8_t quantTable[64]);
    
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
    
    
    
    uint8_t quantTableY[64] =
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
    vector<codeAC> codesAC;
    vector<codeDC> codesDC;
    
    int zigzagMap[64] = {0, 1, 8, 16, 9, 2, 3, 10,
        17, 24, 32, 25, 18, 11, 4, 5,
        12, 19, 26, 33, 40, 48, 41, 34,
        27, 20, 13, 6, 7, 14, 21, 28,
        35, 42, 49, 56, 57, 50, 43, 36,
        29, 22, 15, 23, 30, 37, 44, 51,
        58, 59, 52, 45, 38, 31, 39, 46,
        53, 60, 61, 54, 47, 55, 62, 63
    };
    
    
    
    
    

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
