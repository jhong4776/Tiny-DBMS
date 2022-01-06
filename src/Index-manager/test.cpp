#include <iostream>
#include "BplusTree.h"
#include "../Header_define.h"
#include <unistd.h>
#include <string.h>

using namespace std;

struct Record{
    int key;
    char name[20];
};


void initial(int objID, BplusTree& tree1) {
    int p_offset = 0;
    tree1.create_new_tree(objID);
}

void insert_record(int key, int rid, BplusTree& tree1) {
    cout << "insert : key = " << key << std::endl;
    tree1.insert_record(key, rid);
    cout << "after insert record" << endl;
}

void get_record(int k, BplusTree& tree1) {
    std::cout << "get record k = " << k << std::endl;
    int get_record = tree1.get_record(k);
    if (get_record == -1) {
        cout << "doesn't have the record!" << endl;
    } else {
        cout << "key = " << k << " rid = " << get_record << endl;
    }  
}

void initial_root(int objID, BplusTree& tree1) {
    int p_offset = 0;
    tree1.initial_tree_root(objID);
    cout << endl;  
}

int main() {
    BplusTree tree1;

    // initial(2, tree1);    
    // for (int i = 1; i <= 1000000; i++) {
    //     int now = i*9 + 100;
    //     insert_record(i, now, tree1);
    // }
    // tree1.write_back(); 
    // std::cout << "end" << std::endl;

    // tree1.root_debug();
    
    initial_root(2, tree1); 
    get_record(656825, tree1);
}