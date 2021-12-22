#include <iostream>
#include "Index-manager/BplusTree.h"
#include <unistd.h>
#include <string.h>

using namespace std;

struct Record{
    int key;
    char name[20];
};

BplusTree tree1;

void initial() {
    tree1.create_new_tree(1, sizeof(Record));
}

void insert_record(int key, string n) {
    Record record;
    record.key = key;
    for(int i = 0; i < 20; i++)
        record.name[i] = 0;

    n.copy(record.name, 6, 0);

    uint8_t buf[1024];
    memcpy(buf, (uint8_t *)&record, sizeof(Record));
    tree1.insert_record(buf, sizeof(Record));
    cout << "after insert record" << endl;
}

void get_record(int k) {

    uint8_t buf2[1024];
    bool get_record = tree1.get_record(k, buf2);
    if (get_record == false) {
        cout << "doesn't have the record!" << endl;
    } else {
        Record* record2 = (Record *)buf2;
        cout << "key = " << record2->key << " name = " << record2->name << endl;
    }  
}

void initial_root() {
    tree1.initial_tree_root(1);
    cout << endl;  
}

int main() {
    // initial();
    initial_root();
    
    // insert_record(2, "caster\n");
    // insert_record(4, "archer\n");
    // insert_record(3, "saber\n");
    // insert_record(6, "berserker\n");
    // insert_record(7, "lancer\n");
    // tree1.write_back(tree1.get_root());
    
    for (int i = 0; i <= 7; i++) {
        get_record(i);
    }
}