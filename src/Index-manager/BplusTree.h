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
    int record = 0; // rid：记录在页式文件中的位置

    PKUnit(){};
    PKUnit(int p1, int k1){
        p = p1;
        k = k1;
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
    std::list<PKUnit> pkus;
    BplusNode();
    void node_initial(std::list<PKUnit> pkus1, bool leaf1, int pageID1, int objID1, int next_page_id1);
    ~BplusNode();
    
    void data_write_back(BufPageManager* b_manager, int fileID);
    void pk_write_back(BufPageManager* b_manager, int fileID);
    void alloc_new_page(bool leafNode, BufPageManager* b_manager, int fileID);

    int spilt_page(int is_leaf_node, int max_page_ID, BufPageManager* b_manager, int fileID);

    int insert_PK_Unit(PKUnit i_pku, int& max_pageID, BufPageManager* b_manager, int fileID);
    bool delete_PK_Unit(PKUnit d_pku, int& max_pageID, BufPageManager* b_manager, int fileID);
    int get_record(int k, BufPageManager* b_manager, int fileID);

    bool isLeaf();
    void setLeaf(bool l);
    bool isDirty();
    void setRoot(bool isroot);
    BplusNode* get_bplus_node(int index, BufPageManager* b_manager, int fileID);
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

protected:
    void get_maxpageID();
    void update_maxpageID();

public:
    BplusTree();
    ~BplusTree();
    int get_size();
    void write_back();
    void create_new_tree(int objID, std::string database, std::string index_name);
    void initial_tree_root(int objID, std::string database, std::string index_name);

    void insert_record(int k, int record);
    void delete_record(int k);
    int get_record(int key);
    BplusNode* get_root();

    void root_debug();
};

#endif