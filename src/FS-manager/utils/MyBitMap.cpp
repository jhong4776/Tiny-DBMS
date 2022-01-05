#include "MyBitMap.h"
using namespace std;

MyBitMap::MyBitMap() {
    for (int i = 0; i < 128; i++) {
        data[i] = 0;
    }
}

MyBitMap::~MyBitMap() {

}

void MyBitMap::drawmap() {
    for (int i = 0; i < 128; i++) {
        cout << data << " ";
    }
    cout << endl;
}

void MyBitMap::setBit(int fileID) {
    uint now = 1;
    int base = fileID / 32;
    int offset = fileID % 32;
    for (int i = 0; i < offset; i++) {
        now <<= 1;
    }
    data[base] |= now;
}

void MyBitMap::releaseBit(int fileID) {
    uint now = 1;
    int base = fileID / 32;
    int offset = fileID % 32;
    for (int i = 0; i < offset; i++) {
        now <<= 1;
    }
    now ^= now;
    data[base] &= now;
}

int MyBitMap::getNewID() {
    uint now = 1;
    int newID = -1;
    for (int i = 0; i < 128; i++) {
        for (int j = 0; j < 32; j++) {
            if (data[i*32 + j] & now == 0) {
                newID = i*32 + j;
            }
        }
    }
    return newID;
}

bool MyBitMap::getBit(int fileID) {
    uint now = 1;
    int base = fileID / 32;
    int offset = fileID % 32;
    for (int i = 0; i < offset; i++) {
        now <<= 1;
    }
    if (data[base] &= now == 0)
        return false;
    return true;    
}