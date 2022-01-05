#ifndef DATABASEM
#define DATABASEM

#include <iostream>
#include <list>
#include <map>
#include <string>
#include "../FS-manager/fileio/FileManager.h"
#include "../FS-manager/bufmanager/BufPageManager.h"

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
public:
    System_manager();
    ~System_manager();
    void create_database(std::string name);
    void drop_database(std::string name);
    void use_database(std::string name);
    void create_table(std::string name);
    void drop_table(std::string name);
    void rename_table(std::string oldname, std::string newname);
    std::list<std::string>* show_table();
    std::list<std::string>* show_database();
};


#endif