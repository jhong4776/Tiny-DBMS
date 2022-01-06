#include "Index_manager.h"
#include "../Page-manager/Header_manager.h"
#include "../FS-manager/fileio/FileManager.h"
#include "../FS-manager/bufmanager/BufPageManager.h"
#include <string.h>

IndexHandler::IndexHandler() {}

IndexHandler::~IndexHandler() {}

/**
 * 创建索引页，从根节点开始创建
 * root
 * @param filename 新建根索引页的文件名
*/
RC IndexHandler::CreateIndex(const char *filename) {
    btree.create_new_tree(2);
    return 0;
}

RC IndexHandler::DestroyIndex(const char *filename) {
    return 0;
}

RC IndexHandler::OpenIndex(const char *filename) {
    btree.initial_tree_root(2);
    return 0;
}

RC IndexHandler::CloseIndex() {
    return 0;
}

RC IndexHandler::Seainth(int lowerBound, int upperBound) {
    for (int i = lowerBound; i < upperBound; i++) {
       int rid = btree.get_record(i);
       if (rid != -1)
           return rid;
    }
    return -1;
}

RC IndexHandler::DeleteRecord(int lowerBound, int upperBound) {
    for (int i = lowerBound; i < upperBound; i++) {
        btree.delete_record(i);
    }
    return 0;
}

RC IndexHandler::InsertRecoder(int key, const int &rid) {
    btree.insert_record(key, rid);
    return 0;
}

RC IndexHandler::UpdateRecoder(int oldKey, const int &oldint, int newKey, const int & newint) {
    btree.delete_record(oldKey);
    btree.insert_record(newKey, newint);
    return 0;
}

void IndexHandler::WriteBack() {
    btree.write_back();
}