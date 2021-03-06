#include "BplusTree.h"
#include "../Header_define.h"
#include "../FS-manager/bufmanager/BufPageManager.h"
#include "../FS-manager/fileio/FileManager.h"
#include "../Page-manager/Header_manager.h"
#include <string.h>
#include <iostream>
#include <climits>
#include <vector>

BplusNode bn_block[60000];
int b_offset = 0;

BplusNode::BplusNode() {

}

BplusNode::~BplusNode() {
    
}

void BplusNode::node_initial(std::list<PKUnit> pkus1, bool leaf1, int pageID1, int objID1, int next_page_ID1) {
    leaf = leaf1;
    pageID = pageID1;
    objID = objID1;
    next_page_ID = next_page_ID1; 
    dirty = false;
    root = false;
    
    pkus = pkus1;  
}

/**
 * @brief 将数据页写回磁盘
 * 
 */
void BplusNode::data_write_back(BufPageManager* b_manager, int fileID) {
    // 使用文件管理器打开页面，文件名应为Index+所属表编号+页号
    // 获取缓存块 
    int buf_index = 0;
    BufType buf = b_manager->getPage(fileID, pageID, buf_index); 

    // 解析并更新索引页头（记录条数，第一个空闲记录的起始地址）
    Index_Header_info* ih = (Index_Header_info*)buf;
    ih->slotCnt = pkus.size();
    int old_size = ih->freeData;
    ih->freeData = sizeof(Index_Header_info) + pkus.size() * sizeof(PKUnit);

    // 将索引页头以及数据写入页中
    memcpy(&buf[0], ih, sizeof(Index_Header_info));
    int offset = 0;
    for (auto it = pkus.begin(); it != pkus.end(); it++) {
        memcpy(&buf[sizeof(Index_Header_info) + offset * sizeof(PKUnit)], &(*it), sizeof(PKUnit));
        offset++;
    }
    b_manager->markDirty(buf_index);
}

/**
 * @brief 更新非页节点的页面
 * 
 */
void BplusNode::pk_write_back(BufPageManager* b_manager, int fileID) {
    // 通过文件管理器打开当前节点所对应的页
    // 对页寻找缓存单元
    int buf_index = 0;
    BufType buf = b_manager->getPage(fileID, pageID, buf_index);
    // 更新索引页头
    Index_Header_info* ih = (Index_Header_info*)buf;
    ih->slotCnt = pkus.size();
    ih->freeData = sizeof(Index_Header_info) + pkus.size() * sizeof(PKUnit);
    ih->nextPage = next_page_ID;
    ih->level = NOT_LEAF_NODE;

    // 将索引页头和列表中的p_k对写入页面
    memcpy(buf, (uint8_t *)ih, sizeof(Index_Header_info));

    int offset = 0;
    for (auto it = pkus.begin(); it != pkus.end(); it++) {
        memcpy(&buf[sizeof(Index_Header_info) + offset*sizeof(PKUnit)], &(*it), sizeof(PKUnit));
        offset ++;
    }
    std::cout << std::endl;
    b_manager->markDirty(buf_index);
}

/**
 * @brief 在页分裂时分配新的索引页 
 * 
 * @param record_size 一条记录的长度 
 */
void BplusNode::alloc_new_page(bool leafNode, BufPageManager* b_manager, int fileID) {
    // 使用文件管理器创建新的索引页
    // 获取新页面的缓存块
    int buf_index = 0;
    BufType buf = b_manager->allocPage(fileID, pageID, buf_index);

    // 构建新的索引页头
    Header_manager h_manager;
    int next_pageID = 0;
    int prev_pageID = 0;
    if (leafNode == LEAF_NODE)
        h_manager.header_initialize(pageID, next_pageID, prev_pageID, objID, LEAF_NODE, INDEX_DATA_PAGE, sizeof(int));
    else    
        h_manager.header_initialize(pageID, next_pageID, prev_pageID, objID, 0, INDEX_DATA_PAGE, sizeof(int));
    
    Index_Header_info header = h_manager.get_header_info();
    memcpy(buf, (uint8_t *)(&header), sizeof(Index_Header_info));

    b_manager->markDirty(buf_index);
}

/**
 * @brief 当页中的记录条数过大时，分裂页表并创建新的页
 * 
 * @param is_leaf_node 
 * @param max_page_ID 
 * @return int 
 */
int BplusNode::spilt_page(int is_leaf_node, int max_page_ID, BufPageManager* b_manager, int fileID) {
    // 将本节点后一半元素转移到新的list中
    std::list<PKUnit> new_pkus;
    int size2 = pkus.size() / 2;
    int offset = 0;

    auto it = pkus.begin();
    while(offset < size2) {
        it++;
        offset++;
    }
    
    new_pkus.splice(new_pkus.begin(), pkus, it, pkus.end());

    // 构建新的BplusNode
    BplusNode* new_node = &bn_block[b_offset++];
    new_node->node_initial(new_pkus, is_leaf_node, max_page_ID, objID, next_page_ID);
    // 建立node间的连接关系
    next_page_ID = max_page_ID;

    // 为分裂节点分配新页面并写入
    if (is_leaf_node == LEAF_NODE) {
        new_node->alloc_new_page(LEAF_NODE, b_manager, fileID);
        new_node->data_write_back(b_manager, fileID);
        data_write_back(b_manager, fileID);
    } else {
        new_node->alloc_new_page(false, b_manager, fileID);
        new_node->pk_write_back(b_manager, fileID);    
        pk_write_back(b_manager, fileID); 
    }

    return (*new_pkus.begin()).k;
}

/**
 * @brief ：递归地将记录插入B+树节点中
 * 
 * @param i_pku ：插入的记录
 * @return true : 需要分页
 * @return false ：不需要分页
 */
int BplusNode::insert_PK_Unit(PKUnit i_pku, int& max_pageID, BufPageManager* b_manager, int fileID) {
    // 在叶节点中插入数据
    bool has_insert = false;
    if (leaf == LEAF_NODE) {
        for (auto it = pkus.begin(); it != pkus.end(); it++) {
            if (i_pku.k < (*it).k) {
                i_pku.p = pageID;
                pkus.insert(it, i_pku);
                has_insert = true; 
                break;
            }
        }
        if (has_insert == false) {
            pkus.push_back(i_pku);
        }

        if (sizeof(Index_Header_info) + sizeof(PKUnit) * pkus.size() > PAGE_SIZE >> 2){
            // 页分裂
            int next_key = spilt_page(LEAF_NODE, max_pageID, b_manager, fileID);
            max_pageID++;

            if (root == true) {
            // 对于根节点分裂还应该建立一个新的根节点
                std::list<PKUnit> new_pkus;
                PKUnit n_pku1(max_pageID, next_key);
                new_pkus.push_back(n_pku1);

                PKUnit n_pku2(max_pageID - 1, INT_MAX);
                new_pkus.push_back(n_pku2);

                BplusNode* new_node = &bn_block[b_offset++];
                new_node->node_initial(pkus, 0, max_pageID, objID, max_pageID - 1);

                new_node->alloc_new_page(LEAF_NODE, b_manager, fileID);
                new_node->data_write_back(b_manager, fileID);

                max_pageID++;

                // 当前节点（pageID = 0）称为非叶根节点
                leaf = NOT_LEAF_NODE;
                next_page_ID = 0;
                pkus.clear();
                pkus.splice(pkus.begin(), new_pkus);
                pk_write_back(b_manager, fileID);
            }
            return next_key;
        }
        else { // 不需分页，将更改后的页面信息写入页中
            data_write_back(b_manager, fileID);
            return INT_MAX;
        }
    }
    // 非叶节点插入pk对
    else { 
        int cutpage = 0;
        BplusNode* child_node;
        auto iterator = pkus.begin();
        for (auto it = pkus.begin(); it != pkus.end(); it++) {
            // 通过key值获取子节点的页号
            if (i_pku.k <= (*it).k) {
                auto map_it = child_map.find((*it).p);
                if (map_it != child_map.end())
                    child_node = map_it->second;
                else{
                    child_node = get_bplus_node((*it).p, b_manager, fileID);
                    child_map.insert(map<int, BplusNode*>::value_type((*it).p, child_node));
                }
                iterator = it;
                break;
            }
        }
        cutpage = child_node->insert_PK_Unit(i_pku, max_pageID, b_manager, fileID);

        // 插入新节点
        if (cutpage != INT_MAX) {
            // 若叶节点需要分页，应插入新p，k对
            // 更新当前p,k对的k值为分裂第二页的最小k值
            int old_key = (*iterator).k;
            int new_key = (*child_node->pkus.begin()).k;
            (*iterator).k = cutpage;
            // 在当前p,k对之后插入新的p,k对，其p值为分裂第二页的页编号，k值为当前p,k对的原k值
            PKUnit n_pku(max_pageID-1, old_key);

            // 在当前PKUnit之后插入新的PKUnit
            iterator++;
            pkus.insert(iterator, n_pku);

            // 中间节点分裂
            if (sizeof(Index_Header_info) + sizeof(PKUnit) * pkus.size() > PAGE_SIZE >> 2) {
                int k = spilt_page(1, max_pageID, b_manager, fileID);
                max_pageID ++;

            // 对于根节点分裂还应该建立一个新的根节点
                if (root == true) {
                    std::list<PKUnit> new_pkus;
                    PKUnit n_pku1(max_pageID, k);
                    new_pkus.push_back(n_pku1);

                    PKUnit n_pku2(max_pageID - 1, INT_MAX);
                    new_pkus.push_back(n_pku2);

                    BplusNode* new_node = &bn_block[b_offset++];
                    new_node->node_initial(pkus, 0, max_pageID, objID, 0);
                    new_node->alloc_new_page(NOT_LEAF_NODE, b_manager, fileID);
                    new_node->pk_write_back(b_manager, fileID);

                    max_pageID++;

                    leaf = NOT_LEAF_NODE;
                    next_page_ID = 0;
                    pkus.clear();
                    pkus.splice(pkus.begin(), new_pkus);
                    pk_write_back(b_manager, fileID);
                }
                return k;
            } else {
            // 中间节点不分裂
                pk_write_back(b_manager, fileID);
                return INT_MAX;
            }
        }
        else
            return INT_MAX;
    }
}

bool BplusNode::delete_PK_Unit(PKUnit d_pku, int& max_pageID, BufPageManager* b_manager, int fileID) {
    std::cout << "BplusNode delete PKUnit... k = " << d_pku.k << std::endl;
    bool has_delete = false;
    if (leaf == LEAF_NODE) {
        for (auto it = pkus.begin(); it != pkus.end(); it++) {
            if (d_pku.k == (*it).k) {
                pkus.erase(it);
                data_write_back(b_manager, fileID);
                return true;
            }
        }
        return false;
    } else {
        BplusNode* child_node;
        for (auto it = pkus.begin(); it != pkus.end(); it++) {
            if (d_pku.k <= (*it).k) {
                auto map_it = child_map.find((*it).p);
                if (map_it != child_map.end())
                    child_node = map_it->second;
                else {
                    child_node = get_bplus_node((*it).p, b_manager, fileID);
                    child_map.insert(map<int, BplusNode*>::value_type((*it).p, child_node));
                }
                return child_node->insert_PK_Unit(d_pku, max_pageID, b_manager, fileID);
            }
        }
        return false;
    }
}

/**
 * @brief 读取子节点页，将子节点页的信息加载到ChlidNode中
 * 
 * @param index 子节点页号p
 * @return BplusNode* 子节点
 */
BplusNode* BplusNode::get_bplus_node(int index, BufPageManager* b_manager, int fileID) {
    // 使用文件管理器打开子节点页面

    // 读取文件内容载入到buffer中
    int buf_index = 0;
    BufType buf = b_manager->getPage(fileID, index, buf_index);
    std::list<PKUnit> child_record;
 
    // 获取索引头
    Index_Header_info* ih = (Index_Header_info*)buf;
    int slotCnt = ih->slotCnt;
    int level = ih->level;
    int len = ih->pminlen;
    int nextPage = ih->nextPage;
    int objID = ih->objID;

    // 读取索引页中的所有数据
    for (int i = 0; i < slotCnt; i++) {
        PKUnit* pku = (PKUnit *)&buf[sizeof(Index_Header_info) + sizeof(PKUnit) * i];
        child_record.push_back(*pku);
    }

    BplusNode* child_node = &bn_block[b_offset++]; 
    child_node->node_initial(child_record, leaf, index, objID, nextPage);
    if(level == LEAF_NODE) child_node->setLeaf(LEAF_NODE);
    else child_node->setLeaf(NOT_LEAF_NODE);
    child_record.clear();

    return child_node;
}

/**
 * @brief 
 * 
 * @param k 主索引值
 * @param record 返回的记录值（存在uint8_t数组中）
 * @return true 成功找到记录
 * @return false 未找到记录
 */
int BplusNode::get_record(int k, BufPageManager* b_manager, int fileID) {
    if (leaf == LEAF_NODE) {
        for (auto it = pkus.begin(); it != pkus.end(); it++) {
            if ((*it).k == k) {
                return (*it).record;
            }
        }
        return -1;
    } else {
        for (auto it = pkus.begin(); it != pkus.end(); it++) {
            if ((*it).k > k) {
                auto map_it = child_map.find((*it).p);
                if (map_it != child_map.end())
                    return map_it->second->get_record(k, b_manager, fileID);

                BplusNode* child_node = get_bplus_node((*it).p, b_manager, fileID);
                child_map.insert(map<int, BplusNode*>::value_type((*it).p, child_node));
                return child_node->get_record(k, b_manager, fileID);
            }
        }
        return -1;
    }
}

/**
 * @brief 判断本节点是否是叶节点
 * 
 * @return true 
 * @return false 
 */
bool BplusNode::isLeaf() {
    return leaf;
}

void BplusNode::setLeaf(bool l) {
    leaf = l;
}

bool BplusNode::isDirty() {
    return dirty;
}

void BplusNode::setRoot(bool isroot) {
    root = isroot;
}

/**
 * @brief 返回当前节点中共有多少个键值对或多少条记录
 * 
 * @return int 
 */
int BplusNode::get_size() {
    return pkus.size();
}


BplusTree::BplusTree() {
}

BplusTree::~BplusTree() {
    // b_manager->~BufPageManager();
    // f_manager->closeFile(fileID);
    // f_manager->~FileManager();
}

/**
 * @brief 返回B+树的大小
 * 
 * @return int 
 */
int BplusTree::get_size() {
    return size;
}

/**
 * @brief 创建新的B+树,为新的B+树开启新的页式文件
 * 
 * @param objID B+树所属的表ID
 */
void BplusTree::create_new_tree(int objID, std::string database, std::string index_name) {
    std::list<PKUnit> root_list;
    root = &bn_block[b_offset++];
    root->node_initial(root_list, LEAF_NODE, 0, objID, 0);

    f_manager = new FileManager();
    string str_filename = "Index_" + database + "_" + std::to_string(objID) + "_" + index_name;
    const char* filename = str_filename.data();
    f_manager->createFile(filename);

    std::cout << "open\n";
    f_manager->openFile(filename, fileID);

    b_manager = new BufPageManager(f_manager);

    root->setRoot(true);
    root->alloc_new_page(LEAF_NODE, b_manager, fileID);
}

/**
 * @brief 获取特定主索引的B+树根
 * 
 * @param objID B+树所属的表ID
 */
void BplusTree::initial_tree_root(int objID, std::string database, std::string index_name) {
    std::list<PKUnit> root_list;
    root = &bn_block[b_offset++]; 
    root->node_initial(root_list, LEAF_NODE, 0, objID, 0);

    f_manager = new FileManager();
    string str_filename = "Index_" + database + "_" + std::to_string(objID) + "_" + index_name;
    const char* filename = str_filename.data();
    f_manager->openFile(filename, fileID);

    b_manager = new BufPageManager(f_manager);

    root->setRoot(true);
    root = root->get_bplus_node(0, b_manager, fileID);  

    get_maxpageID();
}

/**
 * @brief 将数据插入B+树
 * 
 * @param record 
 * @param len 
 */
void BplusTree::insert_record(int k, int record) {
    PKUnit n_pku(0, k);
    n_pku.record = record;
    
    cout << "Insert : " << " k = " << k << " record = " << record << endl;
    root->insert_PK_Unit(n_pku, max_pageID, b_manager, fileID);
}

void BplusTree::delete_record(int k) {
    PKUnit n_pku(0, k);
    root->delete_PK_Unit(n_pku, max_pageID, b_manager, fileID);
}

int BplusTree::get_record(int key) {
    return root->get_record(key, b_manager, fileID);
}
 
void BplusTree::write_back() {
    cout << "write_back" << endl; 
    if (b_manager == nullptr)
        cout << "null" << endl; 
    b_manager->close();
}

BplusNode* BplusTree::get_root() {
    return root;
}

void BplusTree::root_debug() {
    std::cout << "debug\n";
    f_manager->closeFile(fileID);
    std::cout << "close\n";
}

void BplusTree::get_maxpageID() {
    int bID = 0;
    BufType bt = b_manager->getPage(fileID, 0, bID);
    Index_Header_info *i_h = (Index_Header_info *)bt;
    max_pageID = i_h->maxPageID;
    b_manager->release(bID);
}

void BplusTree::update_maxpageID() {
    int bID = 0;
    BufType bt = b_manager->getPage(fileID, 0, bID);
    Index_Header_info *i_h = (Index_Header_info *)bt;
    i_h->maxPageID = max_pageID;
    memcpy(bt, i_h, sizeof(Index_Header_info));
    b_manager->markDirty(bID);        
}