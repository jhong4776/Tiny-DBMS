#ifndef BPLUSTREE
#define BPLUSTREE

#include <vector>
#include <list>
#include "../bufmanager/BufPageManager.h"
#include "../Data_define.h"

/**
 * @brief p,k键值对,若节点为叶节点则需记录数据
 * 
 */
class PKUnit{ 
public:
    int p; // 页号索引
    int k;   // key值
    uint8_t record[MAX_RECORD];
    int record_len;

    PKUnit(){}
    PKUnit(int p1, int k1, int record_len1){
        p = p1;
        k = k1;
        record_len = record_len1;
    }
    ~PKUnit(){}
};

class BplusNode{
    std::list<PKUnit* > pkus;
    bool leaf;
    bool root;
    int pageID;
    int objID;

    int next_page_ID;
public:
    BplusNode();
    BplusNode(std::list<PKUnit*> pkus1, bool leaf1, int pageID1, int objID1);
    ~BplusNode();

    int size_to_mask(int size);
    int mask_to_size(int mask);
    
    void data_write_back();
    void pk_write_back();
    void alloc_new_page(int page_size);

    int spilt_page(int is_leaf_node, int max_page_ID);

    bool insert_PK_Unit(PKUnit* i_pku, int& max_pageID);
    bool delete_PK_Unit(PKUnit* d_pku, int& max_pageID);

    bool isLeaf();
    BplusNode* get_bplus_node(int index);
    int get_size();
    void get_node_info(int fileID, int pageID);

    BplusNode* father;
    BplusNode* left;
    BplusNode* right;
};

class BplusTree {
    BplusNode* root;
    int size;
    int max_pageID;
public:
    BplusTree();
    ~BplusTree();
    int get_size();

};

#endif