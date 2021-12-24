#include "BplusTree.h"
#include "../Header_define.h"
#include "../FS-manager/bufmanager/BufPageManager.h"
#include "../FS-manager/fileio/FileManager.h"
#include "../Page-manager/Header_manager.h"
#include <string.h>
#include <iostream>
#include <climits>
#include <vector>

struct P_K
{
    int p = 0;
    int k = 0;
}pk_temp;


BplusNode bn_block[60000];
int b_offset = 0;

BplusNode::BplusNode() {

}

BplusNode::~BplusNode() {
    
}

void BplusNode::node_initial(std::list<int> pkus1, bool leaf1, int pageID1, int objID1, int next_page_ID1) {
    std::cout << "BplusNode Creating..." << std::endl;
    std::cout << "leaf1 = " << leaf1 << " pageID = " << pageID1 << " objID1 = " << objID1 << " next_page_ID1 = " << next_page_ID1 << std::endl;
    leaf = leaf1;
    pageID = pageID1;
    objID = objID1;
    next_page_ID = next_page_ID1; 
    dirty = false;
    root = false;
    std::cout << "pku list: " << std::endl;
    
    pkus = pkus1;
    std::cout << "BplusNode Created!" << std::endl;    
}

/**
 * @brief 将列表元素个数转换成null位图的格式，null位图有32位，最低位表示列表的第一项，以此类推
 * 
 * @param size 列表的元素个数
 * @return int 返回null位图
 */
int BplusNode::size_to_mask(int size) {
    int mask = 0;
    int off = 1;
    for (int i = 0; i < size; i++) {
        mask |= off;
        off << 1;
    }
    return mask;
}

/**
 * @brief 将null位图转化成列表大小
 * 
 * @param mask 
 * @return int 
 */
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
void BplusNode::data_write_back(BufPageManager* b_manager, int fileID, int& p_offset, PKUnit* pku_block) {
    // 使用文件管理器打开页面，文件名应为Index+所属表编号+页号
    std::cout << "data_write_back" << std::endl;
    // 获取缓存块 
    int buf_index = 0;
    BufType buf = b_manager->getPage(fileID, pageID, buf_index); 

    // 解析并更新索引页头（记录条数，第一个空闲记录的起始地址，null位图）
    Index_Header_info* ih = (Index_Header_info*)buf;
    ih->slotCnt = pkus.size();
    int old_size = ih->freeData;
    ih->freeData = sizeof(Index_Header_info) + pkus.size() * pku_block[(*pkus.begin())].record_len;
    ih->nullmap = size_to_mask(pkus.size());

    std::cout << "data slotCnt = " << ih->slotCnt << " level = " << leaf << std::endl;

    // 将索引页头以及数据写入页中
    memcpy(&buf[0], (uint8_t *)ih, sizeof(Index_Header_info));
    int offset = 0;
    for (auto it = pkus.begin(); it != pkus.end(); it++) {
        // TODO：是否应当把BufType改为以byte为单位
        int index = (*it);
        memcpy(&buf[sizeof(Index_Header_info) + offset * pku_block[index].record_len], pku_block[index].record, pku_block[index].record_len);
        offset++;
    }
    b_manager->markDirty(buf_index);
}

/**
 * @brief 更新非页节点的页面
 * 
 */
void BplusNode::pk_write_back(BufPageManager* b_manager, int fileID, int& p_offset, PKUnit* pku_block) {
    // 通过文件管理器打开当前节点所对应的页
    std::cout << "pk_write_back" << std::endl;
    // 对页寻找缓存单元
    int buf_index = 0;
    BufType buf = b_manager->getPage(fileID, pageID, buf_index);
    // 更新索引页头
    Index_Header_info* ih = (Index_Header_info*)buf;
    ih->slotCnt = pkus.size();
    ih->freeData = sizeof(Index_Header_info) + pkus.size() * sizeof(P_K);
    ih->nullmap = size_to_mask(pkus.size());
    ih->nextPage = next_page_ID;
    ih->level = NOT_LEAF_NODE;

    // 将索引页头和列表中的p_k对写入页面
    memcpy(buf, (uint8_t *)ih, sizeof(Index_Header_info));

    int offset = 0;
    std::cout << "size = " << pkus.size() << std::endl;
    for (auto it = pkus.begin(); it != pkus.end(); it++) {
        pk_temp.p = pku_block[(*it)].p;
        pk_temp.k = pku_block[(*it)].k;
        memcpy(&buf[sizeof(Index_Header_info) + offset*8], (uint8_t*)&pk_temp, sizeof(P_K));
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
void BplusNode::alloc_new_page(int record_size, bool leafNode, BufPageManager* b_manager, int fileID, int& p_offset) {
    // 使用文件管理器创建新的索引页
    std::cout << "new_page allocating..." << std::endl;
    std::cout << "record_size = " << record_size << " leafNode = " << leafNode << std::endl;

    std::cout << "f_manager created!" << std::endl;
    // 获取新页面的缓存块
    int buf_index = 0;
    BufType buf = b_manager->allocPage(fileID, pageID, buf_index);

    std::cout << buf_index << " " << fileID << std::endl;

    // 构建新的索引页头
    Header_manager h_manager;
    int next_pageID = 0;
    int prev_pageID = 0;
    if (leafNode == LEAF_NODE)
        h_manager.header_initialize(pageID, next_pageID, prev_pageID, objID, LEAF_NODE, INDEX_DATA_PAGE, record_size);
    else    
        h_manager.header_initialize(pageID, next_pageID, prev_pageID, objID, 0, INDEX_DATA_PAGE, record_size);
    
    Index_Header_info header = h_manager.get_header_info();
    std::cout << header.pageID << " " << header.objID << " " << header.pminlen << std::endl;
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
int BplusNode::spilt_page(int is_leaf_node, int max_page_ID, BufPageManager* b_manager, int fileID, int& p_offset, PKUnit* pku_block) {
    // 将本节点后一半元素转移到新的list中
    std::cout << "SPILT" << std::endl;
    std::list<int> new_pkus;
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
        new_node->alloc_new_page(pku_block[(*new_pkus.begin())].record_len, LEAF_NODE, b_manager, fileID, p_offset);
        new_node->data_write_back(b_manager, fileID, p_offset, pku_block);
        data_write_back(b_manager, fileID, p_offset, pku_block);
    } else {
        new_node->alloc_new_page(sizeof(P_K), false, b_manager, fileID, p_offset);
        new_node->pk_write_back(b_manager, fileID, p_offset, pku_block);    
        pk_write_back(b_manager, fileID, p_offset, pku_block); 
    }
    std::cout << "after write_back" << std::endl;

    return pku_block[*new_pkus.begin()].k;
}

/**
 * @brief ：递归地将记录插入B+树节点中
 * 
 * @param i_pku ：插入的记录
 * @return true : 需要分页
 * @return false ：不需要分页
 */
bool BplusNode::insert_PK_Unit(PKUnit i_pku, int& max_pageID, BufPageManager* b_manager, int fileID, int& p_offset, PKUnit* pku_block) {
    // 在叶节点中插入数据
    std::cout << "BplusNode insert PKUnit... k = " << i_pku.k << std::endl;
    bool has_insert = false;
    if (leaf == LEAF_NODE) {
        std::cout << "leaf insert " << pageID << std::endl;
        for (auto it = pkus.begin(); it != pkus.end(); it++) {
            if (i_pku.k < pku_block[(*it)].k) {
                std::cout << "(" << i_pku.k << "," << i_pku.index << ")" << std::endl;
                pku_block[i_pku.index].p = pageID;
                pkus.insert(it, i_pku.index);
                has_insert = true; 
                break;
            }
        }
        if (has_insert == false) {
            std::cout << "2(" << i_pku.k << "," << i_pku.index << ")" << std::endl;
            pkus.push_back(i_pku.index);
        }
        std::cout << "insert already!" << std::endl;

        if (sizeof(Index_Header_info) + i_pku.record_len * pkus.size() > PAGE_SIZE >> 2){
            // 页分裂
            int next_key = spilt_page(LEAF_NODE, max_pageID, b_manager, fileID, p_offset, pku_block);
            std::cout << "next_key = " << next_key << std::endl;
            max_pageID++;

            if (root == true) {
            // 对于根节点分裂还应该建立一个新的根节点
                std::cout << "Spilt new root" << std::endl; 
                std::list<int> new_pkus;
                pku_block[p_offset].index = p_offset; pku_block[p_offset].k = next_key; pku_block[p_offset].p = max_pageID; pku_block[p_offset].record_len = i_pku.record_len;
                new_pkus.push_back(p_offset++);

                pku_block[p_offset].index = p_offset; pku_block[p_offset].k = INT_MAX; pku_block[p_offset].p = max_pageID - 1; pku_block[p_offset].record_len = i_pku.record_len;
                new_pkus.push_back(p_offset++);

                BplusNode* new_node = &bn_block[b_offset++];
                new_node->node_initial(pkus, 0, max_pageID, objID, max_pageID - 1);

                new_node->alloc_new_page(pku_block[(*pkus.begin())].record_len, LEAF_NODE, b_manager, fileID, p_offset);
                new_node->data_write_back(b_manager, fileID, p_offset, pku_block);

                max_pageID++;

                // 当前节点（pageID = 0）称为非叶根节点
                leaf = NOT_LEAF_NODE;
                next_page_ID = 0;
                pkus.clear();
                pkus.splice(pkus.begin(), new_pkus);
                pk_write_back(b_manager, fileID, p_offset, pku_block);
            }
            return true;
        }
        else { // 不需分页，将更改后的页面信息写入页中
            data_write_back(b_manager, fileID, p_offset, pku_block);
            std::cout << "data write back ok!" <<std::endl;
            return false;
        }
    }
    // 非叶节点插入pk对
    else { 
        std::cout << "not leaf insert " << pageID << std::endl;
        bool cutpage = false;
        BplusNode* child_node;
        auto iterator = pkus.begin();
        for (auto it = pkus.begin(); it != pkus.end(); it++) {
            // 通过key值获取子节点的页号
            if (i_pku.k <= pku_block[(*it)].k) {
                std::cout << "0leaf = " << leaf << std::endl;
                std::cout << "pku_block[(*it)].k = " << pku_block[(*it)].k << " pku_block[(*it)].p = " << pku_block[(*it)].p << std::endl;
                child_node = get_bplus_node(pku_block[(*it)].p, b_manager, fileID, p_offset, pku_block);
                std::cout << "1leaf = " << leaf << std::endl;
                iterator = it;
                break;
            }
        }
        cutpage = child_node->insert_PK_Unit(i_pku, max_pageID, b_manager, fileID, p_offset, pku_block);
        std::cout << "leaf = " << leaf << std::endl;

        // 插入新节点
        if (cutpage == true) {
            // 若叶节点需要分页，应插入新p，k对
            // 更新当前p,k对的k值为分裂第二页的最小k值
            int old_key = pku_block[(*iterator)].k;
            int new_key = pku_block[(*child_node->pkus.begin())].k;
            pku_block[(*iterator)].k = new_key;
            // 在当前p,k对之后插入新的p,k对，其p值为分裂第二页的页编号，k值为当前p,k对的原k值
            int new_p = next_page_ID;
            int len = i_pku.record_len;
            pku_block[p_offset].index = p_offset;
            pku_block[p_offset].k = old_key; pku_block[p_offset].p = new_p; pku_block[p_offset].record_len = len;

            // 在当前PKUnit之后插入新的PKUnit
            iterator++;
            pkus.insert(iterator, p_offset++);

            // 中间节点分裂
            if (sizeof(Index_Header_info) + 8 * pkus.size() > PAGE_SIZE >> 2) {
                int k = spilt_page(1, max_pageID, b_manager, fileID, p_offset, pku_block);
                max_pageID ++;

            // 对于根节点分裂还应该建立一个新的根节点
                if (root == true) {
                    std::list<int> new_pkus;
                    pku_block[p_offset].k = k; pku_block[p_offset].p = max_pageID; pku_block[p_offset].record_len = i_pku.record_len; pku_block[p_offset].index = p_offset;
                    new_pkus.push_back(p_offset++);

                    pku_block[p_offset].k = INT_MAX; pku_block[p_offset].p = max_pageID-1; pku_block[p_offset].record_len = i_pku.record_len; pku_block[p_offset].index = p_offset;
                    new_pkus.push_back(p_offset++);

                    BplusNode* new_node = &bn_block[b_offset++];
                    new_node->node_initial(pkus, 0, max_pageID, objID, 0);
                    new_node->alloc_new_page(sizeof(P_K), NOT_LEAF_NODE, b_manager, fileID, p_offset);
                    new_node->pk_write_back(b_manager, fileID, p_offset, pku_block);

                    max_pageID++;

                    leaf = NOT_LEAF_NODE;
                    next_page_ID = 0;
                    pkus.clear();
                    pkus.splice(pkus.begin(), new_pkus);
                    pk_write_back(b_manager, fileID, p_offset, pku_block);
                }
                return true;
            } else {
            // 中间节点不分裂
                std::cout << "not split not leaf" << pageID << std::endl;
                pk_write_back(b_manager, fileID, p_offset, pku_block);
                return false;
            }
        }
        else
            return false;
    }
}

/**
 * @brief 读取子节点页，将子节点页的信息加载到ChlidNode中
 * 
 * @param index 子节点页号p
 * @return BplusNode* 子节点
 */
BplusNode* BplusNode::get_bplus_node(int index, BufPageManager* b_manager, int fileID, int& p_offset, PKUnit* pku_block) {
    // 使用文件管理器打开子节点页面

    // 读取文件内容载入到buffer中
    int buf_index = 0;
    BufType buf = b_manager->getPage(fileID, index, buf_index);
    std::cout << "get buf index = " << pageID << std::endl;
    std::list<int> child_record;
 
    // 获取索引头
    Index_Header_info* ih = (Index_Header_info*)buf;
    int slotCnt = ih->slotCnt;
    int level = ih->level;
    int len = ih->pminlen;
    int nextPage = ih->nextPage;

    std::cout << "slotCnt = " << slotCnt << " level = " << level << " len = " << len << " nextPage = " << nextPage << std::endl;

    // 读取索引页中的所有数据
    for (int i = 0; i < slotCnt; i++) {
        pku_block[p_offset].index = p_offset;
        if (level == LEAF_NODE) {
            // 读取叶节点的数据存入pku的record数组中
            memcpy(pku_block[p_offset].record, (uint8_t *)&buf[sizeof(Index_Header_info) + len*i], len);
            pku_block[p_offset].k = (*(int *)&buf[sizeof(Index_Header_info) + len*i]);
            pku_block[p_offset].record_len = len;
            child_record.push_back(p_offset++);
        } else {
            P_K* child_pk = (P_K *)&buf[sizeof(Index_Header_info) + sizeof(P_K)*i];
            pku_block[p_offset].p = child_pk->p;
            pku_block[p_offset].k = child_pk->k;
            pku_block[p_offset].record_len = sizeof(P_K);
            child_record.push_back(p_offset++);
        }
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
bool BplusNode::get_record(int k, uint8_t* record, BufPageManager* b_manager, int fileID, int& p_offset, PKUnit* pku_block) {
    std::cout << "bn::get_record" << std::endl;
    if (leaf == LEAF_NODE) {
        std::cout << "get_record LEAF NODE " << pkus.size() << std::endl;
        for (auto it = pkus.begin(); it != pkus.end(); it++) {
            if (pku_block[(*it)].k == k) {
                std::cout << "get!" << std::endl;
                memcpy(record, pku_block[(*it)].record, pku_block[(*it)].record_len);
                return true;
            }
        }
        std::cout << "not found!" << std::endl;
        return false;
    } else {
        std::cout << "get_record NOT LEAF NODE " << pkus.size() << std::endl;
        for (auto it = pkus.begin(); it != pkus.end(); it++) {
            if (pku_block[(*it)].k > k) {
                auto map_it = child_map.find(pku_block[(*it)].p);
                if (map_it != child_map.end())
                    return map_it->second->get_record(k, record, b_manager, fileID, p_offset, pku_block);

                BplusNode* child_node = get_bplus_node(pku_block[(*it)].p, b_manager, fileID, p_offset, pku_block);
                child_map.insert(map<int, BplusNode*>::value_type(pku_block[(*it)].p, child_node));
                return child_node->get_record(k, record, b_manager, fileID, p_offset, pku_block);
            }
        }
        return false;
    }
}

/**
 * @brief 判断本节点是否是叶节点
 * 
 * @return true 
 * @return false 
 */
bool BplusNode::isLeaf() {
    std::cout << "leaf = " << leaf << std::endl;
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
void BplusTree::create_new_tree(int objID, int len, int& p_offset) {
    std::list<int> root_list;
    root = &bn_block[b_offset++];
    root->node_initial(root_list, LEAF_NODE, 0, objID, 0);

    FileManager *f_manager = new FileManager();
    string str_filename = "Index" + std::to_string(objID);
    const char* filename = str_filename.data();
    f_manager->createFile(filename);

    f_manager->openFile(filename, fileID);

    b_manager = new BufPageManager(f_manager);

    root->setRoot(true);
    root->alloc_new_page(len, LEAF_NODE, b_manager, fileID, p_offset);
}

/**
 * @brief 获取特定主索引的B+树根
 * 
 * @param objID B+树所属的表ID
 */
void BplusTree::initial_tree_root(int objID, int& p_offset) {
    std::cout << "get_tree_root..." << std::endl;
    std::list<int> root_list;
    root = &bn_block[b_offset++]; 
    root->node_initial(root_list, LEAF_NODE, 0, objID, 0);

    FileManager *f_manager = new FileManager();
    string str_filename = "Index" + std::to_string(objID);
    const char* filename = str_filename.data();
    f_manager->openFile(filename, fileID);

    b_manager = new BufPageManager(f_manager);

    root->setRoot(true);
    root = root->get_bplus_node(0, b_manager, fileID, p_offset, pku_block);  
}

/**
 * @brief 将数据插入B+树
 * 
 * @param record 
 * @param len 
 */
void BplusTree::insert_record(uint8_t* record, int len) {
    int max_pageID = 1;

    pku_block[p_offset].index = p_offset;
    pku_block[p_offset].record_len = len;
    pku_block[p_offset].p = 0;
    int* key = (int *)record;
    pku_block[p_offset].k = *key;
    memcpy(pku_block[p_offset].record, record, len);

    root->insert_PK_Unit(pku_block[p_offset++], max_pageID, b_manager, fileID, p_offset, pku_block);
}

bool BplusTree::get_record(int key, uint8_t* record) {
    std::cout << "bt::get_record\n";
    if (root == nullptr)
        std::cout << "null" <<std::endl;
    return root->get_record(key, record, b_manager, fileID, p_offset, pku_block);
}

void BplusTree::write_back() {
    b_manager->close();
}

BplusNode* BplusTree::get_root() {
    return root;
}

void BplusTree::root_debug() {
    for (int i = 0; i < p_offset; i++) {
        std::cout << "(" << pku_block[i].k << "," << pku_block[i].p << "," << pku_block[i].record_len << ") ";
    }
    std::cout << std::endl;
}