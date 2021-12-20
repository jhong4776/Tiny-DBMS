#include "BplusTree.h"
#include "../Header_define.h"
#include "../bufmanager/BufPageManager.h"
#include "../FS-manager/fileio/FileManager.h"
#include "../Page-manager/Header_manager.h"
#include <string.h>

struct P_K
{
    int p = 0;
    int k = 0;
}pk_temp;


BplusNode::BplusNode() {

}

BplusNode::~BplusNode() {
    
}

BplusNode::BplusNode(std::list<PKUnit*> pkus1, bool leaf1, int pageID1, int objID1) {
    pkus = pkus1;
    leaf = leaf1;
    pageID = pageID1;
    objID = objID1;
    father = nullptr;
    left = nullptr;
    right = nullptr;
}

int BplusNode::size_to_mask(int size) {
    int mask = 0;
    int off = 1;
    for (int i = 0; i < size; i++) {
        mask |= off;
        off << 1;
    }
    return mask;
}

int BplusNode::mask_to_size(int mask) {
    int size = 0;
    int off = 1;
    for (int i = 0; i < 32; i++) {
        if (off & mask != 0)
            size += 1;
        else    
            break;
    }
    return size;
}

/**
 * @brief 将数据页写回磁盘
 * 
 */
void BplusNode::data_write_back() {
    FileManager *f_manager = new FileManager();
    int file_ID = 0;
    string str_filename = "Index" + std::to_string(objID) + "_" + std::to_string(pageID);
    const char* filename = str_filename.data();
    f_manager->openFile(filename, file_ID);

    BufPageManager *b_manager = new BufPageManager(f_manager);
    int buf_index = 0;
    BufType buf = b_manager->getPage(file_ID, pageID, buf_index);

    Index_Header_info* ih = (Index_Header_info*)buf;
    ih->slotCnt += pkus.size();
    ih->freeData += sizeof(Index_Header_info) + pkus.size() * pkus->begin()->record_len;
    ih->nullmap = size_to_mask(pkus.size());

    memcpy(buf, (uint8_t *)ih, sizeof(Index_Header_info));
    int offset = 0;
    for (auto it = pkus.begin(); it != pkus.end(); it++) {
        // TODO：是否应当把BufType改为以byte为单位
        memcpy(&buf[sizeof(Index_Header_info) + offset * (*it)->record_len], (*it)->record, (*it)->record_len);
    }
    b_manager->markDirty(buf_index);
    b_manager->close();

    f_manager->closeFile(file_ID);
}

void BplusNode::pk_write_back() {
    FileManager *f_manager = new FileManager();
    int file_ID = 0;
    string str_filename = "Index" + std::to_string(objID) + "_" + std::to_string(pageID);
    const char* filename = str_filename.data();
    f_manager->openFile(filename, file_ID);

    BufPageManager *b_manager = new BufPageManager(f_manager);
    int buf_index = 0;
    BufType buf = b_manager->getPage(file_ID, pageID, buf_index);

    Index_Header_info* ih = (Index_Header_info*)buf;
    ih->slotCnt += pkus.size();
    ih->freeData += sizeof(Index_Header_info) + pkus.size() * 8;
    ih->nullmap = size_to_mask(pkus.size());

    memcpy(buf, (uint8_t *)ih, sizeof(Index_Header_info));
    int offset = 0;
    for (auto it = pkus.begin(); it != pkus.end(); it++) {
        pk_temp.p = (*it)->p;
        pk_temp.k = (*it)->k;
        memcpy(&buf[sizeof(Index_Header_info) + offset*8], (uint8_t*)&pk_temp, sizeof(P_K));
    }
    b_manager->markDirty(buf_index);
    b_manager->close();
    f_manager->closeFile(file_ID);
}

void BplusNode::alloc_new_page(int page_size) {
    FileManager *f_manager = new FileManager();
    int file_ID = 0;
    string str_filename = "Index" + std::to_string(objID) + "_" + std::to_string(pageID);
    const char* filename = str_filename.data();
    f_manager->createFile(filename);
    f_manager->openFile(filename, file_ID);

    BufPageManager *b_manager = new BufPageManager(f_manager);
    int buf_index = 0;
    BufType buf = b_manager->allocPage(file_ID, pageID, buf_index);

    Header_manager h_manager;
    int next_pageID = 0;
    int prev_pageID = 0;
    if(right != nullptr) next_pageID = right->pageID;
    if(left != nullptr) prev_pageID = prev_pageID;
    h_manager.header_initialize(pageID, next_pageID, prev_pageID, objID, LEAF_NODE, INDEX_DATA_PAGE, page_size);
    memcpy(buf, (uint8_t *)(&h_manager.get_header_info()), sizeof(Index_Header_info));
    b_manager->markDirty(buf_index);
    b_manager->close();

    f_manager->closeFile(file_ID);
}

int BplusNode::spilt_page(int is_leaf_node, int max_page_ID) {
    // 将本节点后一半元素转移到新的list中
    std::list<PKUnit*> new_pkus;
    int size2 = pkus.size() / 2;
    int offset = 0;
    for (auto it = pkus.begin(); it != pkus.end(); it++) {
        if (offset >= size2) {
            new_pkus.splice(new_pkus.end(), pkus, it);
        }
        offset++;
    }
    // 构建新的BplusNode
    BplusNode new_node(new_pkus, is_leaf_node, max_page_ID, objID);
    // 建立node间的连接关系
    // Todo：如何在页文件中表示这种连接关系
    new_node.left = this;
    if (this->right != nullptr){
        new_node.right = right;
        right->left = &new_node;
    } 
    else
        new_node.right = nullptr;
    this->right = &new_node; 
    // 为分裂节点分配新页面并写入
    if (is_leaf_node == LEAF_NODE) {
        new_node.alloc_new_page((*new_pkus.begin())->record_len);
        new_node.data_write_back();
        data_write_back();
    } else {
        new_node.alloc_new_page(sizeof(P_K));
        new_node.pk_write_back();
        pk_write_back();        
    }

    return (*new_pkus.begin())->k;
}

/**
 * @brief ：递归地将记录插入B+树节点中
 * 
 * @param i_pku ：插入的记录
 * @return true : 需要分页
 * @return false ：不需要分页
 */
bool BplusNode::insert_PK_Unit(PKUnit* i_pku, int& max_pageID) {
    // 在叶节点中插入数据
    if (leaf == true) {
        for (auto it = pkus.begin(); it != pkus.end(); it++) {
            if (i_pku->k < (*it)->k) {
                pkus.insert(it, i_pku);
                break;
            }
        }
        if (sizeof(Index_Header_info) + i_pku->record_len * pkus.size() > PAGE_SIZE >> 2){
            // 页分裂
            int next_key = spilt_page(LEAF_NODE, max_pageID);
            max_pageID++;

            if (root == true) {
            // 对于根节点分裂还应该建立一个新的根节点
                root = false;  
                std::list<PKUnit*> new_pkus;
                PKUnit new_pkus(max_pageID, next_key, i_pku->record_len);
                new_pkus.push_back(new_pkus);

                BplusNode new_node(new_pkus, 0, max_pageID, objID);
                new_node.alloc_new_page(sizeof(P_K));
                new_node.data_write_back();

                max_pageID++;
            }
            return true;
        }
        else { // 不需分页，将更改后的页面信息写入页中
            data_write_back();
            return false;
        }
    }
    // 非叶节点插入pk对
    else { 
        bool cutpage = false;
        BplusNode* child_node;
        auto iterator = pkus.begin();
        for (auto it = pkus.begin(); it != pkus.end(); it++) {
            // 通过key值获取子节点的页号
            if (i_pku->k <= (*it)->k) {
                child_node = get_bplus_node((*it)->p);
                iterator = it;
                break;
            }
        }
        cutpage = child_node->insert_PK_Unit(i_pku, max_pageID);

        // 插入新节点
        if (cutpage == true) {
            // 若叶节点需要分页，应插入新p，k对
            // 更新当前p,k对的k值为分裂第二页的最小k值
            int old_key = (*iterator)->k;
            int new_key = child_node->pkus.begin()->k;
            (*iterator)->k = new_key;
            // 在当前p,k对之后插入新的p,k对，其p值为分裂第二页的页编号，k值为当前p,k对的原k值
            int new_p = child_node->right->pageID;
            int len = i_pku->record_len;
            PKUnit new_pku(new_p, new_key, len);
            // 在当前PKUnit之后插入新的PKUnit
            iterator++;
            pkus.insert(iterator, new_pku);
        }

        // 中间节点分裂
        if (sizeof(Index_Header_info) + 8 * pkus.size() > PAGE_SIZE >> 2) {
            spilt_page(1, max_pageID);
            max_pageID ++;

        // 对于根节点分裂还应该建立一个新的根节点
            root = false;  
            std::list<PKUnit*> new_pkus;
            PKUnit new_pkus(max_pageID, next_key, i_pku->record_len);
            new_pkus.push_back(new_pkus);

            BplusNode new_node(new_pkus, 0, max_pageID, objID);
            new_node.alloc_new_page(sizeof(P_K));
            new_node.data_write_back();

            max_pageID++;
            return true;
        } else {
        // 中间节点不分裂
            data_write_back();
            return false;
        }
    }
}