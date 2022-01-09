#include "Check_manager.h"
#include <iostream>
#include <string>
#include <vector>

CheckTool::CheckTool(std::string database_now) {
    db_now = database_now;
}
CheckTool::~CheckTool() {}

bool CheckTool::record_equa(int tableID, std::string colunm, int num) { 
    int rid = sizeof(Record_Header);
    System_manager s_m;
    s_m.use_database(db_now);
    Table_Header* t_h = s_m.get_tableHeader(tableID);
    int max_page = t_h->max_page;

    int offset = 0;
    vector<Property> pro_v = s_m.get_property_vector(tableID);
    for (int i = 0; i < pro_v.size(); i++) {
        string proname = pro_v[i].name;
        if (proname == colunm) {
            break;
        }
        offset += pro_v[i].len;
    }

    Record_manager r_m;
    r_m.openFile(db_now, tableID);
    for (int i = 0; i <= max_page; i++) {
        if (r_m.find_record(i, offset, num)) {
            r_m.closeFile();
            return true;
        }
    }
    r_m.closeFile();
    return false;
}

bool CheckTool::record_equa(int tableID, std::string colunm, string num) {
    int rid = sizeof(Record_Header);
    System_manager s_m;
    s_m.use_database(db_now);
    Table_Header* t_h = s_m.get_tableHeader(tableID);
    int max_page = t_h->max_page;

    int offset = 0;
    int len = 0;
    vector<Property> pro_v = s_m.get_property_vector(tableID);
    for (int i = 0; i < pro_v.size(); i++) {
        string proname = pro_v[i].name;
        if (proname == colunm) {
            len = pro_v[i].len;
            break;
        }
        offset += pro_v[i].len;
    }

    Record_manager r_m;
    r_m.openFile(db_now, tableID);
    for (int i = 0; i <= max_page; i++) {
        if (r_m.find_record(i, offset, num, len)) {
            r_m.closeFile();
            return true;
        }
    }
    r_m.closeFile();
    return false;
}

bool CheckTool::record_equa(int tableID, std::string colunm, float num) {
    int rid = sizeof(Record_Header);
    System_manager s_m;
    s_m.use_database(db_now);
    Table_Header* t_h = s_m.get_tableHeader(tableID);
    int max_page = t_h->max_page;

    int offset = 0;
    vector<Property> pro_v = s_m.get_property_vector(tableID);
    for (int i = 0; i < pro_v.size(); i++) {
        string proname = pro_v[i].name;
        if (proname == colunm) {
            break;
        }
        offset += pro_v[i].len;
    }

    Record_manager r_m;
    r_m.openFile(db_now, tableID);
    for (int i = 0; i <= max_page; i++) {
        if (r_m.find_record(i, offset, num)){
            r_m.closeFile();
            return true;
        }
    }
    r_m.closeFile();
    return false;
}

bool CheckTool::checkPrikey(std::string table_name, std::string forkey, int num) {
    // 找到相应的主键和主键所在的数据表
    System_manager s_m;
    s_m.use_database(db_now);
    int table_ID = s_m.get_table_ID(-1, table_name);
    vector<ForKey> for_v = s_m.get_for_vector(table_ID);
    //先判断是不是外键
    bool is_forkey = false;
    string priname;
    int pritable = 0;
    for (int i = 0; i < for_v.size(); i++) {
        string forname = for_v[i].name; 
        if (forname == forkey) {
            is_forkey = true;
            priname = for_v[i].prename;
            pritable = for_v[i].pretable;
            break;
        }
    }
    if (is_forkey == false) return true; // 不是外键直接通过检查
    // 检查主键的值是否在数据表中
    return record_equa(pritable, priname, num);
}

bool CheckTool::checkPrikey(std::string table_name, std::string forkey, float num) {
    // 找到相应的主键和主键所在的数据表
    System_manager s_m;
    s_m.use_database(db_now);
    int table_ID = s_m.get_table_ID(-1, table_name);
    vector<ForKey> for_v = s_m.get_for_vector(table_ID);
    //先判断是不是外键
    bool is_forkey = false;
    string priname;
    int pritable = 0;
    for (int i = 0; i < for_v.size(); i++) {
        string forname = for_v[i].name; 
        if (forname == forkey) {
            is_forkey = true;
            priname = for_v[i].prename;
            pritable = for_v[i].pretable;
            break;
        }
    }
    if (is_forkey == false) return true; // 不是外键直接通过检查
    // 检查主键的值是否在数据表中
    return record_equa(pritable, priname, num);
}

bool CheckTool::checkPrikey(std::string table_name, std::string forkey, std::string num) {
    // 找到相应的主键和主键所在的数据表
    System_manager s_m;
    s_m.use_database(db_now);
    int table_ID = s_m.get_table_ID(-1, table_name);
    vector<ForKey> for_v = s_m.get_for_vector(table_ID);
    //先判断是不是外键
    bool is_forkey = false;
    string priname;
    int pritable = 0;
    for (int i = 0; i < for_v.size(); i++) {
        string forname = for_v[i].name; 
        if (forname == forkey) {
            is_forkey = true;
            priname = for_v[i].prename;
            pritable = for_v[i].pretable;
            break;
        }
    }
    if (is_forkey == false) return true; // 不是外键直接通过检查
    // 检查主键的值是否在数据表中
    return record_equa(pritable, priname, num);
}