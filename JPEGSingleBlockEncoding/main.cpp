//
//  main.cpp
//  JPEGSingleBlockEncoding
//
//  Created by Norbert Szawloga on 23/06/2024.
//

#include <iostream>
#include <stdlib.h>
using namespace std;

//prints coefficients, shows how many coefficients are in between the coefficients
//and how many beats long they are
void readBlock(int block[64]){
    
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

int main(int argc, const char * argv[]) {
    
    //8 by 8 qunatisized DCT pixel block written in zigzag order
    int testedBlock[64] = {52, 0, -6, 0, 3, 0, 0, 0,
                            4, -2, 1, 0, -1, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0,
                            0, 0, 0, 0, 0, 0, 0, 0};
    

    
    
    
    readBlock(testedBlock);
    return 0;
}
