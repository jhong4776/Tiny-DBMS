#include "System_manager.h"
#include <string.h>
#include <stdint.h>

System_manager::System_manager() { 
    std::cout << "System_manager creating..." << std::endl;
    f_manager = new FileManager();
    bool new_file = f_manager->openFile("system", fileID);
    b_manager = new BufPageManager(f_manager);
    update = false;

    if (new_file == false){
        std::cout << "create system1" << std::endl;
        f_manager->createFile("system");
        new_file = f_manager->openFile("system", fileID);
        database_num = 0;

        buf = b_manager->allocPage(fileID, 0, bufID);
        update = true;
    }

    int* len = new int;
    if(new_file == true) {
        buf = b_manager->getPage(fileID, 0, bufID);
        memcpy(len, buf, sizeof(int));
        database_num = *len;
        std::cout << "len = " << database_num << " bufID = " << bufID << std::endl;

        for (int i = 0; i < database_num; i++) {
            char database_name[20];
            memcpy(database_name, &buf[sizeof(int) + i*20], 20);
            string name = database_name;
            databases.push_back(name);
        }
    }
    std::cout << "System_manager created!" << std::endl;
}

System_manager::~System_manager() {
    std::cout << "destorying System_manager" << std::endl;
    if (update == true) {
        memcpy(buf, (uint8_t *)&database_num, sizeof(int));
        int offset = 0;
        for (auto it = databases.begin(); it != databases.end(); it++) {
            const char* p = (*it).data();
            memcpy(&buf[sizeof(int) + offset*20], (uint8_t *)p, 20);
            offset ++;
        } 
        b_manager->markDirty(bufID);
    }
    b_manager->close();
    b_manager->~BufPageManager();
    f_manager->closeFile(fileID);
    f_manager->~FileManager(); 
}

void System_manager::create_database(std::string name) {
    std::cout << "create database" << std::endl;
    bool error = false;
    for (auto it = databases.begin(); it != databases.end(); it++) {
        if((*it) == name) {
            error = true;
            std::cout << "Can't create database " << name << " database exists" << std::endl;
            break;
        }
    }
    if (error == false) {
        databases.push_back(name);
        database_num++;
        f_manager->createFile(name.data());
    }
    // 初始化数据库文件的信息：设置数据表个数为0
    int b_id = 0;
    int f_id = 0;
    f_manager->openFile(name.data(), f_id);
    BufType buf_now = b_manager->getPage(f_id, 0, b_id);
    DBHeader new_header;
    new_header.newID = 0;
    new_header.size = 0;
    memcpy(buf, &new_header, sizeof(DBHeader));
    b_manager->markDirty(b_id);

    b_manager->writeBack(b_id);
    f_manager->closeFile(f_id);

    update = true;
}

void System_manager::drop_database(std::string name) {
    std::cout << "drop database" << std::endl;
    bool error = true;
    for (auto it = databases.begin(); it != databases.end(); it++) {
        if((*it) == name) {
            error = false;
            break;
        }
    }    
    if (error == true)
        std::cout << "Can't drop database " << name << " database doesn't exists" << std::endl;
    else {
        databases.remove(name);
        database_num--;
    }

    update = true;
}

void System_manager::use_database(std::string name) {
    bool error = true;
    for (auto it = databases.begin(); it != databases.end(); it++) {
        if((*it) == name) {
            error = false;
            database_now = name;
            break;
        }
    }   
    if (error == true) {
        std::cout << "Can't use database " << name << " database doesn't exists" << std::endl;
        return;
    }
    std::cout << "use " << database_now << std::endl;
}

void System_manager::create_table(std::string name) {
    std::cout << "create table " << name << std::endl;

    // 打开数据库文件
    int bufID_now = 0;
    int new_fileID = 0;
    f_manager->openFile(database_now.data(), new_fileID); 
    BufType table_buf = b_manager->getPage(new_fileID, 0, bufID_now);    

    // 解析数据库文件头
    DBHeader* new_header = (DBHeader*)table_buf;
    int table_num = new_header->size;
    std::cout << "len = " << new_header->size << " newID = " << new_header->newID << "bufID = " << bufID_now << std::endl;

    for (int i = 0; i < table_num; i++) {
        TableList* tablelist = (TableList*)&table_buf[sizeof(DBHeader) + i * sizeof(TableList)];
        string str_table_name = tablelist->table_name;

        if (name == str_table_name) {
            std::cout << "Can't create table " << name << " table exists" << std::endl;
            return;
        }
    }

    // 更新数据库文件头&插入新的表项
    TableList newtable;
    for (int i = 0; i < 20; i++) {
        newtable.table_name[i] = 0;
    }
    name.copy(newtable.table_name, 19, 0);
    newtable.table_index = new_header->newID;

    DBHeader newDBHeader;
    newDBHeader.newID = new_header->newID + 1;
    newDBHeader.size = new_header->size + 1;
    
    memcpy(table_buf, &newDBHeader, sizeof(DBHeader));
    memcpy(&table_buf[sizeof(DBHeader) + sizeof(TableList)*table_num], (uint8_t *)&newtable, sizeof(TableList));

    b_manager->markDirty(bufID_now);
    b_manager->writeBack(bufID_now);
    f_manager->closeFile(new_fileID);
}

void System_manager::drop_table(std::string name) {
    std::cout << "drop table " << name << std::endl;

    // 打开数据库文件
    int bufID_now = 0;
    int new_fileID = 0;
    f_manager->openFile(database_now.data(), new_fileID);
    BufType table_buf = b_manager->getPage(new_fileID, 0, bufID_now);

    // 解析数据库文件头
    DBHeader* new_header = (DBHeader*)table_buf;
    int table_num = new_header->size;
    std::cout << "len = " << new_header->size << " newID = " << new_header->newID << std::endl;

    bool table_exit = false;
    std::list<TableList> tables_in_DB;
    for (int i = 0; i < table_num; i++) {
        TableList newtable;
        memcpy(&newtable, &table_buf[sizeof(DBHeader) + i * sizeof(TableList)], sizeof(TableList));

        string t_name = newtable.table_name;
        if (t_name != name)
            tables_in_DB.push_back(newtable);
        else
            table_exit = true;
    }

    if (table_exit == false)
        std::cout << "Can't drop table " << name << " table doesn't exists" << std::endl;
    else {
        DBHeader newDBHeader;
        newDBHeader.newID = new_header->newID;
        newDBHeader.size = new_header->size - 1;
        memcpy(table_buf, &newDBHeader, sizeof(DBHeader));
        
        int off = 0;
        for (auto it = tables_in_DB.begin(); it != tables_in_DB.end(); it++) {
            memcpy(&table_buf[sizeof(DBHeader) + sizeof(TableList)* off], (uint8_t *)&(*it), sizeof(TableList));
        }

        b_manager->markDirty(bufID_now);
        b_manager->writeBack(bufID_now);
        f_manager->closeFile(new_fileID);        
    }
}

void System_manager::rename_table(std::string oldname, std::string newname) {
    // 打开数据库文件
    int bufID_now = 0;
    int new_fileID = 0;
    f_manager->openFile(database_now.data(), new_fileID);
    BufType table_buf = b_manager->getPage(new_fileID, 0, bufID_now);

    // 解析数据库文件头
    DBHeader* new_header = (DBHeader*)table_buf;
    int table_num = new_header->size;
    std::cout << "len = " << new_header->size << " newID = " << new_header->newID << std::endl;

    int index = -1;
    TableList R_table;
    for (int i = 0; i < table_num; i++) {
        TableList newtable;
        memcpy(&newtable, &table_buf[sizeof(DBHeader) + i * sizeof(TableList)], sizeof(TableList));

        string t_name = newtable.table_name;
        if (t_name == oldname){
            for (int i = 0; i < 20; i++)
                newtable.table_name[i] = 0;
            newname.copy(newtable.table_name, 20, 0);
            memcpy(&table_buf[sizeof(DBHeader) + i * sizeof(TableList)], (uint8_t *)&newtable, sizeof(TableList));
            index = i;
        }
    }    

    if (index == -1) {
        std::cout << "Can't rename table " << oldname << " table doesn't exists" << std::endl;
    } else {
        b_manager->markDirty(bufID_now);
        b_manager->writeBack(bufID_now);
        f_manager->closeFile(new_fileID);        
    }
}

std::list<std::string>* System_manager::show_table() {
    std::list<std::string>* table_names = new std::list<std::string>;
    std::cout << "show tables in " << database_now << std::endl;

    // 打开数据库文件
    int bufID_now = 0;
    int new_fileID = 0;
    f_manager->openFile(database_now.data(), new_fileID);
    BufType table_buf = b_manager->getPage(new_fileID, 0, bufID_now);

    // 解析数据库文件头
    DBHeader* new_header = (DBHeader*)table_buf;
    int table_num = new_header->size;
    std::cout << "len = " << new_header->size << " newID = " << new_header->newID << std::endl;    

    for (int i = 0; i < table_num; i++) {
        TableList* table = (TableList *)&table_buf[sizeof(DBHeader) + sizeof(TableList) * i];
        string name = table->table_name;
        table_names->push_back(name);
        std::cout << name << " ";
    }
    std::cout << std::endl;
    return table_names;
}

std::list<std::string>* System_manager::show_database() {
    std::list<std::string>* database_names = new std::list<std::string>;
    for (auto it = databases.begin(); it != databases.end(); it++) {
        database_names->push_back((*it));
    }
    return database_names;
}