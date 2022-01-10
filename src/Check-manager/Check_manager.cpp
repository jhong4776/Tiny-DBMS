#include "Check_manager.h"
#include <iostream>
#include <string>
#include <vector>
#include "../Index-manager/Index_manager.h"

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

void CheckTool::mark_equa(int tableID, std::string colunm, int num) {
    cout << "mark_equa : tableID = " << tableID << " colunm = " << colunm << " num = " << num << endl;
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
        r_m.mark_record(i, offset, num);
    }
    r_m.closeFile();
    return;            
}

void CheckTool::mark_equa(int tableID, std::string colunm, std::string num) {
    cout << "mark_equa : tableID = " << tableID << " colunm = " << colunm << " num = " << num << endl;
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
        r_m.mark_record(i, offset, num, len);
    }
    r_m.closeFile();
    return;            
}

void CheckTool::mark_equa(int tableID, std::string colunm, float num) {
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
        r_m.mark_record(i, offset, num);
    }
    r_m.closeFile();
    return;            
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
    IndexHandler i_m;
    i_m.OpenIndex(db_now, pritable, priname);
    int rid = i_m.Seainth(num, num+1);

    if (rid == -1)
        return false;

    rid += sizeof(Record_Header);
    Record_manager r_m;
    r_m.openFile(db_now, pritable);
    int tag = r_m.get_tag(rid);
    r_m.closeFile();
    if (tag != LIVE) return false;
    return true;
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

void CheckTool::checkForkey(std::string table_name, std::string prikey, int num) {
    System_manager s_m;
    s_m.use_database(db_now);
    int table_ID = s_m.get_table_ID(-1, table_name);
    vector<PriKey> pri_v = s_m.get_pri_vector(table_ID);
    // 先判断是不是主键
    bool is_prikey = false;
    string forname;
    int fortable = 0;
    for (int i = 0; i < pri_v.size(); i++) {
        string priname = pri_v[i].name;
        if (priname == prikey) {
            is_prikey = true;
            forname = pri_v[i].forname;
            fortable = pri_v[i].fortable;
            break;
        }
    }
    if (is_prikey == false) {
        mark_equa(table_ID, prikey, num);
        return;
    }
    
    // 删掉所有索引
    vector<Index> index_list = s_m.get_index_vector(table_ID);
    int rid = -1;
    for (int i = 0; i < index_list.size(); i++) {
        string index_name = index_list[i].name;
        if (index_name == prikey) {
            cout << "index del" << endl;
            IndexHandler i_m;
            i_m.OpenIndex(db_now, table_ID, prikey);
            int rid = i_m.Seainth(num, num+1) + sizeof(Record_Header);
            
            if (rid != -1) {
                Record_manager r_m;
                r_m.openFile(db_now, table_ID);
                r_m.deleteRecord(rid);
                r_m.closeFile();

                if (fortable != -1)
                    mark_equa(fortable, forname, num);
            }
            return;
        }
    }
    mark_equa(fortable, forname, num);
}

void CheckTool::checkForkey(std::string table_name, std::string prikey, std::string num) {
    System_manager s_m;
    s_m.use_database(db_now);
    int table_ID = s_m.get_table_ID(-1, table_name);
    vector<PriKey> pri_v = s_m.get_pri_vector(table_ID);
    // 先判断是不是主键
    bool is_prikey = false;
    string forname;
    int fortable = 0;
    for (int i = 0; i < pri_v.size(); i++) {
        string priname = pri_v[i].name;
        if (priname == prikey) {
            is_prikey = true;
            forname = pri_v[i].forname;
            fortable = pri_v[i].fortable;
            break;
        }
    }
    if (is_prikey == false) return;
    mark_equa(fortable, forname, num);
}

void CheckTool::checkForkey(std::string table_name, std::string prikey, float num) {
    System_manager s_m;
    s_m.use_database(db_now);
    int table_ID = s_m.get_table_ID(-1, table_name);
    vector<PriKey> pri_v = s_m.get_pri_vector(table_ID);
    // 先判断是不是主键
    bool is_prikey = false;
    string forname;
    int fortable = 0;
    for (int i = 0; i < pri_v.size(); i++) {
        string priname = pri_v[i].name;
        if (priname == prikey) {
            is_prikey = true;
            forname = pri_v[i].forname;
            fortable = pri_v[i].fortable;
            break;
        }
    }
    if (is_prikey == false) return;
    mark_equa(fortable, forname, num);
}

int CheckTool::getColunmType(std::string table_name, std::string colunm) {
    System_manager s_m;
    s_m.use_database(db_now);
    vector<Property> pro_v = s_m.get_property_vector(s_m.get_table_ID(-1, table_name));

    for (int i = 0; i < pro_v.size(); i++) {
        string c_name = pro_v[i].name;
        cout << "c_name = " << c_name << " colunm = " << colunm << endl;
        if (c_name == colunm) {
            cout << pro_v[i].type << endl;
            return pro_v[i].type;
        }
    }
    return -1;
}

bool CheckTool::checkDup(std::string table_name, std::string colunm) {
    System_manager s_m;
    s_m.use_database(db_now);
    int tableID = s_m.get_table_ID(-1, table_name);
    vector<Property> pro_v = s_m.get_property_vector(tableID);
    Table_Header* r_h = s_m.get_tableHeader(tableID);
    int maxPage = r_h->max_page;

    int offset = 0;
    for (int i = 0; i < pro_v.size(); i++) {
        string proname = pro_v[i].name;
        if(proname == colunm) {
            Record_manager r_m;
            r_m.openFile(db_now, tableID);
            for (int j = 0; j <= maxPage; j++) {
                bool check = r_m.check_dup(j, offset, pro_v[i].type, pro_v[i].len, proname);
                r_m.closeFile();
                if (check == false) return false;
            }
            if (pro_v[i].type == 0) {
                s_m.add_index(table_name, proname);
            }
        }
        offset += pro_v[i].len;
    }
    return true;
}

bool CheckTool::indexCheckDup(int table_ID, std::string colunm, int num) {
    IndexHandler i_m;
    i_m.OpenIndex(db_now, table_ID, colunm);
    int rid = i_m.Seainth(num, num+1);

    if (rid == -1) return true;

    rid += sizeof(Record_Header);
    Record_manager r_m;
    r_m.openFile(db_now, table_ID);
    int tag = r_m.get_tag(rid);

    if (tag != LIVE) return true;

    return false;
}