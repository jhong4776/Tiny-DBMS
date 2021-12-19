#include "Index_manager.h"
#include "../Page-manager/Header_manager.h"
#include "../FS-manager/fileio/FileManager.h"

IndexHandler::IndexHandler() {}

IndexHandler::~IndexHandler() {}

/**
 * 创建索引页，从根节点开始创建
 * root
 * @param filename 新建根索引页的文件名
*/
RC IndexHandler::CreateIndex(const char *filename) {
    FileManager filemanager;
    filemanager.createFile(filename);

    Header_manager h_manager;
    // TODO: 补完该处的缺省属性初始值:
    // 1. 新建管理整个页式结构的页面记录相关属性，如下一个pageID应该是多少
    // 2. 获得该索引所属的数据表的ID
    h_manager.header_initialize(ROOT_PAGEID, 0, 0, 0, LEAF_NODE, INDEX_DATA_PAGE, 0);
}