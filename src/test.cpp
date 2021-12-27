#include <iostream>
#include "Index-manager/BplusTree.h"
#include "Header_define.h"
#include <unistd.h>
#include <string.h>

using namespace std;

struct Record{
    int key;
    char name[20];
};


void initial(int objID, BplusTree& tree1) {
    int p_offset = 0;
    tree1.create_new_tree(objID, sizeof(Record), p_offset);
}

void insert_record(int key, string n, BplusTree& tree1) {
    cout << "insert : key = " << key << std::endl;
    Record record;
    record.key = key;
    for(int i = 0; i < 20; i++)
        record.name[i] = 0;

    n.copy(record.name, 7, 0);

    uint8_t buf[1024];
    memcpy(buf, (uint8_t *)&record, sizeof(Record));
    tree1.insert_record(buf, sizeof(Record));
    cout << "after insert record" << endl;
}

void get_record(int k, BplusTree& tree1) {
    uint8_t buf2[1024];
    std::cout << "get record k = " << k << std::endl;
    bool get_record = tree1.get_record(k, buf2);
    if (get_record == false) {
        cout << "doesn't have the record!" << endl;
    } else {
        Record* record2 = (Record *)buf2;
        cout << "key = " << record2->key << " name = " << record2->name << endl;
    }  
}

void initial_root(int objID, BplusTree& tree1) {
    int p_offset = 0;
    tree1.initial_tree_root(objID, p_offset);
    cout << endl;  
}

int main() {
    BplusTree tree1;
    initial(2, tree1);
    
    for (int i = 1; i <= 124; i++) {
        string now = "name" + to_string(i);
        insert_record(i, now, tree1);
    }
    tree1.write_back(); 
    std::cout << "end" << std::endl;

    tree1.root_debug();
    
    // initial_root(2, tree1); 
    // get_record(101, tree1);
}