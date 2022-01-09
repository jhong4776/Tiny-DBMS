#include <iostream>
#include <string>
#include "Record_manager.h"

using namespace std;

struct Record
{
    int id;
    char test[25];
    float f;
};


int test() {
    Record_manager r_m;

    System_manager s_m;
    // s_m.create_database("fate");
    s_m.use_database("fate");

    // Table_Header t_h;
    // t_h.forkey_num = 0; t_h.index_num = 0; t_h.prikey_num = 0; t_h.pro_num = 3; t_h.max_page = 0;

    // list<Property> pro_list;
    // list<PriKey> pri_list;
    // list<ForKey> for_list;
    // Property pro;
    // for (int i = 0; i < 20; i++) {
    //     pro.name[i] = 0;
    // }
    // string proname = "id";
    // proname.copy(pro.name, proname.size(), 0);
    // pro.len = sizeof(int); pro.forkey = false; pro.prikey = false; pro.type = 0;
    
    // Property pro1;
    // for (int i = 0; i < 20; i++) {
    //     pro1.name[i] = 0;
    // }
    // string proname1 = "test";
    // proname1.copy(pro1.name, proname1.size(), 0);
    // pro1.len = 25; pro1.forkey = false; pro1.prikey = false; pro1.type = 1;

    // Property pro2;
    // for (int i = 0; i < 20; i++) {
    //     pro2.name[i] = 0;
    // }
    // string proname2 = "f";
    // proname2.copy(pro2.name, proname2.size(), 0);
    // pro2.len = sizeof(float); pro2.forkey = false; pro2.prikey = false; pro2.type = 2;

    // pro_list.push_back(pro); pro_list.push_back(pro1); pro_list.push_back(pro2);

    // s_m.create_table("test1", t_h, pro_list, pri_list, for_list);

    // r_m.createFile("fate", 1);
    r_m.openFile("fate", 1);

    // char input[1024];
    // for (int i = 0; i < 1024; i++) input[i] = 0;

    // int id1 = 6; string test1 = "saber"; float f1 = 0.75;
    // memcpy(input, &id1, sizeof(int));
    // memcpy(&input[sizeof(int)], test1.data(), 25);
    // memcpy(&input[sizeof(int) + 25], &f1, sizeof(float));

    // int rid = r_m.insertRecord(input) + sizeof(Record_Header);

    int rid = 37;
    rid = rid + sizeof(Record_Header);
    // r_m.deleteRecord(rid);
    // r_m.updateRecord(rid, input);

    char output[1024];
    for (int i = 0; i < 1024; i++) output[i] = 0;
    int status = r_m.getRecord(rid, output);
    if (status == LIVE) {
        int id2 = 0; float f2 = 0; char s2[25];
        for (int i = 0; i < 25; i++) s2[i] = 0;
        memcpy(&id2, output, sizeof(int));
        memcpy(&s2, &output[sizeof(int)], 25);
        memcpy(&f2, &output[sizeof(int) + 25], sizeof(float));  
        string name = s2;  

        cout << "id = " << id2 << " name = " << name << " f = " << f2 << endl;
    } else {
        cout << "record doesn't exit!" << endl;
    }

    cout << "find 4 : " << r_m.find_record(0, sizeof(int), "saber", 25) << endl;
    // r_m.closeFile();

    return 0;
}