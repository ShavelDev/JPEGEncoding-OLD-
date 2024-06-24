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
void readBlock(int8_t block[64]){
    
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

//encodes the block but without the huffman
void encodeBlockNoHuffman(int8_t block[64]){
    
    cout << "DCT: " << block[0] << endl;
    
    int zeroCounter = 0;
    for (int i = 1; i < 64; i++) {
        if (block[i] == 0) {
            zeroCounter++;
            continue;
            
        }
        else{
            cout << zeroCounter << "/" << getNumOfBits(block[i]) << ";  val: " << int(block[i]) << " binary: " <<  intToBitstring(block[i])<< endl;
            zeroCounter = 0;
        }
        
    }
    
    cout << "EOB" << endl;
}



int main(int argc, const char * argv[]) {
    
    //8 by 8 qunatisized DCT pixel block written in zigzag order
    int8_t testedBlock[64] = {52, 0, -6, 0, 3, 0, 0, 0,
                            4, -2, 1, 0, -1, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0};
    

    
    
    
    readBlock(testedBlock);
    encodeBlockNoHuffman(testedBlock);

  //cout << getImportantBits<numOfBits>(10).to_string() << endl;
    
    
    return 0;
}
