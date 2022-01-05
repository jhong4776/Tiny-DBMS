#include "System_manager.h"
#include <iostream>
#include <string>
#include <list>

using namespace std;

int main() {
    System_manager s_m;
    // s_m.create_database("akina");
    s_m.use_database("akina");
    s_m.create_table("test1");
    s_m.show_table(); 
    // s_m.rename_table("test5", "test51");
    // s_m.create_table("test6");

    // list<string>* names;
    // names = s_m.show_database();
    // for (auto it = names->begin(); it != names->end(); it++) {
    //     cout << (*it) << " ";
    // }
    // cout << endl;
    return 0;
}