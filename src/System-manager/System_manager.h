#ifndef DATABASEM
#define DATABASEM

#include <iostream>
#include <list>
#include <vector>
#include <map>
#include <string>
#include "../FS-manager/fileio/FileManager.h"
#include "../FS-manager/bufmanager/BufPageManager.h"
#include "../Header_define.h"

struct DBHeader{
    int size;
    int newID;
};

struct TableList {
    int table_index;
    char table_name[20];
};

class System_manager
{
private:
    std::list<std::string> databases;

    std::string database_now;
    FileManager* f_manager;
    BufPageManager* b_manager;
    BufType buf;
    
    int fileID;
    int bufID;

    int database_num;
    bool update;
protected:
    bool insert_foreign_key(std::string forname, int forpage, std::string priname, int pripage);
    bool drop_foreign_key(std::string forname, int forpage, std::string priname, int pripage);
    bool record_dup();
public:
    System_manager();
    ~System_manager();
    void create_database(std::string name);
    void drop_database(std::string name);
    void use_database(std::string name);
    void create_table(std::string name, Table_Header table_header, std::list<Property> property, std::list<PriKey> prikey, std::list<ForKey> forkey);
    void drop_table(std::string name);
    void rename_table(std::string oldname, std::string newname);
    std::list<std::string>* show_table();
    std::list<std::string>* show_database();
    void table_schema(std::string table_name);
    int get_table_ID(int fID, std::string table_name);
    std::string get_table_name(int fID, int pageID);
    void add_prikey(std::string table_name, std::string prikey);
    void drop_prikey(std::string table_name, std::string prikey);
    void add_foreignkey(std::string table_name, std::string forkey, std::string r_table, std::string prikey);
    void drop_foreignkey(std::string table_name, std::string forkey);
    void add_index(std::string table_name, std::string index_name);
    void drop_index(std::string table_name, std::string index_name);
    void update_pagenum(int tableID);
    int get_recordLen(int tableID);
    Table_Header* get_tableHeader(int tableID);
    std::string get_database_now();
    std::vector<Property> get_property_vector(int tableID);
    std::vector<Index> get_index_vector(int tableID);
    std::vector<PriKey> get_pri_vector(int tableID);
    std::vector<ForKey> get_for_vector(int tableID);
};


#endif