#include "Record_manager.h"
#include "../Index-manager/Index_manager.h"
#include <set>

using namespace std;

Record_manager::Record_manager() { 
    f_manager = new FileManager();
    b_manager = new BufPageManager(f_manager);
}

Record_manager::~Record_manager() {
    b_manager->~BufPageManager();
    f_manager->~FileManager();
}

int Record_manager::createFile(std::string database_name, int tableID) {
    int buf_ID = 0;
    int pageID = 0;
    f_manager->createFile((database_name + "_" + to_string(tableID)).data());
    f_manager->openFile((database_name + "_" + to_string(tableID)).data(), fileID);
    BufType buf = b_manager->getPage(fileID, pageID, buf_ID);

    System_manager s_m;
    s_m.use_database(database_name);
    int len = s_m.get_recordLen(tableID);
    Record_Header r_h;
    r_h.pageID = 0; r_h.slotCnt = 0; r_h.len = len;
    memcpy(buf, &r_h, sizeof(Record_Header));

    b_manager->markDirty(buf_ID);
    b_manager->writeBack(buf_ID);
    f_manager->closeFile(fileID);
    return 0;
}

int Record_manager::openFile(std::string database_name, int tableID) {
    f_manager->openFile((database_name + "_" + to_string(tableID)).data(), fileID);
    db_now = database_name;
    table_ID = tableID;
    return 0;
}

int Record_manager::closeFile() {
    f_manager->closeFile(fileID);
    return 0;
}

int Record_manager::insertRecord(const char* input) {
    int bufID = 0;
    BufType buf = b_manager->getPage(fileID, 0, bufID);
    Record_Header* r_h = (Record_Header *)buf;
    int len = r_h->len; int maxPageID = r_h->pageID;
    
    int ibufID = 0;
    BufType ibuf = b_manager->getPage(fileID, maxPageID, ibufID);
    Record_Header* ir_h = (Record_Header *)ibuf;
    if ((ir_h->len + sizeof(int)) * (ir_h->slotCnt + 1) > PAGE_SIZE) {
        // 记录超过页面容量，写入新的页面
        b_manager->release(ibufID);
        r_h->pageID += 1;
        b_manager->markDirty(bufID);
        b_manager->writeBack(bufID);
        // 写入页头，第一个数据的标记及数据
        int newbufID = 0;
        BufType newbuf = b_manager->getPage(fileID, maxPageID + 1, newbufID);
        Record_Header newheader;
        newheader.len = len; newheader.pageID = maxPageID + 1; newheader.slotCnt = 1;
        memcpy(newbuf, &newheader, sizeof(Record_Header));
        int tag = LIVE;
        memcpy(&newbuf[sizeof(Record_Header)], &tag, sizeof(int));
        memcpy(&newbuf[sizeof(Record_Header) + sizeof(int)], input, len);

        b_manager->markDirty(newbufID);
        b_manager->writeBack(newbufID);

        System_manager s_m;
        s_m.use_database(db_now);
        s_m.update_pagenum(table_ID);

        return (maxPageID + 1) * PAGE_SIZE;
    } else {
        // 写入数据
        int newstart = (len + sizeof(int)) * ir_h->slotCnt;
        int tag = LIVE;
        memcpy(&ibuf[sizeof(Record_Header) + newstart], &tag, sizeof(int));
        memcpy(&ibuf[sizeof(Record_Header) + newstart + sizeof(int)], input, len);
        int oldslot = ir_h->slotCnt;
        ir_h->slotCnt += 1;
        b_manager->markDirty(ibufID);
        b_manager->writeBack(ibufID);
        b_manager->release(bufID);

        return maxPageID * PAGE_SIZE + (len + sizeof(int)) * oldslot;
    }
}

int Record_manager::deleteRecord(int rid) {
    cout << "delete : " << rid << endl;
    int pageID = rid / PAGE_SIZE;
    int start = rid % PAGE_SIZE;

    int bufID = 0;
    BufType buf = b_manager->getPage(fileID, pageID, bufID);
    int tag = DEAD;
    memcpy(&buf[start], &tag, sizeof(int));

    b_manager->markDirty(bufID);
    b_manager->writeBack(bufID);

    return 0;
}

int Record_manager::get_tag(int rid) {
    int pageID = rid / PAGE_SIZE;
    int start = rid % PAGE_SIZE;

    int bufID = 0;
    BufType buf = b_manager->getPage(fileID, pageID, bufID);  
    int tag = 0;
    memcpy(&tag, &buf[start], sizeof(int));

    return tag; 
}

int Record_manager::updateRecord(int rid, const char* input) {
    int pageID = rid / PAGE_SIZE;
    int start = rid % PAGE_SIZE;

    int bufID = 0;
    BufType buf = b_manager->getPage(fileID, 0, bufID);
    Record_Header* r_h = (Record_Header *)buf;
    int len = r_h->len; int maxPageID = r_h->pageID;

    int ubufID = 0;
    BufType ubuf = b_manager->getPage(fileID, pageID, ubufID);
    memcpy(&ubuf[start + sizeof(int)], input, len);
    b_manager->markDirty(ubufID);
    b_manager->writeBack(ubufID);

    b_manager->release(bufID);
    return 0;
}

int Record_manager::getRecord(int rid, char* output) {
    int pageID = rid / PAGE_SIZE;
    int start = rid % PAGE_SIZE;

    int bufID = 0;
    BufType buf = b_manager->getPage(fileID, 0, bufID);
    Record_Header* r_h = (Record_Header *)buf;
    int len = r_h->len; int maxPageID = r_h->pageID;

    int gbufID = 0;
    BufType gbuf = b_manager->getPage(fileID, pageID, gbufID);
    int* tag = (int *)&gbuf[start];
    if (*tag == DEAD)
        return DEAD;
    
    memcpy(output, &gbuf[start + sizeof(int)], len);
    b_manager->release(gbufID);
    b_manager->release(bufID);
    return LIVE;
}

Record_Header* Record_manager::get_header(std::string database_name, int tableID) {
    int bufID = 0;
    BufType buf = b_manager->getPage(fileID, 0, bufID);
    Record_Header* r_h = (Record_Header *)buf;

    return r_h;
}

bool Record_manager::find_record(int pageID, int offset, int num) {
    int bufID = 0;
    BufType buf = b_manager->getPage(fileID, pageID, bufID);
    Record_Header* r_h = (Record_Header *)buf;
    int len = r_h->len; int slot_n = r_h->slotCnt;
    int* tag;
    int* target;

    int rid = sizeof(Record_Header);
    for(int i = 0; i < slot_n; i++) {
        tag = (int *)&buf[rid];
        if (*tag != LIVE) {
            rid = rid + len + sizeof(int);
            continue;
        }
        target = (int *)&buf[rid + sizeof(int) + offset];
        rid = rid + len + sizeof(int);
        if (*target == num) return true;
    }
    return false;
}

bool Record_manager::find_record(int pageID, int offset, std::string name, int size) {
    int bufID = 0;
    BufType buf = b_manager->getPage(fileID, pageID, bufID);
    Record_Header* r_h = (Record_Header *)buf;
    int len = r_h->len; int slot_n = r_h->slotCnt;

    int rid = sizeof(Record_Header);
    for(int i = 0; i < slot_n; i++) {
        int* tag = (int *)&buf[rid];
        if (*tag != LIVE) {
            rid = rid + len + sizeof(int);
            continue;
        }
        char target[size];
        for (int j = 0; j < size; j++) {
            target[j] = 0;
        }
        memcpy(target, &buf[rid + sizeof(int) + offset], size);
        string s_target = target;
        if (s_target == name) return true;
        rid = rid + len + sizeof(int);
    }
    return false;
}

bool Record_manager::find_record(int pageID, int offset, float num) {
    int bufID = 0;
    BufType buf = b_manager->getPage(fileID, pageID, bufID);
    Record_Header* r_h = (Record_Header *)buf;
    int len = r_h->len; int slot_n = r_h->slotCnt;

    int rid = sizeof(Record_Header);
    for(int i = 0; i < slot_n; i++) {
        int* tag = (int *)&buf[rid];
        if (*tag != LIVE) {
            rid = rid + len + sizeof(int);
            continue;
        }
        float* target = (float *)&buf[rid + sizeof(int) + offset];
        if (*target == num) return true;
        rid = rid + len + sizeof(int);
    }
    return false;
}

void Record_manager::mark_record(int pageID, int offset, int num) {
    int bufID = 0;
    BufType buf = b_manager->getPage(fileID, pageID, bufID);
    Record_Header* r_h = (Record_Header *)buf;
    int len = r_h->len; int slot_n = r_h->slotCnt;

    int rid = sizeof(Record_Header);
    for(int i = 0; i < slot_n; i++) {
        int* tag = (int *)&buf[rid];
        int* target = (int *)&buf[rid + sizeof(int) + offset];
        if (*target == num) {
            *tag = DEAD;
        }
        rid = rid + len + sizeof(int);
    } 
    b_manager->markDirty(bufID);
    b_manager->writeBack(bufID);
    return;
}

void Record_manager::mark_record(int pageID, int offset, std::string name, int size) {
    int bufID = 0;
    BufType buf = b_manager->getPage(fileID, pageID, bufID);
    Record_Header* r_h = (Record_Header *)buf;
    int len = r_h->len; int slot_n = r_h->slotCnt;

    int rid = sizeof(Record_Header);
    for(int i = 0; i < slot_n; i++) {
        int* tag = (int *)&buf[rid];
        char target[size];
        for (int j = 0; j < size; j++) {
            target[j] = 0;
        }
        memcpy(target, &buf[rid + sizeof(int) + offset], size);
        string s_target = target;
        if (s_target == name) {
             *tag = DEAD;
        }
        rid = rid + len + sizeof(int);
    }
    b_manager->markDirty(bufID);
    b_manager->writeBack(bufID);
    return;
}

void Record_manager::mark_record(int pageID, int offset, float num) {
    int bufID = 0;
    BufType buf = b_manager->getPage(fileID, pageID, bufID);
    Record_Header* r_h = (Record_Header *)buf;
    int len = r_h->len; int slot_n = r_h->slotCnt;

    int rid = sizeof(Record_Header);
    for(int i = 0; i < slot_n; i++) {
        int* tag = (int *)&buf[rid];
        float* target = (float *)&buf[rid + sizeof(int) + offset];
        if (*target == num) {
            *tag = DEAD;
        }
        rid = rid + len + sizeof(int);
    } 
    b_manager->markDirty(bufID);
    b_manager->writeBack(bufID);
    return;
}

void Record_manager::select_all(std::string database_name, int tableID) {
    System_manager s_m;
    s_m.use_database(database_name);
    Table_Header* t_h = s_m.get_tableHeader(table_ID);

    int maxPage = t_h->max_page;
    vector<Property> pro_v = s_m.get_property_vector(table_ID);

    char output[1024];
    for (int i = 0; i <= maxPage; i++) {
        int bufID = 0;
        BufType buf = b_manager->getPage(fileID, i, bufID);
        int rid = sizeof(Record_Header);
        Record_Header* r_h = (Record_Header *)buf;
        int slotCnt = r_h->slotCnt;
        int len = r_h->len;

        for (int j = 0; j < slotCnt; j++) {
            int* tag = (int *)&buf[rid];
            if (*tag == LIVE) {
                memcpy(output, &buf[rid + sizeof(int)], len);
                int offset = 0;
                for (int k = 0; k < pro_v.size(); k++) {
                    string pro_name = pro_v[k].name;
                    if (pro_v[k].type == 0) {
                        int* target = (int *)&output[offset];
                        cout << pro_name << " = " << *target << " ";
                        offset += sizeof(int);
                    }
                    else if (pro_v[k].type == 1) {
                        int size = pro_v[k].len;
                        char target[size];
                        memcpy(target, &output[offset], size);
                        string s_target = target;
                        cout << pro_name << " = " << s_target << " ";
                        offset += size;
                    }
                    else if (pro_v[k].type == 2) {
                        float* target = (float *)&output[offset];
                        cout << pro_name << " = " << *target << " ";
                        offset += sizeof(float);
                    }
                }
                cout << endl;
            }
            rid = rid + len + sizeof(int);
        }
        b_manager->release(bufID);
    }
}

bool Record_manager::check_dup(int pageID, int offset, int type, int slen, std::string colunm) {
    int bufID = 0;
    BufType buf = b_manager->getPage(fileID, pageID, bufID);
    int rid = sizeof(Record_Header);
    Record_Header* r_h = (Record_Header *)buf;
    int slotCnt = r_h->slotCnt; int len = r_h->len;
    
    if (type == 0) {
        set<int> s1;
        vector<PKUnit> pk_v;
        for (int i = 0; i < slotCnt; i++) {
            int* tag = (int *)&buf[rid];
            if (*tag != LIVE) {
                rid = rid + len + sizeof(int);
                continue;
            }
            int* target = (int *)&buf[rid + sizeof(int) + offset];
            if (s1.find(*target) != s1.end()) return false;
            s1.insert(*target);

            PKUnit pk;
            pk.record = rid; pk.k = *target;
            pk_v.push_back(pk);
            rid = rid + len + sizeof(int);
        }
        IndexHandler i_m;
        i_m.CreateIndex(db_now, table_ID, colunm);
        for (int i = 0; i < pk_v.size(); i++) {
            i_m.InsertRecoder(pk_v[i].k, pk_v[i].record);
        }
        i_m.WriteBack();
    }
    else if(type == 1) {
        set<string> s2;
        for (int i = 0; i < slotCnt; i++) {
            int* tag = (int *)&buf[rid];
            if (*tag != LIVE) { 
                rid = rid + len + sizeof(int);
                continue;
            }
            char target[slen];
            memcpy(target, &buf[rid + sizeof(int) + offset], slen);
            string s_target = target;
            if (s2.find(s_target) != s2.end()) return false;
            s2.insert(s_target);

            rid = rid + len + sizeof(int);
        }
    }
    else if(type == 2) {
        set<float> s3;
        for (int i = 0; i < slotCnt; i++) {
            int* tag = (int *)&buf[rid];
            if (*tag != LIVE) {
                rid = rid + len + sizeof(int);
                continue;
            }
            float* target = (float *)&buf[rid + sizeof(int) + offset];
            if (s3.find(*target) != s3.end()) return false;
            s3.insert(*target);

            rid = rid + len + sizeof(int);
        }
    }

    return true;
}

void Record_manager::select_equa(std::string database_name, int tableID, int num, std::string colunm) {
    System_manager s_m;
    s_m.use_database(database_name);
    Table_Header* t_h = s_m.get_tableHeader(table_ID);

    int maxPage = t_h->max_page;
    vector<Property> pro_v = s_m.get_property_vector(table_ID);

    // 求offset
    int equa_off = 0;
    for (int i = 0; i < pro_v.size(); i++) {
        string c_name = pro_v[i].name;
        if (c_name == colunm)
            break;
        equa_off += pro_v[i].len;
    }

    char output[1024];
    for (int i = 0; i <= maxPage; i++) {
        int bufID = 0;
        BufType buf = b_manager->getPage(fileID, i, bufID);
        int rid = sizeof(Record_Header);
        Record_Header* r_h = (Record_Header *)buf;
        int slotCnt = r_h->slotCnt;
        int len = r_h->len;

        for (int j = 0; j < slotCnt; j++) {
            int* tag = (int *)&buf[rid];
            if (*tag == LIVE) {
                memcpy(output, &buf[rid + sizeof(int)], len);
                // 判定
                int* e_num = (int *)&output[equa_off];
                if (*e_num != num) {
                    rid = rid + len + sizeof(int);
                    continue;
                }
                int offset = 0;
                for (int k = 0; k < pro_v.size(); k++) {
                    string pro_name = pro_v[k].name;
                    if (pro_v[k].type == 0) {
                        int* target = (int *)&output[offset];
                        cout << pro_name << " = " << *target << " ";
                        offset += sizeof(int);
                    }
                    else if (pro_v[k].type == 1) {
                        int size = pro_v[k].len;
                        char target[size];
                        memcpy(target, &output[offset], size);
                        string s_target = target;
                        cout << pro_name << " = " << s_target << " ";
                        offset += size;
                    }
                    else if (pro_v[k].type == 2) {
                        float* target = (float *)&output[offset];
                        cout << pro_name << " = " << *target << " ";
                        offset += sizeof(float);
                    }
                }
                cout << endl;
            }
            rid = rid + len + sizeof(int);
        }
        b_manager->release(bufID);
    }    
}

void Record_manager::select_equa(std::string database_name, int tableID, std::string num, std::string colunm) {
    System_manager s_m;
    s_m.use_database(database_name);
    Table_Header* t_h = s_m.get_tableHeader(table_ID);

    int maxPage = t_h->max_page;
    vector<Property> pro_v = s_m.get_property_vector(table_ID);

    // 求offset
    int equa_off = 0;
    int slen = 0;
    for (int i = 0; i < pro_v.size(); i++) {
        string c_name = pro_v[i].name;
        if (c_name == colunm) {
            slen = pro_v[i].len;
            break;
        }
        equa_off += pro_v[i].len;
    }

    char output[1024];
    for (int i = 0; i <= maxPage; i++) {
        int bufID = 0;
        BufType buf = b_manager->getPage(fileID, i, bufID);
        int rid = sizeof(Record_Header);
        Record_Header* r_h = (Record_Header *)buf;
        int slotCnt = r_h->slotCnt;
        int len = r_h->len;
        for (int j = 0; j < slotCnt; j++) {
            int* tag = (int *)&buf[rid];
            if (*tag == LIVE) {
                memcpy(output, &buf[rid + sizeof(int)], len);
                // 判定
                char e_num[slen];
                memcpy(e_num, &output[equa_off], slen);
                string se_num = e_num;
                if (se_num != num) {
                    rid = rid + len + sizeof(int);
                    continue;
                }

                int offset = 0;
                for (int k = 0; k < pro_v.size(); k++) {
                    string pro_name = pro_v[k].name;
                    if (pro_v[k].type == 0) {
                        int* target = (int *)&output[offset];
                        cout << pro_name << " = " << *target << " ";
                        offset += sizeof(int);
                    }
                    else if (pro_v[k].type == 1) {
                        int size = pro_v[k].len;
                        char target[size];
                        memcpy(target, &output[offset], size);
                        string s_target = target;
                        cout << pro_name << " = " << s_target << " ";
                        offset += size;
                    }
                    else if (pro_v[k].type == 2) {
                        float* target = (float *)&output[offset];
                        cout << pro_name << " = " << *target << " ";
                        offset += sizeof(float);
                    }
                }
                cout << endl;
            }
            rid = rid + len + sizeof(int);
        }
        b_manager->release(bufID);
    }    
}

void Record_manager::select_equa(std::string database_name, int tableID, float num, std::string colunm) {
    System_manager s_m;
    s_m.use_database(database_name);
    Table_Header* t_h = s_m.get_tableHeader(table_ID);

    int maxPage = t_h->max_page;
    vector<Property> pro_v = s_m.get_property_vector(table_ID);

    // 求offset
    int equa_off = 0;
    for (int i = 0; i < pro_v.size(); i++) {
        string c_name = pro_v[i].name;
        if (c_name == colunm)
            break;
        equa_off += pro_v[i].len;
    }

    char output[1024];
    for (int i = 0; i <= maxPage; i++) {
        int bufID = 0;
        BufType buf = b_manager->getPage(fileID, i, bufID);
        int rid = sizeof(Record_Header);
        Record_Header* r_h = (Record_Header *)buf;
        int slotCnt = r_h->slotCnt;
        int len = r_h->len;

        for (int j = 0; j < slotCnt; j++) {
            int* tag = (int *)&buf[rid];
            if (*tag == LIVE) {
                memcpy(output, &buf[rid + sizeof(int)], len);
                // 判定
                float* e_num = (float *)&output[equa_off];
                if (*e_num != num) {
                    rid = rid + len + sizeof(int);
                    continue;
                }
                int offset = 0;
                for (int k = 0; k < pro_v.size(); k++) {
                    string pro_name = pro_v[k].name;
                    if (pro_v[k].type == 0) {
                        int* target = (int *)&output[offset];
                        cout << pro_name << " = " << *target << " ";
                        offset += sizeof(int);
                    }
                    else if (pro_v[k].type == 1) {
                        int size = pro_v[k].len;
                        char target[size];
                        memcpy(target, &output[offset], size);
                        string s_target = target;
                        cout << pro_name << " = " << s_target << " ";
                        offset += size;
                    }
                    else if (pro_v[k].type == 2) {
                        float* target = (float *)&output[offset];
                        cout << pro_name << " = " << *target << " ";
                        offset += sizeof(float);
                    }
                }
                cout << endl;
            }
            rid = rid + len + sizeof(int);
        }
        b_manager->release(bufID);
    }    
}