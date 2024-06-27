//
//  main.cpp
//  JPEGSingleBlockEncoding
//
//  Created by Norbert Szawloga on 23/06/2024.
//

#include <iostream>
#include <stdlib.h>
#include <vector>
#include "bitset"
#include "CompresserJPG.hpp"
using namespace std;

//prints coefficients, shows how many coefficients are in between the coefficients
//and how many beats long they are
void readBlock(int8_t block[64]){
    
    cout << "DCT: " << block[0] << endl;
    
    int zeroCounter = 0;
    for (int i = 1; i < 64; i++) {
        if (block[i] == 0) {
            zeroCounter++;
            continue;
            
        }
        else{
            cout << zeroCounter << "/" << int(block[i]) << endl;
            zeroCounter = 0;
        }
        
    }
    
    cout << "EOB" << endl;
}

/*
void testBitSet(){
    
    bitset<3> b3(2);
    bitset<2> b2(2);
    
    if (b2 == b3) {
        cout << "BITSETS ARE THE SAME" << endl;
    }else{
        cout << "BITSETS ARE DIFFERENT" << endl;
    }
}*/

const int getNumOfBits(int8_t num){
    
    const size_t sizeBits = sizeof(num) * 8;
    bitset<8> bits(num);
    for (int i = sizeBits - 1; i >= 0 ; i--) {
        if (bits[i] == 0) {
            continue;
        }else{
            return (i + 1);
        }
    }
    
    return 1;
}


/*
bitset<8> getImportantBits(int8_t num){
    
    num = abs(num);
    int size = getNumOfBits(num);
    
    if (size == 1) {
        return bitset<8>(0);
    }
    
    bitset<size> importantBits;
    bitset<8> bits(num);
    for (int i = 0; i < size; i++) {
        importantBits[i] = bits[i];
    }
    
    return importantBits;
    
}*/

string intToBitstring(int8_t num){
    
    
    bitset<8> bits(abs(num));
    
    if (num < 0) {
        bits.flip();
        
    }
    
    return bits.to_string().substr(8-getNumOfBits(abs(num)));
    

}

template <int T>
bitset<T> intToBitset(int8_t num){
    
    bitset<T> bits(abs(num));
    
    if (num < 0) {
        bits.flip();
        
        
    }
    
    return bits;
    
}

bitset<8> getHuffmanSymbol(int zerosCount, int numOfBits){
    bitset<4> zeros(zerosCount);
    bitset<4> length(numOfBits);
    
    
   bitset<8> result;
    
    // Copy bits from the first bitset into the higher part of the result
    for (size_t i = 0; i < 4; ++i) {
        result[i + 4] = zeros[i];
    }
    
    // Copy bits from the second bitset into the lower part of the result
    for (size_t i = 0; i < 4; ++i) {
        result[i] = length[i];
    }
    
    return result;
}
/*
struct node{
    int freq;
    const bitset<8> huffmanSymbol;
    unique_ptr<node>  left = nullptr;
    unique_ptr<node> right = nullptr;
    
    node(int f, const bitset<8>& symbol)
            : freq(f), huffmanSymbol(symbol) {}
};*/

//encodes the block but without the huffman
void encodeBlockNoHuffman(int8_t block[64], vector<unique_ptr<node>>& huffmanSymbols){
    
    cout << "DCT: " << block[0] << endl;
    
    int zeroCounter = 0;
    for (int i = 1; i < 64; i++) {
        if (block[i] == 0) {
            zeroCounter++;
            continue;
            
        }
        else{
            cout << zeroCounter << "/" << getNumOfBits(abs(block[i])) << ";  val: " << int(block[i]) <<  endl;
            cout << "Huffman Symbol: " <<  getHuffmanSymbol( zeroCounter,getNumOfBits(abs(block[i])))<< " binary: " <<  intToBitstring(block[i])<< endl << endl;
            
            bitset<8> currSymbol = getHuffmanSymbol(zeroCounter, getNumOfBits(abs(block[i])));
            
            bool symInNodes = false;
            for (int j = 0; j < huffmanSymbols.size(); j++) {
                if (huffmanSymbols[j]->huffmanSymbol == currSymbol) {
                    symInNodes = true;
                    huffmanSymbols[j]->freq += 1;
                    break;

                }
            }
            if (!symInNodes) {
                huffmanSymbols.push_back(std::make_unique<node>(1, currSymbol));
            }
            
            
            
            zeroCounter = 0;
        }
        
    }
    
    cout << "EOB" << endl;
}

void findCodes(){
    
}



void joinTheLowest(vector<unique_ptr<node>>& nodes){
    
    
    int lowestInd = -1;
    int secondLow = -1;
    int minVal = INT_MAX;
    int secondMinVal = INT_MAX;
    
    for (int i = 0; i < nodes.size(); i++) {
        if (nodes[i]->freq < minVal) {
            secondLow = lowestInd;
            secondMinVal = minVal;
            lowestInd = i;
            minVal = nodes[i]->freq;
        } else if (nodes[i]->freq < secondMinVal) {
            secondLow = i;
            secondMinVal = nodes[i]->freq;
        }
    }
    
    
    int firstIndex = lowestInd < secondLow ? lowestInd : secondLow;
    int secondIndex = lowestInd > secondLow ? lowestInd : secondLow;
    unique_ptr<node> newNode = make_unique<node>((nodes[lowestInd]->freq + nodes[secondLow]->freq), bitset<8>(0));
    newNode->left = std::move(nodes[secondLow]);
    newNode->right = std::move(nodes[lowestInd]);
    nodes.push_back(std::move(newNode));
    
    nodes.erase(nodes.begin() + secondIndex);
    nodes.erase(nodes.begin() + firstIndex);
    
    
}

/*
struct code{
    bitset<8> huffmanSym;
    bitset<8> codeInBits;
    string code;
    int branchDepth;
};
*/

//struct code is outdated below
/*
void calculateCodes(vector<code>& codes, unique_ptr<node>& node, string code, int branchNum){
    
    if (node->left != nullptr) {
        string newCode = code + "0";
        calculateCodes(codes, node->left, newCode, branchNum + 1);
    }
    
    if (node->right != nullptr) {
        string newCode = code + "1";
        calculateCodes(codes, node->right, newCode, branchNum + 1);
    }
    
    if (node->left == nullptr && node->right == nullptr) {
        codes.push_back({node->huffmanSymbol, bitset<8>(0),code, branchNum});
        cout << "Symbol: " << node->huffmanSymbol << " Code: " << code << " Occurances: " << node->freq << " Branch: "<< branchNum<< endl;
        
    }
    
}
*/
bool compareByBranchDepth(const code& a, const code& b) {
    return a.branchDepth < b.branchDepth;

}



int main(int argc, const char * argv[]) {
    
    
    /*
    //8 by 8 qunatisized DCT pixel block written in zigzag order
    int8_t testedBlock[64] = {52, 0, -6, 0, 3, 0, 0, 0,
                            4, -2, 1, 0, -1, 0, -6, 0,
                            5, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0};
    

    
    
    
    //readBlock(testedBlock);
    
    vector<unique_ptr<node>> nodes;
    encodeBlockNoHuffman(testedBlock, nodes);
    
    while (nodes.size() != 1) {
        joinTheLowest(nodes);
    }

    
    unique_ptr<node> head = std::move(nodes[0]);
    vector<code> codes;
    
    calculateCodes(codes, head, "", 0);
    sort(codes.begin(), codes.end(), compareByBranchDepth);
    
    int codeLengthMap[16] = {};
    for (int i = 0; i < codes.size(); i++) {
        codeLengthMap[codes[i].branchDepth-1]++;
    }
    

    
    int currCode = 0;
    short codeCandidate = 0;
    
    for (int i = 0; i < 16; i++) {
        
        
        
        for (int j = 0; j < codeLengthMap[i]; j++) {
            codes[currCode].codeInBits = bitset<8>(codeCandidate);
            
            cout << "Symbol: " << codes[currCode].huffmanSym << " code: " << codes[currCode].codeInBits.to_string().substr(8-codes[currCode].branchDepth) << " code-length: "<< codes[currCode].branchDepth << endl;
            codeCandidate++;
            currCode++;
            
        }
        
        codeCandidate = codeCandidate << 1;
        
        //cout << "Length: " << i+1 << " Num of codes: " << codeLengthMap[i] << endl;
    }
    
  //cout << getImportantBits<numOfBits>(10).to_string() << endl;
    */
    
    CompresserJPG("TestBitMap2.bmp");
    
    
    return 0;
}
