//
//  main.cpp
//  JPEGSingleBlockEncoding
//
//  Created by Norbert Szawloga on 23/06/2024.
//

#include <iostream>
#include <stdlib.h>
#include "bitset"
using namespace std;

//prints coefficients, shows how many coefficients are in between the coefficients
//and how many beats long they are
void readBlock(short block[64]){
    
    cout << "DCT: " << block[0] << endl;
    
    int zeroCounter = 0;
    for (int i = 1; i < 64; i++) {
        if (block[i] == 0) {
            zeroCounter++;
            continue;
            
        }
        else{
            cout << zeroCounter << "/" << block[i] << endl;
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

constexpr int getNumOfBits(short num){
    
    const size_t sizeBits = sizeof(num) * 8;
    bitset<sizeBits> bits(num);
    for (int i = sizeBits - 1; i >= 0 ; i--) {
        if (bits[i] == 0) {
            continue;
        }else{
            return (i + 1);
        }
    }
    
    return 1;
}


template <int size>
bitset<size> getImportantBits(short num){
    
    num = abs(num);
    
    if (size == 1) {
        return bitset<size>(0);
    }
    
    bitset<size> importantBits;
    bitset<sizeof(num) * 8> bits(num);
    for (int i = 0; i < size; i++) {
        importantBits[i] = bits[i];
    }
    
    return importantBits;
    
}

//encodes the block but without the huffman
void encodeBlockNoHuffman(short block[64]){
    
    cout << "DCT: " << block[0] << endl;
    
    int zeroCounter = 0;
    for (int i = 1; i < 64; i++) {
        if (block[i] == 0) {
            zeroCounter++;
            continue;
            
        }
        else{
            //cout << zeroCounter << "/" << getNumOfBits(block[i]) << ";  val: " << block[i] << " binary: " << getImportantBits<getNumOfBits(block[i])>(block[i]) << endl;
            zeroCounter = 0;
        }
        
    }
    
    cout << "EOB" << endl;
}



int main(int argc, const char * argv[]) {
    
    //8 by 8 qunatisized DCT pixel block written in zigzag order
    short testedBlock[64] = {52, 0, -6, 0, 3, 0, 0, 0,
                            4, -2, 1, 0, -1, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0};
    

    
    
    
    readBlock(testedBlock);
    getImportantBits<numOfBits>(10);
    
    
    return 0;
}
