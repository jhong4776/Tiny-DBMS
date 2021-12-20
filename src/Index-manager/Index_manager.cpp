#include "Index_manager.h"
#include "../Page-manager/Header_manager.h"
#include "../FS-manager/fileio/FileManager.h"
#include "../bufmanager/BufPageManager.h"
#include <string.h>

IndexHandler::IndexHandler() {}

IndexHandler::~IndexHandler() {}

/**
 * 创建索引页，从根节点开始创建
 * root
 * @param filename 新建根索引页的文件名
*/
RC IndexHandler::CreateIndex(const char *filename) {
    FileManager *f_manager = new FileManager();
    int file_ID = 0;
    f_manager->createFile(filename);
    f_manager->openFile(filename, file_ID);

    BufPageManager *b_manager = new BufPageManager(f_manager);
    int buf_index = 0;
    BufType buf = b_manager->allocPage(file_ID, ROOT_PAGEID, buf_index);

    Header_manager h_manager;
    // TODO: 补完该处的缺省属性初始值:
    // 1. 新建管理整个页式结构的页面记录相关属性，如下一个pageID应该是多少
    // 2. 获得该索引所属的数据表的ID
    h_manager.header_initialize(ROOT_PAGEID, 0, 0, 0, LEAF_NODE, INDEX_DATA_PAGE, 0);
    memcpy(buf, (uint8_t *)(&h_manager.get_header_info()), sizeof(Index_Header_info));
    b_manager->markDirty(buf_index);
    b_manager->close();

    f_manager->closeFile(file_ID);

    // TODO: 弄清楚返回的RC是什么
}

