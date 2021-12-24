#ifndef BPLUSTREE
#define BPLUSTREE

#include <list>
#include <stdint.h>
#include <iostream>
#include <map>
#include "../FS-manager/bufmanager/BufPageManager.h"

/**
 * @brief p,k键值对,若节点为叶节点则需记录数据
 * 
 */
class PKUnit{ 
public:
    int p = 0; // 页号索引
    int k = 0;   // key值
    int index = 0;
    uint8_t record[1024];
    int record_len = 0;

    PKUnit(){};
    PKUnit(int p1, int k1, int record_len1){
        p = p1;
        k = k1;
        record_len = record_len1;
    };
    ~PKUnit(){};
};

class BplusNode{
    bool leaf;
    bool root;
    bool dirty;
    int pageID;
    int objID;

    int next_page_ID;
    int height;
    std::map<int, BplusNode*> child_map; 
public:
    std::list<int> pkus;
    BplusNode();
    void node_initial(std::list<int> pkus1, bool leaf1, int pageID1, int objID1, int next_page_id1);
    ~BplusNode();

    int size_to_mask(int size);
    int mask_to_size(int mask);
    
    void data_write_back(BufPageManager* b_manager, int fileID, int& n_offset, PKUnit* pku_block);
    void pk_write_back(BufPageManager* b_manager, int fileID, int& n_offset, PKUnit* pku_block);
    void alloc_new_page(int page_size, bool leafNode, BufPageManager* b_manager, int fileID, int& n_offset);

    int spilt_page(int is_leaf_node, int max_page_ID, BufPageManager* b_manager, int fileID, int& n_offset, PKUnit* pku_block);

    bool insert_PK_Unit(PKUnit i_pku, int& max_pageID, BufPageManager* b_manager, int fileID, int& n_offset, PKUnit* pku_block);
    bool delete_PK_Unit(PKUnit d_pku, int& max_pageID);
    bool get_record(int k, uint8_t* record, BufPageManager* b_manager, int fileID, int& p_offset, PKUnit* pku_block);

    bool isLeaf();
    void setLeaf(bool l);
    bool isDirty();
    void setRoot(bool isroot);
    BplusNode* get_bplus_node(int index, BufPageManager* b_manager, int fileID, int& n_offset, PKUnit* pku_block);
    int get_size();
    // void get_node_info(int fileID, int pageID);
};

class BplusTree {
    BplusNode* root;
    int size;
    int max_pageID;
    std::list<int> dirty_page;

    FileManager* f_manager;
    BufPageManager* b_manager;
    int fileID;
    int p_offset;

    PKUnit pku_block[1000];
public:
    BplusTree();
    ~BplusTree();
    int get_size();
    void write_back();
    void create_new_tree(int objID, int len, int& p_offset);
    void initial_tree_root(int objID, int& p_offset);

    void insert_record(uint8_t* record, int len);
    bool get_record(int key, uint8_t* record);
    BplusNode* get_root();

    void root_debug();
};

#endif