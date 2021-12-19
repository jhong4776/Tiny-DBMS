#include "Record.hpp"
#include <cstring>
#include <string>
#include <sstream>

#include "../filesystem/fileio/FileManager.h"
#include "../filesystem/bufmanager/BufPageManager.h"

RecordHandler::RecordHandler(){

}                      
int RecordHandler::CreateFile(const char *fileName){
    assert(!ready);
    this->fileName = std::string(fileName);
    BufPageManager::getFileManager().createFile(fileName);
    fileID = BufPageManager::getFileManager().openFile(fileName);
    permID = BufPageManager::getFileManager().getFilePermID(fileID);
    BufPageManager::getInstance().allocPage(fileID, 0);
    RegisterManager::getInstance().checkIn(permID, this);
    ready = true;
    head.pageTot = 1;
    head.recordByte = 4; // reserve first 4 bytes for notnull info
    //head.rowTot = 0;
    head.columnTot = 0;
    head.dataArrUsed = 0;
    head.nextAvail = (unsigned int) -1;
    head.notNull = 0;
    head.checkTot = 0;
    head.foreignKeyTot = 0;
    head.primaryCount = 0;
    addColumn("RID", CT_INT, 10, true, false, nullptr);
    setPrimary(0);
    buf = nullptr;
    for (auto &col: colIndex) {
        col.clear();
    }
}
int RecordHandler::DestroyFile(const char *fileName){
    assert(ready == 1);
    dropIndex();
    RegisterManager::getInstance().checkOut(permID);
    BufPageManager::getInstance().closeFile(fileID, false);
    BufPageManager::getFileManager().closeFile(fileID);
    ready = false;
}
int RecordHandler::OpenFile(const char *fileName){
    assert(ready == 0);
    this->fileName = std::string(fileName);
    fileID = BufPageManager::getFileManager().openFile(fileName);
    permID = BufPageManager::getFileManager().getFilePermID(fileID);
    RegisterManager::getInstance().checkIn(permID, this);
    int index = BufPageManager::getInstance().getPage(fileID, 0);
    memcpy(&head, BufPageManager::getInstance().access(index), sizeof(TableHead));
    ready = true;
    buf = nullptr;
    for (auto &col: colIndex) {
        col.clear();
    }
    loadIndex();
}
int RecordHandler::CloseFile(){
    assert(ready);
    storeIndex();
    int index = BufPageManager::getInstance().getPage(fileID, 0);
    memcpy(BufPageManager::getInstance().access(index), &head, sizeof(TableHead));
    BufPageManager::getInstance().markDirty(index);
    RegisterManager::getInstance().checkOut(permID);
    BufPageManager::getInstance().closeFile(fileID);
    BufPageManager::getFileManager().closeFile(fileID);
    ready = false;
    if (buf) {
        delete[] buf;
        buf = 0;
    }
}
int RecordHandler::GetRecord(const int &rid, char *&pData){
    int pageID = rid / PAGE_SIZE;
    int offset = rid % PAGE_SIZE;
    assert(1 <= pageID && pageID < head.pageTot);
    auto index = BufPageManager::getInstance().getPage(fileID, pageID);
    auto page = BufPageManager::getInstance().access(index);
    assert(getFooter(page, offset / head.recordByte));
    return page + offset;
}
int RecordHandler::DeleteRecord(const int &rid){
    int pageID = rid / PAGE_SIZE;
    int offset = rid % PAGE_SIZE;
    for (int i = 0; i < head.columnTot; i++) {
        if (head.hasIndex & (1 << i)) eraseColIndex(rid, i);
    }
    int index = BufPageManager::getInstance().getPage(fileID, pageID);
    char *page = BufPageManager::getInstance().access(index);
    char *record = page + offset;
    unsigned int &next = *(unsigned int *) record;
    next = head.nextAvail;
    head.nextAvail = rid;
    inverseFooter(page, offset / head.recordByte);
    BufPageManager::getInstance().markDirty(index);
}

void RecordHandler::allocPage() {
    auto index = BufPageManager::getInstance().allocPage(fileID, head.pageTot);
    auto buf = BufPageManager::getInstance().access(index);
    auto n = (PAGE_SIZE - PAGE_FOOTER_SIZE) / head.recordByte;
    n = (n < MAX_REC_PER_PAGE) ? n : MAX_REC_PER_PAGE;
    for (int i = 0, p = 0; i < n; i++, p += head.recordByte) {
        unsigned int &ptr = *(unsigned int *) (buf + p);
        ptr = head.nextAvail;
        head.nextAvail = (unsigned int) head.pageTot * PAGE_SIZE + p;
    }
    memset(buf + PAGE_SIZE - PAGE_FOOTER_SIZE, 0, PAGE_FOOTER_SIZE);
    BufPageManager::getInstance().markDirty(index);
    head.pageTot++;
}

int RecordHandler::InsertRecord(const int &rid, const char *pData){
    assert(buf != nullptr);
    if (head.nextAvail == (RID_t) -1) {
        allocPage();
    }
    int rid = head.nextAvail;
    setTempRecord(0, (char *) &head.nextAvail);
    auto error = checkRecord();
    if (!error.empty()) {
        printf("Error occurred when inserting record, aborting...\n");
        return error;
    }
    int pageID = head.nextAvail / PAGE_SIZE;
    int offset = head.nextAvail % PAGE_SIZE;
    int index = BufPageManager::getInstance().getPage(fileID, pageID);
    char *page = BufPageManager::getInstance().access(index);
    head.nextAvail = *(unsigned int *) (page + offset);
    memcpy(page + offset, buf, head.recordByte);
    BufPageManager::getInstance().markDirty(index);
    inverseFooter(page, offset / head.recordByte);
    for (int i = 0; i < head.columnTot; i++) insertColIndex(rid, i);
    return "";
}
int RecordHandler::UpdateRecord(const int &rid, const char *pData){
    if (data == nullptr) {
        return modifyRecordNull(rid, col);
    }
    int pageID = rid / PAGE_SIZE;
    int offset = rid % PAGE_SIZE;
    int index = BufPageManager::getInstance().getPage(fileID, pageID);
    char *page = BufPageManager::getInstance().access(index);
    char *record = page + offset;
    std::string err = loadRecordToTemp(rid, page, offset);
    if (!err.empty()) {
        return err;
    }
    assert(col != 0);
    err = setTempRecord(col, data);
    if (!err.empty()) {
        return err;
    }
    err = checkRecord();
    if (!err.empty()) {
        return err;
    }
    eraseColIndex(rid, col);
    memcpy(record, buf, head.recordByte);
    BufPageManager::getInstance().markDirty(index);
    insertColIndex(rid, col);
    return "";
}

void RecordHandler::initTempRecord() {
    unsigned int &notNull = *(unsigned int *) buf;
    notNull = 0;
    for (int i = 0; i < head.columnTot; i++) {
        if (head.defaultOffset[i] != -1) {
            switch (head.columnType[i]) {
                case CT_INT:
                case CT_FLOAT:
                case CT_DATE:
                    memcpy(buf + head.columnOffset[i], head.dataArr + head.defaultOffset[i], 4);
                    break;
                case CT_VARCHAR:
                    strcpy(buf + head.columnOffset[i], head.dataArr + head.defaultOffset[i]);
                    break;
                default:
                    assert(false);
            }
            notNull |= (1u << i);
        }
    }
}

std::string RecordHandler::setTempRecord(int col, const char *data) {
    if (data == nullptr) {
        setTempRecordNull(col);
        return "";
    }
    if (buf == nullptr) {
        buf = new char[head.recordByte];
        initTempRecord();
    }
    unsigned int &notNull = *(unsigned int *) buf;
    switch (head.columnType[col]) {
        case CT_INT:
        case CT_DATE:
        case CT_FLOAT:
            memcpy(buf + head.columnOffset[col], data, 4);
            break;
        case CT_VARCHAR:
            if ((unsigned int) head.columnLen[col] < strlen(data)) {
                printf("%d %s\n", head.columnLen[col], data);
            }
            if (strlen(data) > (unsigned int) head.columnLen[col]) {
                return "ERROR: varchar too long";
            }
            strcpy(buf + head.columnOffset[col], data);
            break;
        default:
            assert(0);
    }
    notNull |= (1u << col);
    return "";
}