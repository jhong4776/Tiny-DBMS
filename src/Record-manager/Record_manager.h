#ifndef R_M
#define R_M

#include "../FS-manager/fileio/FileManager.h"
#include "../FS-manager/bufmanager/BufPageManager.h"
#include <iostream>
#include <string>
#include "../Header_define.h"
#include "../System-manager/System_manager.h"

#define LIVE 0
#define DEAD 1

class Record_manager {
    FileManager* f_manager;
    BufPageManager* b_manager;
    int fileID;
    std::string db_now;
    int table_ID;
public:
    Record_manager();
    ~Record_manager();
    int createFile(std::string database_name, int tableID);
    int destroyFile(std::string file_name);
    int openFile(std::string database_name, int tableID);
    int closeFile();
    int getRecord(int rid, char* output);
    int insertRecord(const char* input);
    int deleteRecord(int rid);
    int updateRecord(int rid, const char* input);
    int schema();
    bool find_record(int pageID, int offset, int num);
    bool find_record(int pageID, int offset, float num);
    bool find_record(int pageID, int offset, std::string name, int size);
    void mark_record(int pageID, int offset, int num);
    void mark_record(int pageID, int offset, float num);
    void mark_record(int pageID, int offset, std::string name, int size);  
    void select_all(std::string database_name, int tableID);
    void select_equa(std::string database_name, int tableID, int num, std::string colunm);
    void select_equa(std::string database_name, int tableID, std::string num, std::string colunm);
    void select_equa(std::string database_name, int tableID, float num, std::string colunm);
    bool check_dup(int pageID, int offset, int type, int len, std::string colunm);
    int get_tag(int rid);
    Record_Header* get_header(std::string database_name, int tableID);
};

#endif