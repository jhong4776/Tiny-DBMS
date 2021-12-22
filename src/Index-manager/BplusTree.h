#ifndef BPLUSTREE
#define BPLUSTREE

#include <list>
#include <stdint.h>

/**
 * @brief p,k键值对,若节点为叶节点则需记录数据
 * 
 */
class PKUnit{ 
public:
    int p; // 页号索引
    int k;   // key值
    uint8_t record[1024];
    int record_len;

    PKUnit(){};
    PKUnit(int p1, int k1, int record_len1){
        p = p1;
        k = k1;
        record_len = record_len1;
    };
    ~PKUnit(){};
};

class BplusNode{
    std::list<PKUnit* > pkus;
    bool leaf;
    bool root;
    bool dirty;
    int pageID;
    int objID;

    int next_page_ID;
    int height;
public:
    BplusNode();
    BplusNode(std::list<PKUnit*> pkus1, bool leaf1, int pageID1, int objID1, int next_page_id1);
    ~BplusNode();

    int size_to_mask(int size);
    int mask_to_size(int mask);
    
    void data_write_back();
    void pk_write_back();
    void alloc_new_page(int page_size, bool leafNode);

    int spilt_page(int is_leaf_node, int max_page_ID);

    bool insert_PK_Unit(PKUnit* i_pku, int& max_pageID);
    bool delete_PK_Unit(PKUnit* d_pku, int& max_pageID);
    bool get_record(int k, uint8_t* record);

    bool isLeaf();
    bool isDirty();
    BplusNode* get_bplus_node(int index, bool root);
    int get_size();
    // void get_node_info(int fileID, int pageID);

    std::list<BplusNode* > child_nodes;
    BplusNode* father_node;
};

class BplusTree {
    BplusNode* root;
    int size;
    int max_pageID;
    std::list<int> dirty_page;
public:
    BplusTree();
    ~BplusTree();
    int get_size();
    void write_back(BplusNode* node);
    void create_new_tree(int objID, int len);
    void initial_tree_root(int objID);

    PKUnit* record_to_PKUnit(uint8_t* record, int len);

    void insert_record(uint8_t* record, int len);
    bool get_record(int key, uint8_t* record);
    BplusNode* get_root();
};

#endif