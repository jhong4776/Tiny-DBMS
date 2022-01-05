#ifndef MY_BIT_MAP
#define MY_BIT_MAP
typedef unsigned int uint;
/*
#define LEAF_BIT 32
#define MAX_LEVEL 5
#define MAX_INNER_NUM 67
#define MOD 61
#define BIAS 5*/
#include <iostream>
#include <cstring>
using namespace std;

#define LEAF_BIT 32
#define MAX_LEVEL 5
#define MAX_INNER_NUM 67
//#define MOD 61
#define BIAS 5
static unsigned char h[61];


class MyBitMap {
protected:
//	static const int LEAF_BIT = 32;
//	static const int MAX_LEVEL = 5;
//	static const int MAX_INNER_NUM = 10;
//	static const int MOD = 61;
//	static unsigned char h[MOD];
	int data[128];
public:
	MyBitMap() {
		cout << "cr" << endl;
		for(int i = 0; i < 128; i++) {
			data[i] = 0;
		}
	};
	~MyBitMap(){

	};

	void setBit(int fileID){
		uint now = 1;
		int base = fileID / 32;
		int offset = fileID % 32;
		for (int i = 0; i < offset; i++) {
			now <<= 1;
		}
		data[base] |= now;		
	};
	void releaseBit(int fileID){
		uint now = 1;
		int base = fileID / 32;
		int offset = fileID % 32;
		for (int i = 0; i < offset; i++) {
			now <<= 1;
		}
		data[base] ^= now;
	};
	int getNewID(){
		uint now = 1;
		int newID = -1;
		for (int i = 0; i < 128; i++) {
			for (int j = 0; j < 32; j++) {
				if ((data[i*32 + j] & now) == 0) {
					newID = i*32 + j;
					return newID;
				}
				now <<= 1;
			}
		}
		return -1;
	};
	bool getBit(int fileID){
		uint now = 1;
		int base = fileID / 32;
		int offset = fileID % 32;
		for (int i = 0; i < offset; i++) {
			now <<= 1;
		}
		if (data[base] &= now == 0)
			return false;
		return true;    
	};
	void drawmap(){
		for (int i = 0; i < 128; i++) {
			cout << data[i] << " ";
		}
		cout << endl;
	};
};
#endif
