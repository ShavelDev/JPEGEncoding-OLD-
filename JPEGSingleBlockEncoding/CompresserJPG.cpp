//
//  CompresserJPG.cpp
//  JPEGSingleBlockEncoding
//
//  Created by Norbert Szawloga on 25/06/2024.
//

#include "CompresserJPG.hpp"
#include <vector>



bool CompresserJPG::readFile(string imageName, vector<uint8_t>& pixelData){
    ifstream file(imageName, std::ios::binary);
    
    if (!file) {
        cerr << "File could not be opened" << endl;
        return 0;
    }
    
    BMPFileHeader fileHeader;
    file.read(reinterpret_cast<char*>(&fileHeader), 14);
    
    if (fileHeader.fileType == 0x244D) {
        cerr << "Error: Not a BMP file." << endl;
        return 0;
    }
   
    
    BMPInfoHeader infoHeader;
    file.read(reinterpret_cast<char*>(&infoHeader), 40);
    
    imageWidth = infoHeader.width;
    imageHeight = infoHeader.height;
    
    if (imageWidth % 8 != 0 || imageHeight % 8 != 0) {
        cerr << "Error: Wrong Image Size" << endl;
    }
    
    int bitCount = infoHeader.bitCount;
    
    cout << "Offset Data: "<< fileHeader.offsetData << endl;
    cout << "width: "<< (int)infoHeader.width << endl;
    cout << "height: "<< (int)infoHeader.height << endl;
    
    
    
    cout << "Size: " << infoHeader.sizeImage << endl;
    int rowPadded = (imageWidth * bitCount / 8 + 3) & (~3);
    pixelData.resize(infoHeader.sizeImage);
    file.seekg(fileHeader.offsetData, ios::beg);
    
    cout << "SIZE OF VECTOR" << pixelData.size() << endl;
    
    for (int i = 0; i < imageHeight; ++i) {
        file.read(reinterpret_cast<char*>(&pixelData[(imageHeight - 1 - i) * rowPadded]), rowPadded);
    }
    
    
//    file.read(reinterpret_cast<char*>(pixelData.data()), infoHeader.sizeImage);
//    reverse(pixelData.begin(), pixelData.end());
    
    file.close();
    
    
    
    
    return 1;
    
}



void CompresserJPG::arraysToBlock(int width, int height, int8_t* array, vector<Block>& blockStorage){
    
    int blocksInX = width/8;
    int blocksInY = height/8;
    cout << "NumOfBlocks: x: " << blocksInX << " y: " << blocksInY << endl;
    
    blockStorage.resize(blocksInX * blocksInY);
    
    
    for (int yB = 0; yB < blocksInY; yB++) {
                for (int xB = 0; xB < blocksInX; xB++) {
                    for (int y = 0; y < 8; y++) {
                        for (int x = 0; x < 8; x++) {
                            blockStorage[xB + yB * blocksInX].data[x + 8 * y] = array[(xB * 8 + x) + (yB * 8 + y) * width];
                        }
                    }
                }
            }
    
    
    //test for the first block
    
    for (int i = 0; i < 64; i++) {
        cout << "ARRAY: "<< i << " VAL: "<< (int)blockStorage[0].data[i] << endl;
    }

}

void performDCT(Block b){
    
    int DctCoeff[64];
    
    cout << "BEFORE: " << endl;
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            cout <<  (int) b.data[x + 8*y] << " " ;
        }
        cout << endl;
    }
    
    
    for (int yC = 0; yC < 8; yC++) {
        for (int xC = 0; xC < 8; xC++) {
            
            float currCoeff  = 0;
            
            for (int y = 0; y < 8; y++) {
                for (int x = 0; x < 8; x++) {
                    float coeffX = (xC == 0) ? 0.707107 : 1;
                    float coeffY = (yC == 0) ? 0.707107: 1;
                    
                    currCoeff += ((float)b.data[x + 8*y]) * coeffX*coeffY * cos((2*x+1)*xC*M_PI/16) * cos((2*y+1)*yC*M_PI/16)/4;
                    
                    
                }
            }
            
            DctCoeff[xC+8*yC] = (int)currCoeff;
            
        }
        
        
        
        
        
    }
    
    cout << "After: " << endl;
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            cout <<  (int) DctCoeff[x + 8*y] << " " ;
        }
        cout << endl;
    }
    
}
CompresserJPG::CompresserJPG(string imageName){
    vector<uint8_t> pixelData;
    
    readFile(imageName, pixelData);
    
    
    
    int imageSize = imageWidth * imageHeight;
    
    int8_t arrY[imageSize];
    int8_t arrCb[imageSize];
    int8_t arrCr[imageSize];
    
    cout << "ImageSize: "<< imageSize << endl;
    for (int i = 0; i < imageSize; i++) {
        uint8_t blue = pixelData[3*i];
        uint8_t green = pixelData[3*i+1];
        uint8_t red = pixelData[3*i+2];
        
        
        
        cout << "Red: " << (int)red << " Green: " << (int)green << " Blue: " << (int)blue<< endl;
        
        int8_t Y = (int8_t)(0.114 * (float)blue + 0.587 * (float)green + 0.299 * (float) red - (int)128);
        int8_t Cb = (int8_t)(0.500 * (int)blue + (-0.331) * (int)green + (-0.169) * (int) red);
        int8_t Cr = (int8_t)((-0.081) * (float)blue + (-0.419) * (float)green + (0.500) * (float) red);
        
        cout << "Y: " << (int)Y << " Cb: " << (int)Cb << " Cr: " << (int)Cr << endl;
        
        
        arrY[i] = Y;
        arrCb[i] = Cb;
        arrCr[i] = Cr;
        
        
        
    }
    
    arraysToBlock(imageWidth, imageHeight, arrY, blocksY);
    arraysToBlock(imageWidth, imageHeight, arrCb, blocksCb);
    arraysToBlock(imageWidth, imageHeight, arrCr, blocksCr);
    
    
    //Block test = {{-76, -73, -67, -62, -58, -67, -64, -55, -65, -69, -73, -38, -19, -43, -59, -56, -66, -69, -60, -15, 16, -24, -62, -55, -65, -70, -57, -6, 26, -22, -58, -59, -61, -67, -60, -24, -2, -40, -60, -58, -49, -63, -68, -58, -51, -60, -70, -53, -43, -57, -64, -69, -73, -67, -63, -45, -41, -49, -59, -60, -63, -52, -50, -34}};
    
    Block test = {{
        -66, -73, -73, -74, -79, -80, -81, -73,
        -66, -71, -74, -76, -80, -81, -80, -75,
        -67, -68, -76, -79, -80, -81, -79, -74,
        -65, -67, -68, -68, -65, -63, -60, -63,
        -61, -61, -58, -54, -49, -43, -37, -36,
        -46, -33, -27, -22, -14, -13, -16, -11,
        -32, -17, -13, -9, 0, 0, 2, -1,
        -19, -7, -1, 5, 11, 13, 12, 5
    }};
    performDCT(test);
    
}
