#include "System_manager.h"
#include <iostream>
#include <string>
#include <list>

using namespace std;

Property create_pro(string name, int len, int type, bool be_null, bool prikey, bool forkey) {
    Property new_pro;
    for (int i = 0; i < 20; i++)
        new_pro.name[i] = 0;
    name.copy(new_pro.name, 20, 0);
    new_pro.len = len;
    new_pro.type = type;
    new_pro.be_null = be_null;
    new_pro.prikey = prikey; 
    new_pro.forkey = forkey;
    return new_pro;
}

PriKey create_pri(string name, int fortable, string forname) {
    PriKey new_pri;
    for (int i = 0; i < 20; i++){
        new_pri.name[i] = 0;
        new_pri.forname[i] = 0;
    }
    name.copy(new_pri.name, 20, 0);
    new_pri.fortable = fortable;
    forname.copy(new_pri.forname, 20, 0);
    
    return new_pri;
}

ForKey create_for(string name, int pretable, string prename) {
    ForKey new_for;
    for (int i = 0; i < 20; i++){
        new_for.name[i] = 0;
        new_for.prename[i] = 0;
    }
    name.copy(new_for.name, 20, 0);
    new_for.pretable = pretable;
    prename.copy(new_for.prename, 20, 0);
    
    return new_for;
}

int main() {
    System_manager s_m;
    s_m.create_database("akina12");
    s_m.use_database("akina12");

    // s_m.show_table();
    // s_m.table_schema("test2");

    // s_m.add_foreignkey("test2", "name", "test1", "id");

    // s_m.update_Index(s_m.get_table_ID(-1, "test1"));
    cout << s_m.get_Index(s_m.get_table_ID(-1, "test1")) << endl;

    // Table_Header t_h; 
    // t_h.pro_num = 2;
    // t_h.prikey_num = 1;
    // t_h.forkey_num = 0;
    
    // list<Property> pro_list;
    // Property new_pro1 = create_pro("id", 4, 0, false, true, false);
    // Property new_pro2 = create_pro("country", 20, 1, false, false, false);
    // pro_list.push_back(new_pro1);
    // pro_list.push_back(new_pro2);

    // list<PriKey> pri_list;
    // PriKey pri = create_pri("id", -1, "");
    // pri_list.push_back(pri);

    // list<ForKey> for_list;
    // s_m.create_table("test1", t_h, pro_list, pri_list, for_list);

    // Table_Header t_h1; 
    // t_h1.pro_num = 4;
    // t_h1.prikey_num = 0;
    // t_h1.forkey_num = 1;
    
    // list<Property> pro_list1;
    // Property inew_pro1 = create_pro("num", 4, 0, false, false, false);
    // Property inew_pro2 = create_pro("name", 20, 1, false, false, false);
    // Property inew_pro3 = create_pro("c_id", 4, 0, false, false, true);
    // Property inew_pro4 = create_pro("solary", 4, 0, false, false, false);
    // pro_list1.push_back(inew_pro1);
    // pro_list1.push_back(inew_pro2);
    // pro_list1.push_back(inew_pro3);
    // pro_list1.push_back(inew_pro4);

    // list<PriKey> pri_list1;

    // list<ForKey> for_list1;
    // ForKey inew_for1 = create_for("c_id", s_m.get_table_ID(-1, "test1"), "id");
    // for_list1.push_back(inew_for1);
    // s_m.create_table("test2", t_h1, pro_list1, pri_list1, for_list1);
    return 0;
}