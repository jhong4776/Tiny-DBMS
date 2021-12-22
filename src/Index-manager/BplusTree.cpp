#include "BplusTree.h"
#include "../Header_define.h"
#include "../FS-manager/bufmanager/BufPageManager.h"
#include "../FS-manager/fileio/FileManager.h"
#include "../Page-manager/Header_manager.h"
#include <string.h>
#include <iostream>
#include <climits>

struct P_K
{
    int p = 0;
    int k = 0;
}pk_temp;


BplusNode::BplusNode() {

}

BplusNode::~BplusNode() {
    
}

/**
 * @brief Construct a new Bplus Node:: Bplus Node object
 * 
 * @param pkus1 新的数据列表或(p,k)键值列表
 * @param leaf1 是否为叶节点，(=LEAF_NODE：是叶节点，=0：非叶节点)
 * @param pageID1 本页的ID
 * @param objID1 所属对象的ID
 */
BplusNode::BplusNode(std::list<PKUnit*> pkus1, bool leaf1, int pageID1, int objID1, int next_page_ID1) {
    for (auto it = pkus1.begin(); it != pkus1.end(); it++) {
        pkus.push_back((*it));
    }
    std::cout << "BplusNode Creating..." << std::endl;
    std::cout << "leaf1 = " << leaf1 << " pageID = " << pageID1 << " objID1 = " << objID1 << " next_page_ID1 = " << next_page_ID1 << std::endl;
    leaf = leaf1;
    pageID = pageID1;
    objID = objID1;
    next_page_ID = next_page_ID1;
    dirty = false;
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
void BplusNode::data_write_back() {
    // 使用文件管理器打开页面，文件名应为Index+所属表编号+页号
    FileManager *f_manager = new FileManager();
    int file_ID = 0;
    string str_filename = "Index" + std::to_string(objID) + "_" + std::to_string(pageID);
    const char* filename = str_filename.data();
    f_manager->openFile(filename, file_ID);
    std::cout << "Data_write_back... filename = " << filename << std::endl;

    // 获取缓存块
    BufPageManager *b_manager = new BufPageManager(f_manager);
    int buf_index = 0;
    BufType buf = b_manager->getPage(file_ID, pageID, buf_index);

    // 解析并更新索引页头（记录条数，第一个空闲记录的起始地址，null位图）
    Index_Header_info* ih = (Index_Header_info*)buf;
    ih->slotCnt = pkus.size();
    ih->freeData = sizeof(Index_Header_info) + pkus.size() * (*pkus.begin())->record_len;
    ih->nullmap = size_to_mask(pkus.size());

    // 将索引页头以及数据写入页中
    memcpy(buf, (uint8_t *)ih, sizeof(Index_Header_info));
    int offset = 0;
    std::cout << "size = " << pkus.size() << std::endl;
    for (auto it = pkus.begin(); it != pkus.end(); it++) {
        // TODO：是否应当把BufType改为以byte为单位
        std::cout << "(*it)->record_len " << (*it)->record_len << std::endl;
        memcpy(&buf[sizeof(Index_Header_info) + offset * (*it)->record_len], (*it)->record, (*it)->record_len);
        offset++;
    }
    b_manager->markDirty(buf_index);
    b_manager->close();

    f_manager->closeFile(file_ID);
    dirty = false;
}

/**
 * @brief 更新非页节点的页面
 * 
 */
void BplusNode::pk_write_back() {
    // 通过文件管理器打开当前节点所对应的页
    FileManager *f_manager = new FileManager();
    int file_ID = 0;
    string str_filename = "Index" + std::to_string(objID) + "_" + std::to_string(pageID);
    const char* filename = str_filename.data();
    f_manager->openFile(filename, file_ID);

    // 对页寻找缓存单元
    BufPageManager *b_manager = new BufPageManager(f_manager);
    int buf_index = 0;
    BufType buf = b_manager->getPage(file_ID, pageID, buf_index);

    // 更新索引页头
    Index_Header_info* ih = (Index_Header_info*)buf;
    ih->slotCnt = pkus.size();
    ih->freeData = sizeof(Index_Header_info) + pkus.size() * sizeof(P_K);
    ih->nullmap = size_to_mask(pkus.size());

    // 将索引页头和列表中的p_k对写入页面
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

    dirty = false;
}

/**
 * @brief 在页分裂时分配新的索引页
 * 
 * @param record_size 一条记录的长度 
 */
void BplusNode::alloc_new_page(int record_size, bool leafNode) {
    // 使用文件管理器创建新的索引页
    std::cout << "new_page allocating..." << std::endl;
    std::cout << "record_size = " << record_size << " leafNode = " << leafNode << std::endl;
    
    FileManager *f_manager = new FileManager();
    int file_ID = 0;
    string str_filename = "Index" + std::to_string(objID) + "_" + std::to_string(pageID);
    const char* filename = str_filename.data();
    f_manager->createFile(filename);
    f_manager->openFile(filename, file_ID);

    // 获取新页面的缓存块
    BufPageManager *b_manager = new BufPageManager(f_manager);
    int buf_index = 0;
    BufType buf = b_manager->allocPage(file_ID, pageID, buf_index);

    // 构建新的索引页头
    Header_manager h_manager;
    int next_pageID = 0;
    int prev_pageID = 0;
    if (leafNode == LEAF_NODE)
        h_manager.header_initialize(pageID, next_pageID, prev_pageID, objID, LEAF_NODE, INDEX_DATA_PAGE, record_size);
    else    
        h_manager.header_initialize(pageID, next_pageID, prev_pageID, objID, 0, INDEX_DATA_PAGE, record_size);
    Index_Header_info header = h_manager.get_header_info();
    memcpy(buf, (uint8_t *)(&header), sizeof(Index_Header_info));
    b_manager->markDirty(buf_index);
    b_manager->close();

    f_manager->closeFile(file_ID);
}

/**
 * @brief 当页中的记录条数过大时，分裂页表并创建新的页
 * 
 * @param is_leaf_node 
 * @param max_page_ID 
 * @return int 
 */
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
    BplusNode new_node(new_pkus, is_leaf_node, max_page_ID, objID, next_page_ID);
    // 建立node间的连接关系
    next_page_ID = max_page_ID;
    new_node.father_node = this->father_node;

    // 为分裂节点分配新页面并写入
    if (is_leaf_node == LEAF_NODE) {
        new_node.alloc_new_page((*new_pkus.begin())->record_len, LEAF_NODE);
        new_node.data_write_back();
    } else {
        new_node.alloc_new_page(sizeof(P_K), false);
        new_node.pk_write_back();     
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
    std::cout << "BplusNode insert PKUnit... len = " << i_pku->record_len << std::endl;
    bool has_insert = false;
    if (leaf == LEAF_NODE) {
        std::cout << "Leaf node : origin size = " << pkus.size() << std::endl;
        for (auto it = pkus.begin(); it != pkus.end(); it++) {
            if (i_pku->k < (*it)->k) {
                std::cout << "1Insert key = " << i_pku->k << " len = " << i_pku->record_len << std::endl;
                pkus.insert(it, i_pku);
                has_insert = true;
                break;
            }
        }
        if (has_insert == false) {
            std::cout << "2Insert key = " << i_pku->k << std::endl;
            pkus.push_back(i_pku);
        }
        dirty = true;

        if (sizeof(Index_Header_info) + i_pku->record_len * pkus.size() > PAGE_SIZE >> 2){
            // 页分裂
            int next_key = spilt_page(LEAF_NODE, max_pageID);
            max_pageID++;

            if (root == true) {
            // 对于根节点分裂还应该建立一个新的根节点
                root = false;  
                std::list<PKUnit*> new_pkus;
                PKUnit* n_pku = new PKUnit(max_pageID, next_key, i_pku->record_len);
                new_pkus.push_back(n_pku);

                BplusNode new_node(new_pkus, 0, max_pageID, objID, 0);
                new_node.alloc_new_page(sizeof(P_K), LEAF_NODE);
                new_node.data_write_back();

                max_pageID++;
            }
            return true;
        }
        else { // 不需分页，将更改后的页面信息写入页中
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
                child_node = get_bplus_node((*it)->p, false);
                child_nodes.push_back(child_node);
                child_node->father_node = this;

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
            int new_key = (*child_node->pkus.begin())->k;
            (*iterator)->k = new_key;
            // 在当前p,k对之后插入新的p,k对，其p值为分裂第二页的页编号，k值为当前p,k对的原k值
            int new_p = next_page_ID;
            int len = i_pku->record_len;
            PKUnit* new_pku = new PKUnit(new_p, old_key, len);
            // 在当前PKUnit之后插入新的PKUnit
            iterator++;
            pkus.insert(iterator, new_pku);
            dirty = true;
        }

        // 中间节点分裂
        if (sizeof(Index_Header_info) + 8 * pkus.size() > PAGE_SIZE >> 2) {
            int k = spilt_page(1, max_pageID);
            max_pageID ++;

        // 对于根节点分裂还应该建立一个新的根节点
            root = false;  
            int next_key = INT_MAX;
            std::list<PKUnit*> new_pkus;
            PKUnit* n_pku = new PKUnit(max_pageID-1, next_key, i_pku->record_len);
            new_pkus.push_back(n_pku);

            BplusNode new_node(new_pkus, 0, max_pageID, objID, 0);
            new_node.alloc_new_page(sizeof(P_K), false);
            new_node.data_write_back();

            max_pageID++;
            return true;
        } else {
        // 中间节点不分裂
            return false;
        }
    }
}

/**
 * @brief 读取子节点页，将子节点页的信息加载到ChlidNode中
 * 
 * @param index 子节点页号p
 * @return BplusNode* 子节点
 */
BplusNode* BplusNode::get_bplus_node(int index, bool root) {
    // 使用文件管理器打开子节点页面
    std::cout << "get_bplus_node " << index << "..." << std::endl;
    FileManager *f_manager = new FileManager();
    int file_ID = 0;
    string str_filename = "index" + std::to_string(objID) + "_" + std::to_string(index);
    const char* filename = str_filename.data();
    f_manager->openFile(filename, file_ID);

    std::cout << filename << std::endl;

    // 读取文件内容载入到buffer中
    BufPageManager *b_manager = new BufPageManager(f_manager);
    int buf_index = 0;
    BufType buf = b_manager->getPage(file_ID, index, buf_index);
    std::cout << "get buf" << std::endl;

    // 获取索引头
    Index_Header_info* ih = (Index_Header_info*)buf;
    int slotCnt = ih->slotCnt;
    int level = ih->level;
    int len = ih->pminlen;
    int nextPage = ih->nextPage;

    std::cout << "slotCnt = " << slotCnt << " level = " << level << " len = " << len << " nextPage = " << nextPage << std::endl;

    // 读取索引页中的所有数据
    std::list<PKUnit *> child_record;
    for (int i = 0; i < slotCnt; i++) {
        PKUnit *pku = new PKUnit(index, 0, len);
        if (level == 0) {
            // 读取叶节点的数据存入pku的record数组中
            memcpy(pku->record, (uint8_t *)&buf[sizeof(Index_Header_info) + len*i], len);
            int *key = (int *)&buf[sizeof(Index_Header_info) + len*i];
            pku->k = (*key);
            child_record.push_back(pku);
        } else {
            P_K* child_pk = (P_K *)&buf[sizeof(Index_Header_info) + sizeof(P_K)*i];
            pku->p = child_pk->p;
            pku->k = child_pk->k;
            child_record.push_back(pku);
        }
    }

    bool leaf = 0;
    if (level == 0) leaf = LEAF_NODE;
    else leaf = false;
    BplusNode* child_node = new BplusNode(child_record, leaf, index, objID, nextPage);

    b_manager->close();
    f_manager->closeFile(file_ID);
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
bool BplusNode::get_record(int k, uint8_t* record) {
    if (leaf == LEAF_NODE) {
        std::cout << "get_record LEAF NODE " << pkus.size() << std::endl;
        for (auto it = pkus.begin(); it != pkus.end(); it++) {
            std::cout << (*it)->k << " ";
            if ((*it)->k == k) {
                std::cout << "get!" << std::endl;
                memcpy(record, (*it)->record, (*it)->record_len);
                return true;
            }
        }
        std::cout << "not found!" << std::endl;
        return false;
    } else {
        for (auto it = pkus.begin(); it != pkus.end(); it++) {
            if ((*it)->k >= k) {
                return get_bplus_node((*it)->p, false)->get_record(k, record);
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
    return leaf;
}

bool BplusNode::isDirty() {
    return dirty;
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
 * @brief 创建新的B+树
 * 
 * @param objID B+树所属的表ID
 */
void BplusTree::create_new_tree(int objID, int len) {
    std::list<PKUnit*> root_list;
    root = new BplusNode(root_list, LEAF_NODE, 0, objID, 0);
    root->alloc_new_page(len, LEAF_NODE);
}

/**
 * @brief 获取特定主索引的B+树根
 * 
 * @param objID B+树所属的表ID
 */
void BplusTree::initial_tree_root(int objID) {
    std::cout << "get_tree_root..." << std::endl;
    std::list<PKUnit*> root_list;
    root = new BplusNode(root_list, false, 0, objID, 0); 
    root = root->get_bplus_node(0, true);  
}

/**
 * @brief 将数据转换成PKUnit
 * 
 * @param record 数据
 * @param len 数据大小（以byte为单位）
 * @return PKUnit* 
 */
PKUnit* BplusTree::record_to_PKUnit(uint8_t* record, int len) {
    std::cout << "Record_to_PKUnit... len = " << len << std::endl;
    PKUnit* pku = new PKUnit();
    pku->record_len = len;
    int* key = (int *)record;
    pku->k = *key;
    memcpy(pku->record, record, len);

    return pku;
}

/**
 * @brief 将数据插入B+树
 * 
 * @param record 
 * @param len 
 */
void BplusTree::insert_record(uint8_t* record, int len) {
    std::cout << "BplusTree Insert record... len = " << len << std::endl;
    int max_pageID = 1;
    root->insert_PK_Unit(record_to_PKUnit(record, len), max_pageID);
}

bool BplusTree::get_record(int key, uint8_t* record) {
    return root->get_record(key, record);
}

void BplusTree::write_back(BplusNode *node) {
    if (node->isLeaf() == LEAF_NODE && node->isDirty() == true) {
        node->data_write_back();
        
    } else {
        for (auto it = node->child_nodes.begin(); it != node->child_nodes.end(); it++) {
            write_back(*it);
        }
        if (node->isDirty() == true)
            node->pk_write_back();
    }
}

BplusNode* BplusTree::get_root() {
    return root;
}