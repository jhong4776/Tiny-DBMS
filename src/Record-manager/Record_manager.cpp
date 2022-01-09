#include "Record_manager.h"

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

    cout << "rid = " << rid << " start = " << start << endl;

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
