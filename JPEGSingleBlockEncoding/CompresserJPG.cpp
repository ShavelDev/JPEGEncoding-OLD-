//
//  CompresserJPG.cpp
//  JPEGSingleBlockEncoding
//
//  Created by Norbert Szawloga on 25/06/2024.
//

#include "CompresserJPG.hpp"
#include <vector>

//use only for num > abs


int CompresserJPG::getNumOfBits(int num){
    
    num = abs(num);
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

/*
string CompresserJPG::intToBItstring(int num){
    
    
    const size_t sizeBits = sizeof(abs(num))*8;
    bitset<sizeBits> bits(num);
    
    if (num < 0) {
        bits.flip();
        
    }
    
    return bits.to_string().substr(8-getNumOfBits(abs(num)));
    
}*/



bitset<8> CompresserJPG::getHuffmanSymbol(int zerosCount, int numOfBits){
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



void CompresserJPG::createCodesDC(vector<Block> component, vector<codeDC>& codes){
    vector<unique_ptr<nodeDC>> nodes;
    createComponentTreeDC(component, nodes);
    
    while (nodes.size() != 1) {
        joinTheLowestDC(nodes);
    }
    
    unique_ptr<nodeDC> head = std::move(nodes[0]);
    
    calculateCodesDC(codes, head, 0);
    sort(codes.begin(), codes.end(), compareByBranchDepthDC);
    
    int codeLengthMap[16] = {};
    for (int i= 0; i < codes.size(); i++) {
        codeLengthMap[codes[i].branchDepth-1]++;
    }
    
    int currCode = 0;
    short codeCandidate = 0;
    
    for (int i = 0; i < 16; i++) {
        
        
        
        for (int j = 0; j < codeLengthMap[i]; j++) {
            codes[currCode].codeInBits = bitset<16>(codeCandidate);
            
            cout << "Val: " << codes[currCode].val << " code: " << codes[currCode].codeInBits.to_string().substr(16-codes[currCode].branchDepth) << " code-length: "<< codes[currCode].branchDepth << endl;
            codeCandidate++;
            currCode++;
            
        }
        
        codeCandidate = codeCandidate << 1;
        
        //cout << "Length: " << i+1 << " Num of codes: " << codeLengthMap[i] << endl;
    }

    
    
    
}



void CompresserJPG::createCodesAC(vector<Block> component, vector<codeAC>& codes){
    vector<unique_ptr<nodeAC>> nodes;
    createComponentTreeAC(component, nodes);
    
    while (nodes.size() != 1) {
        joinTheLowestAC(nodes);
    }
    
    unique_ptr<nodeAC> head = std::move(nodes[0]);
    
    calculateCodesAC(codes, head, 0);
    sort(codes.begin(), codes.end(), compareByBranchDepthAC);
    
    int codeLengthMap[16] = {};
    for (int i= 0; i < codes.size(); i++) {
        codeLengthMap[codes[i].branchDepth-1]++;
    }
    
    int currCode = 0;
    short codeCandidate = 0;
    
    for (int i = 0; i < 16; i++) {
        
        
        
        for (int j = 0; j < codeLengthMap[i]; j++) {
            codes[currCode].codeInBits = bitset<16>(codeCandidate);
            
            cout << "Symbol: " << codes[currCode].huffmanSym << " code: " << codes[currCode].codeInBits.to_string().substr(16-codes[currCode].branchDepth) << " code-length: "<< codes[currCode].branchDepth << endl;
            codeCandidate++;
            currCode++;
            
        }
        
        codeCandidate = codeCandidate << 1;
        
        //cout << "Length: " << i+1 << " Num of codes: " << codeLengthMap[i] << endl;
    }

    
    
    
}
void CompresserJPG::createComponentTreeAC(vector<Block>& component, vector<unique_ptr<nodeAC>>& nodes){
    
    //add EOB because there will always be at least one
    nodes.push_back(std::make_unique<nodeAC>(component.size(), bitset<8>(0)));
    
    
    for (int k = 0; k < component.size(); k++) {
        
        int zeroCounter = 0;
        for (int i = 1; i < 64; i++) {
            
            //USE ZIGZAG HERE zigzag[i]
            int currVal = component[k].data[zigzagMap[i]];

            if (currVal == 0) {
                zeroCounter++;
                continue;
                
            }
            else{
                cout << zeroCounter << "/" << getNumOfBits(currVal) << ";  val: " << currVal <<  endl;
                cout << "Huffman Symbol: " <<  getHuffmanSymbol( zeroCounter,getNumOfBits(currVal))<< " binary: " <<  intToBitstring(currVal)<< endl << endl;
                
                bitset<8> currSymbol = getHuffmanSymbol(zeroCounter, getNumOfBits(currVal));
                
                bool symInNodes = false;
                for (int j = 0; j < nodes.size(); j++) {
                    if (nodes[j]->huffmanSymbol == currSymbol) {
                        symInNodes = true;
                        nodes[j]->freq += 1;
                        break;
                        
                    }
                }
                if (!symInNodes) {
                    nodes.push_back(std::make_unique<nodeAC>(1, currSymbol));
                }
                
                
                
                zeroCounter = 0;
            }
            
        }
        
    }
    
}

void CompresserJPG::createComponentTreeDC(vector<Block>& component, vector<unique_ptr<nodeDC>>& nodes){
    
    int currCoeff = component[0].data[0];
    nodes.push_back(std::make_unique<nodeDC>(1, getNumOfBits(currCoeff)));
    
    for (int i = 1; i < component.size(); i++) {
        currCoeff = component[i].data[0] - component[i-1].data[0];
        bool valInNodes = false;
        for (int j = 0; j < nodes.size(); j++) {
            if (nodes[j]->val == getNumOfBits(currCoeff)) {
                nodes[j]->freq += 1;
                valInNodes = true;
                break;
            }
        }
        if (!valInNodes) {
            nodes.push_back(std::make_unique<nodeDC>(1,getNumOfBits(currCoeff)));
        }
    }
    
}

string CompresserJPG::intToBitstring(int num){
    return "";
}


void CompresserJPG::joinTheLowestAC(vector<unique_ptr<nodeAC>>& nodes){
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
    unique_ptr<nodeAC> newNode = make_unique<nodeAC>((nodes[lowestInd]->freq + nodes[secondLow]->freq), bitset<8>(0));
    newNode->left = std::move(nodes[secondLow]);
    newNode->right = std::move(nodes[lowestInd]);
    nodes.push_back(std::move(newNode));
    
    nodes.erase(nodes.begin() + secondIndex);
    nodes.erase(nodes.begin() + firstIndex);
}

void CompresserJPG::joinTheLowestDC(vector<unique_ptr<nodeDC>>& nodes){
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
    unique_ptr<nodeDC> newNode = make_unique<nodeDC>((nodes[lowestInd]->freq + nodes[secondLow]->freq), 0);
    newNode->left = std::move(nodes[secondLow]);
    newNode->right = std::move(nodes[lowestInd]);
    nodes.push_back(std::move(newNode));
    
    nodes.erase(nodes.begin() + secondIndex);
    nodes.erase(nodes.begin() + firstIndex);
}

void CompresserJPG::calculateCodesDC(vector<codeDC>& codes, unique_ptr<nodeDC>& node, int branchNum){
    if (node->left != nullptr) {
        calculateCodesDC(codes, node->left, branchNum + 1);
    }
    
    if (node->right != nullptr) {
        calculateCodesDC(codes, node->right, branchNum + 1);
    }
    
    if (node->left == nullptr && node->right == nullptr) {
        codes.push_back({node->val, bitset<16>(0), branchNum});
        cout << "Val: " << node->val << " Occurances: " << node->freq << " Branch: "<< branchNum<< endl;
        
    }
    
}



void CompresserJPG::calculateCodesAC(vector<codeAC>& codes, unique_ptr<nodeAC>& node, int branchNum){
    if (node->left != nullptr) {
        calculateCodesAC(codes, node->left, branchNum + 1);
    }
    
    if (node->right != nullptr) {
        calculateCodesAC(codes, node->right, branchNum + 1);
    }
    
    if (node->left == nullptr && node->right == nullptr) {
        codes.push_back({node->huffmanSymbol, bitset<16>(0), branchNum});
        cout << "Symbol: " << node->huffmanSymbol  << " Occurances: " << node->freq << " Branch: "<< branchNum<< endl;
        
    }
    
}
bool CompresserJPG::compareByBranchDepthAC(const codeAC& a, const codeAC& b){
    return a.branchDepth < b.branchDepth;
}

bool CompresserJPG::compareByBranchDepthDC(const codeDC& a, const codeDC& b){
    return a.branchDepth < b.branchDepth;
}



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

void CompresserJPG::performDCT(Block& b, uint8_t quantTable[64]){
    
    int DctCoeff[64];
    
    for (int i = 0; i < 64; ++i) {
            DctCoeff[i] = static_cast<int>(b.data[i]);
       }
    
    
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
                    
                    currCoeff += ((float)DctCoeff[x + 8*y]) * coeffX*coeffY * cos((2*x+1)*xC*M_PI/16) * cos((2*y+1)*yC*M_PI/16)/4;
                    
                    
                }
            }
            
            b.data[xC+8*yC] = static_cast<int>(std::round(currCoeff/(float)quantTable[xC+8*yC])); //(int)(currCoeff/(float)quantTable[xC+8*yC]);
            
        }
        
    }
    
    cout << "After: " << endl;
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            cout <<  (int) b.data[x + 8*y] << " " ;
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
    
    /*
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
    performDCT(test, quantTableY);
    */
    
    
    
    
    
    for (int i = 0; i < blocksY.size(); i++) {
        performDCT(blocksY[i], quantTableY);
    }
    
    createCodesAC(blocksY, codesAC);
    
    cout << "AC CODES" << endl<< endl;
    for (int i = 0; i < codesAC.size(); i++) {
        
        cout << "Huffman symbol: " << codesAC[i].huffmanSym.to_string()<< endl;
        cout << "Code: " << codesAC[i].codeInBits.to_string().substr(16 - codesAC[i].branchDepth)<< endl << endl;
    }
    
    createCodesDC(blocksY, codesDC);
    
    
    
    cout << "DC Coeff:" << endl << endl;
    for (int i = 0; i < blocksY.size(); i++) {
        
        cout << "Val: " << (int) blocksY[i].data[0]<< endl;

    }
    cout << "DC CODES" << endl<< endl;
    for (int i = 0; i < codesDC.size(); i++) {
        
        cout << "Val: " << codesDC[i].val<< endl;
        cout << "Code: " << codesDC[i].codeInBits.to_string().substr(16 - codesDC[i].branchDepth)<< endl << endl;
    }
    
    //turncodes into writeable table
    
    //
    
    cout << "getBitsOfNum: " << endl;
    vector<bool> test = getBitsOfVal(-255);
    for (int i = 0; i < test.size(); i++) {
        cout << test[i];
    }
    cout << endl;
    
    cout<< "getCodeByVal: "  << endl;
    test = getCodeByVal(2);
    for (int i = 0; i < test.size(); i++) {
        cout << test[i];
    }
    cout << endl;
    
    vector<bool> data;
    for (int i = 0; i < blocksY.size(); i++) {
        int prevDC = i == 0 ? 0 : blocksY[i-1].data[0];
        vector<bool> blockData = blockToBits(prevDC, blocksY[i]);
        data.insert(data.begin(), blockData.begin(), blockData.end());
    }
    
    writeToFile(data);
    
}

vector<bool> CompresserJPG::getCodeBySymbol( bitset<8> symbol){
    
    for (int i = 0; i < codesAC.size(); i++) {
        if (codesAC[i].huffmanSym == symbol) {
            vector<bool> res;
            for (int j = codesAC[i].branchDepth-1; j >=0; j--) {
                res.push_back(codesAC[i].codeInBits[j]);
            }
            return res;
        }
    }
    
    assert(false);
    
}

vector<bool> CompresserJPG::getCodeByVal( int val){
    
    for (int i = 0; i < codesDC.size(); i++) {
        if (codesDC[i].val == val) {
            vector<bool> res;
            for (int j = codesDC[i].branchDepth-1; j >=0; j--) {
                res.push_back(codesDC[i].codeInBits[j]);
            }
            return res;
        }
    }
    
    cout << "value: " << val << endl;
    assert(false);
    
}

vector<bool> CompresserJPG::getBitsOfVal(int val){
    
    bitset<32> bits(abs(val));
    for (int i = 31; i >= 0 ; i--) {
        if (bits[i] != 0) {
            vector<bool> res;
            for (int j = i; j >= 0 ; j--) {
                bool bit = val > 0 ? bits[j] : !bits[j];
                res.push_back(bit);
            }
            return res;
        }
        
    }
    
    
    
    assert(false);
}

vector<bool> CompresserJPG::blockToBits(int8_t prevDC, Block b){
    vector<bool> bits;
    vector<bool> codeDC = getCodeByVal(getNumOfBits((int)b.data[0]-(int)prevDC));
    vector<bool> coeffDC = getBitsOfVal((int)b.data[0]-(int)prevDC);
    codeDC.insert(codeDC.begin(), coeffDC.begin(),coeffDC.end());
    bits.insert(bits.begin(), codeDC.begin(), codeDC.end());
    
    
    int zeroCounter = 0;
    for (int i = 1; i < 64; i++) {
        
        //USE ZIGZAG HERE zigzag[i]
        int currVal = b.data[zigzagMap[i]];

        if (currVal == 0) {
            zeroCounter++;
            continue;
            
        }
        else{
            
            bitset<8> currSymbol = getHuffmanSymbol(zeroCounter, getNumOfBits(currVal));
            
            vector<bool> bitsSymbol = getCodeBySymbol(currSymbol);
            
            bits.insert(bits.begin(), bitsSymbol.begin(), bitsSymbol.end());
            
            
            zeroCounter = 0;
        }
        
    }
    
    
    
    
    //EOB
    vector<bool> bitsEOB = getCodeBySymbol(bitset<8>(0));
    
    bits.insert(bits.begin(), bitsEOB.begin(), bitsEOB.end());
    
    return bits;
}

void write16(std::ofstream& file, uint16_t value) {
    file.put(static_cast<char>((value >> 8) & 0xFF));
    file.put(static_cast<char>(value & 0xFF));
}

// Function to write an 8-bit value
void write8(std::ofstream& file, uint8_t value) {
    file.put(static_cast<char>(value));
}

void CompresserJPG::writeToFile(vector<bool> data){
    std::ofstream jpegFile("output.jpg", std::ios::binary);
    
    if (!jpegFile) {
        std::cerr << "Could not open the file for writing." << std::endl;
        assert(false);
        
    }
    
    // JPEG SOI marker (Start of Image)
    write16(jpegFile, 0xFFD8);
    
    // APP0 marker (JFIF header)
    write16(jpegFile, 0xFFE0); // Marker
    write16(jpegFile, 16);     // Length
    jpegFile.write("JFIF", 5); // Identifier
    write8(jpegFile, 1);       // Version major
    write8(jpegFile, 1);       // Version minor
    write8(jpegFile, 0);       // Units (0 = no units, aspect ratio only)
    write16(jpegFile, 1);      // X density
    write16(jpegFile, 1);      // Y density
    write8(jpegFile, 0);       // X thumbnail
    write8(jpegFile, 0);       // Y thumbnail
    
    // DQT marker (Define Quantization Table)
    write16(jpegFile, 0xFFDB); // Marker
    write16(jpegFile, 67);     // Length
    write8(jpegFile, 0);       // QT information (table 0, precision 8-bit)
    
    
    jpegFile.write(reinterpret_cast<const char*>(quantTableY), 64);
    
    // SOF0 marker (Start of Frame, Baseline DCT)
    write16(jpegFile, 0xFFC0); // Marker
    write16(jpegFile, 17);     // Length
    write8(jpegFile, 8);       // Precision (8 bits)
    write16(jpegFile, 16);     // Image height
    write16(jpegFile, 16);     // Image width
    write8(jpegFile, 1);       // Number of components (1 for grayscale)
    write8(jpegFile, 1);       // Component ID (1)
    write8(jpegFile, 0x11);    // Sampling factors (HxV, 1x1)
    write8(jpegFile, 0);       // Quantization table number
    
    // DHT marker (Define Huffman Table)
    write16(jpegFile, 0xFFC4); // Marker
    write16(jpegFile, 34+2+codesAC.size()+codesDC.size()); // Length
    // DC Huffman Table
    write8(jpegFile, 0x00);    // HT information (DC table 0)
    // Number of codes for each length (example values)
    
    uint8_t codesLengthMapDC[16] = {};
    for (int i = 0; i < codesDC.size(); i++) {
        codesLengthMapDC[codesDC[i].branchDepth-1]++;
    }
    
    
    jpegFile.write(reinterpret_cast<const char*>(codesLengthMapDC), 16);
    // Values for each code (example values)
    const int sizeDC = (const int)codesDC.size();
    uint8_t dcValues[sizeDC];
    for (int i = 0; i < sizeDC; i++) {
        dcValues[i] = codesDC[i].val;
    }
    jpegFile.write(reinterpret_cast<const char*>(dcValues), sizeDC);
    
    // AC Huffman Table
    write8(jpegFile, 0x10);    // HT information (AC table 0)
    // Number of codes for each length (example values)
    
    uint8_t codesLengthMapAC[16] = {};
    for (int i = 0; i < codesAC.size(); i++) {
        codesLengthMapAC[codesAC[i].branchDepth-1]++;
    }
    
    
    jpegFile.write(reinterpret_cast<const char*>(codesLengthMapAC), 16);
    // Values for each code example values)
    const int sizeAC = (const int)codesAC.size();
    uint8_t acValues[sizeAC];
    for (int i = 0; i < sizeAC; i++) {
        acValues[i] = (uint8_t)codesAC[i].huffmanSym.to_ulong();
    }
    jpegFile.write(reinterpret_cast<const char*>(acValues), sizeAC);
    
    // SOS marker (Start of Scan)
    write16(jpegFile, 0xFFDA); // Marker
    write16(jpegFile, 12);     // Length
    write8(jpegFile, 1);       // Number of components (1 for grayscale)
    write8(jpegFile, 1);       // Component ID
    write8(jpegFile, 0);       // DC/AC Huffman table number
    write8(jpegFile, 0);       // Start of spectral selection
    write8(jpegFile, 63);      // End of spectral selection
    write8(jpegFile, 0);       // Successive approximation
    
    writeComponents(jpegFile, data);
    
    
    // EOI marker (End of Image)
    write16(jpegFile, 0xFFD9);
    
    jpegFile.close();
    std::cout << "JPEG header file created successfully." << std::endl;
}


void CompresserJPG::writeComponents(ofstream &file, vector<bool> data){
    bitset<8> buffer(0);
    int i;
    for (i = 0; i < data.size(); i++) {
        buffer[7-i%8] = data[i];
        
        if (i != 0 && i % 8 == 0) {
            file.put(static_cast<char>((uint8_t)buffer.to_ulong()));
        }
    }
    
    if (i % 8 != 0) {
        int bitsLeft = 8 - (i%8);
        for (int i = 0; i < bitsLeft; i++) {
            buffer[i] = 0;
        }
        file.put(static_cast<char>((uint8_t)buffer.to_ulong()));
    }
}
