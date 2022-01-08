#include "System_manager.h"
#include <string.h>
#include <stdint.h>

System_manager::System_manager() {
    f_manager = new FileManager();
    bool new_file = f_manager->openFile("system", fileID);
    b_manager = new BufPageManager(f_manager);
    update = false;

    if (new_file == false){
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

        for (int i = 0; i < database_num; i++) {
            char database_name[20];
            memcpy(database_name, &buf[sizeof(int) + i*20], 20);
            string name = database_name;
            databases.push_back(name);
        }
    } 
}

System_manager::~System_manager() {
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
    bool error = false;
    for (auto it = databases.begin(); it != databases.end(); it++) {
        if((*it) == name) {
            error = true;
            std::cout << "Can't create database " << name << " database exists" << std::endl;
            return;
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
    new_header.newID = 1; 
    new_header.size = 0;
    memcpy(buf_now, &new_header, sizeof(DBHeader));
    b_manager->markDirty(b_id);

    b_manager->writeBack(b_id);
    f_manager->closeFile(f_id);

    update = true;
}

void System_manager::drop_database(std::string name) {
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
}

void System_manager::create_table(std::string name, Table_Header table_header, std::list<Property> property, std::list<PriKey> prikey, std::list<ForKey> forkey) {
    std::cout << "create table " << name << std::endl;

    // 打开数据库文件
    int bufID_now = 0;
    int new_fileID = 0;
    f_manager->openFile(database_now.data(), new_fileID); 
    BufType table_buf = b_manager->getPage(new_fileID, 0, bufID_now);    

    // 解析数据库文件头
    DBHeader* new_header = (DBHeader*)table_buf;
    int table_num = new_header->size;

    std::list<std::string> all_table;
    for (int i = 0; i < table_num; i++) {
        TableList* tablelist = (TableList*)&table_buf[sizeof(DBHeader) + i * sizeof(TableList)];
        all_table.push_back(tablelist->table_name);
        string str_table_name = tablelist->table_name;

        if (name == str_table_name) {
            std::cout << "Can't create table " << name << " table exists" << std::endl;
            return;
        }
    }
    std::cout << "1\n";
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
    std::cout << "2\n";
    // 在数据库页式文件中创建新的数据表页，页编号即为表编号
    int table_bufID = 0;
    BufType c_table_buf = b_manager->getPage(new_fileID, newtable.table_index, table_bufID);
    memcpy(c_table_buf, &table_header, sizeof(Table_Header));
    int off = 0;
    for (auto it = property.begin(); it != property.end(); it++) {
        memcpy(&c_table_buf[sizeof(Table_Header) + sizeof(Property) * off], &(*it), sizeof(Property));
        off++;
    }
    off = 0;
    for (auto it = prikey.begin(); it != prikey.end(); it++) {
        memcpy(&c_table_buf[sizeof(Table_Header) + sizeof(Property) * property.size() + sizeof(PriKey) * off], &(*it), sizeof(PriKey));
        off++;
    }
    off = 0;
    for (auto it = forkey.begin(); it != forkey.end(); it++) {
        if ((*it).pretable == -1) {
            std::cout << "Error: no such primary table" << std::endl;
            b_manager->release(table_bufID);
            b_manager->release(bufID_now);
            f_manager->closeFile(new_fileID);      
            return;      
        }
        bool pri_id_exit = insert_foreign_key((*it).name, newtable.table_index, (*it).prename, (*it).pretable);
        if (pri_id_exit == false) {
            std::cout << "Error: no such primary key" << std::endl;
            b_manager->release(table_bufID);
            b_manager->release(bufID_now);
            f_manager->closeFile(new_fileID);      
            return;            
        }
        memcpy(&c_table_buf[sizeof(Table_Header) + sizeof(Property) * property.size() + sizeof(PriKey) * prikey.size() + sizeof(ForKey) * off], &(*it), sizeof(ForKey));
        off++;        
    }
    b_manager->markDirty(table_bufID);
    b_manager->markDirty(bufID_now);

    b_manager->writeBack(table_bufID);
    b_manager->writeBack(bufID_now);
    f_manager->closeFile(new_fileID);
}

void System_manager::drop_table(std::string name) {

    // 打开数据库文件
    int bufID_now = 0;
    int new_fileID = 0;
    f_manager->openFile(database_now.data(), new_fileID);
    BufType table_buf = b_manager->getPage(new_fileID, 0, bufID_now);

    // 解析数据库文件头
    DBHeader* new_header = (DBHeader*)table_buf;
    int table_num = new_header->size;

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
            off++;
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

    // 打开数据库文件
    int bufID_now = 0;
    int new_fileID = 0;
    f_manager->openFile(database_now.data(), new_fileID);
    BufType table_buf = b_manager->getPage(new_fileID, 0, bufID_now);

    // 解析数据库文件头
    DBHeader* new_header = (DBHeader*)table_buf;
    int table_num = new_header->size;

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
        std::cout << (*it) << " ";
    }
    std::cout << std::endl;
    return database_names;
}

void System_manager::table_schema(std::string table_name) {
    // 打开数据库文件
    int bufID_now = 0;
    int new_fileID = 0;
    f_manager->openFile(database_now.data(), new_fileID); 
    // 获取该数据表所在页
    int pageID = get_table_ID(new_fileID, table_name);
    BufType table_buf = b_manager->getPage(new_fileID, pageID, bufID_now);   
    if (pageID == -1) {
        std::cout << "Error : table " << table_name << " doesn't exit" << std::endl;
        return;
    }
    // 解析表头
    Table_Header* t_h = (Table_Header *)table_buf;
    // 打印属性信息
    std::cout << "Properties: " << std::endl;
    for (int i = 0; i < t_h->pro_num; i++) {
        Property* pro = (Property *)&table_buf[sizeof(Table_Header) + i * sizeof(Property)];
        std::string pro_name = pro->name;
        std::cout << "name = " << pro_name << " len = " << pro->len << " type = " << pro->type << 
                     " be null = " << pro->be_null << " primarykey = " << pro->prikey << " foreignkey = " << pro->forkey << std::endl;
    }
    // 打印主键信息
    std::cout << "Primary keys: " << std::endl;
    for (int i = 0; i < t_h->prikey_num; i++) {
        PriKey* pri = (PriKey *)&table_buf[sizeof(Table_Header) + t_h->pro_num * sizeof(Property) + i * sizeof(PriKey)];
        std::string pri_name = pri->name;
        std::cout << "name = " << pri_name;
        if (pri->fortable != -1) {
            std::string for_name = pri->forname;
            std::cout << " table_name = " << get_table_name(new_fileID, pri->fortable) << " forname = " << for_name;
        }
        std::cout << endl;
    }
    // 打印外键信息
    std::cout << "Foreign keys: " << std::endl;
    for (int i = 0; i < t_h->forkey_num; i++) {
        ForKey* foreign = (ForKey *)&table_buf[sizeof(Table_Header) + t_h->pro_num * sizeof(Property) + t_h->prikey_num * sizeof(PriKey) + i * sizeof(ForKey)];
        std::string for_name = foreign->name;
        std::string pri_name = foreign->prename;
        std::cout << "name = " << for_name << " table_name = " << get_table_name(new_fileID, foreign->pretable) << " priname = " << pri_name << std::endl;
    }
    // 打印索引信息
    std::cout << "Indexs: " << std::endl;
    for (int i = 0; i < t_h->index_num; i++) {
        Index* index = (Index *)&table_buf[sizeof(Table_Header) + t_h->pro_num * sizeof(Property) + t_h->prikey_num * sizeof(PriKey) + t_h->forkey_num * sizeof(ForKey) + i * sizeof(Index)];
        std::string index_name = index->name;
        std::cout << "Index_name = " << index_name << std::endl;
    }    
    b_manager->release(bufID_now);
    f_manager->closeFile(new_fileID);
}

int System_manager::get_table_ID(int fID, std::string table_name) {
    // 打开数据库文件
    int bufID_now = 0;
    if (fID == -1)
        f_manager->openFile(database_now.data(), fID); 
    BufType table_buf = b_manager->getPage(fID, 0, bufID_now);    

    // 解析数据库文件头
    DBHeader* new_header = (DBHeader*)table_buf;
    int table_num = new_header->size;

    for (int i = 0; i < table_num; i++) {
        TableList* tablelist = (TableList*)&table_buf[sizeof(DBHeader) + i * sizeof(TableList)];
        string str_table_name = tablelist->table_name;

        if (table_name == str_table_name) {
            b_manager->release(bufID_now);
            return tablelist->table_index;
        }
    }   

    b_manager->release(bufID_now);
    return -1; 
}

std::string System_manager::get_table_name(int fID, int pageID) {
    // 打开数据库文件
    int bufID_now = 0;
    BufType table_buf = b_manager->getPage(fID, 0, bufID_now);    

    // 解析数据库文件头
    DBHeader* new_header = (DBHeader*)table_buf;
    int table_num = new_header->size;

    for (int i = 0; i < table_num; i++) {
        TableList* tablelist = (TableList*)&table_buf[sizeof(DBHeader) + i * sizeof(TableList)];

        if (pageID == tablelist->table_index) {
            string str_table_name = tablelist->table_name;
            b_manager->release(bufID_now);
            return str_table_name;
        }
    }   

    b_manager->release(bufID_now);
    return ""; 
}

bool System_manager::insert_foreign_key(std::string forname, int forpage, std::string priname, int pripage) {
    // 打开数据库文件
    int bufID_now = 0;
    int new_fileID = 0;
    f_manager->openFile(database_now.data(), new_fileID);
    BufType table_buf = b_manager->getPage(new_fileID, pripage, bufID_now);

    // 解析数据表头
    Table_Header* pri_table_h = (Table_Header *)table_buf;
    for (int i = 0; i < pri_table_h->prikey_num; i++) {
        PriKey* prikey = (PriKey *)&table_buf[sizeof(Table_Header) + sizeof(Property)  * pri_table_h->pro_num + sizeof(PriKey) * i];   
        string p_name = prikey->name;
        if (priname == p_name) {
            PriKey pri_update;
            for (int i = 0; i < 20; i++) {
                pri_update.forname[i] = 0;
                pri_update.name[i] = 0;
            }
            forname.copy(pri_update.forname, 20, 0); pri_update.fortable = forpage; priname.copy(pri_update.name, 20, 0);
            memcpy(&table_buf[sizeof(Table_Header) + sizeof(Property)  * pri_table_h->pro_num + sizeof(PriKey) * i], &pri_update, sizeof(PriKey));
            b_manager->markDirty(bufID_now);
            b_manager->writeBack(bufID_now);
            return true;
        }
    }
    return false;
}

bool System_manager::drop_foreign_key(std::string forname, int forpage, std::string priname, int pripage) {
    // 打开数据库文件
    int bufID_now = 0;
    int new_fileID = 0;
    f_manager->openFile(database_now.data(), new_fileID);
    BufType table_buf = b_manager->getPage(new_fileID, pripage, bufID_now);

    // 解析数据表头
    Table_Header* pri_table_h = (Table_Header *)table_buf;
    for (int i = 0; i < pri_table_h->prikey_num; i++) {
        PriKey* prikey = (PriKey *)&table_buf[sizeof(Table_Header) + sizeof(Property)  * pri_table_h->pro_num + sizeof(PriKey) * i];   
        string p_name = prikey->name;
        if (priname == p_name) {
            PriKey pri_update;
            for (int i = 0; i < 20; i++) {
                pri_update.forname[i] = 0;
                pri_update.name[i] = 0;
            }
            pri_update.fortable = -1;
            memcpy(&table_buf[sizeof(Table_Header) + sizeof(Property)  * pri_table_h->pro_num + sizeof(PriKey) + i], &pri_update, sizeof(PriKey));
            b_manager->markDirty(bufID_now);
            b_manager->writeBack(bufID_now);
            return true;
        }
    }
    return false;
}

void System_manager::add_prikey(std::string table_name, std::string prikey) {
    int new_fileID = 0;
    int new_bufID = 0;
    f_manager->openFile(database_now.data(), new_fileID);
    int pageID = get_table_ID(new_fileID, table_name);
    if (pageID == -1) {
        std::cout << "Error : table " << table_name << " doesn't exit!" << std::endl;
        f_manager->closeFile(new_fileID);
        return;
    }
    BufType table_buf = b_manager->getPage(new_fileID, pageID, new_bufID);

    // 解析表头
    Table_Header* t_h = (Table_Header *)table_buf;
    Table_Header new_table_header;
    int old_prikey_num = t_h->prikey_num;

    new_table_header.pro_num = t_h->pro_num; 
    new_table_header.prikey_num = t_h->prikey_num + 1; 
    new_table_header.forkey_num = t_h->forkey_num;
    new_table_header.index_num = t_h->index_num;
    memcpy(table_buf, &new_table_header, sizeof(Table_Header));

    // 查看对应列是否存在
    bool exist = false;
    for (int i = 0; i < t_h->pro_num; i++) {
        Property* pro = (Property *)&table_buf[sizeof(Table_Header) + sizeof(Property) * i];
        string ori_name = pro->name;
        if (ori_name == prikey) {
            if (pro->prikey == true) {
                std::cout << ori_name << " has been primary key!" << std::endl;
                b_manager->release(new_bufID);
                f_manager->closeFile(new_fileID);
                return;
            }
            pro->prikey = true;
            memcpy(&table_buf[sizeof(Table_Header) + sizeof(Property) * i], pro, sizeof(Property));
            exist = true;
            break;
        }
    }
    if (exist == false) {
        std::cout << "Error : " << prikey << " doesn't in table!" << std::endl;
        b_manager->release(new_bufID);
        f_manager->closeFile(new_fileID);
        return;
    }

    // 获取原主键、外键、索引
    std::list<PriKey> pri_list;
    std::list<ForKey> for_list;
    std::list<Index>  index_list;
    for (int i = 0; i < old_prikey_num; i++) {
        PriKey* pri = (PriKey *)&table_buf[sizeof(Table_Header) + sizeof(Property) * t_h->pro_num + sizeof(PriKey) * i];
        pri_list.push_back(*pri);
    }
    for (int i = 0; i < t_h->forkey_num; i++) {
        ForKey* forkey = (ForKey *)&table_buf[sizeof(Table_Header) + sizeof(Property) * t_h->pro_num + sizeof(PriKey) * old_prikey_num + sizeof(ForKey) * i];
        string f_n = forkey->name;
        for_list.push_back(*forkey);
    }   
    for (int i = 0; i < t_h->index_num; i++) {
        Index* index = (Index *)&table_buf[sizeof(Table_Header) + sizeof(Property) * t_h->pro_num + sizeof(PriKey) * old_prikey_num + sizeof(ForKey) * t_h->forkey_num + sizeof(Index) * i];
        string i_n = index->name;
        index_list.push_back(*index);
    }
    PriKey new_pri_key;
    for (int i = 0; i < 20; i++) {
        new_pri_key.name[i] = 0;
        new_pri_key.forname[i] = 0;
    }
    prikey.copy(new_pri_key.name, 20, 0);
    new_pri_key.fortable = -1;
    pri_list.push_back(new_pri_key);

    // 将主键、外键、索引信息重新写入页中
    int off = 0;
    for (auto it = index_list.begin(); it != index_list.end(); it++) {
        string i_n = (*it).name;
        memcpy(&table_buf[sizeof(Table_Header) + sizeof(Property) * t_h->pro_num + sizeof(PriKey) * new_table_header.prikey_num + sizeof(ForKey) * t_h->forkey_num + sizeof(Index) * off], &(*it), sizeof(Index));
        off ++;
    }   
    off = 0;
    for (auto it = for_list.begin(); it != for_list.end(); it++) {
        string f_n = (*it).name;
        memcpy(&table_buf[sizeof(Table_Header) + sizeof(Property) * t_h->pro_num + sizeof(PriKey) * new_table_header.prikey_num + sizeof(ForKey) * off], &(*it), sizeof(ForKey));
        off ++;
    }   
    off = 0; 
    for (auto it = pri_list.begin(); it != pri_list.end(); it++) {
        memcpy(&table_buf[sizeof(Table_Header) + sizeof(Property) * t_h->pro_num + sizeof(PriKey) * off], &(*it), sizeof(PriKey));
        off ++;
    }
    b_manager->markDirty(new_bufID);
    b_manager->writeBack(new_bufID);

    f_manager->closeFile(new_fileID);
}

void System_manager::drop_prikey(std::string table_name, std::string prikey) {
    int new_fileID = 0;
    int new_bufID = 0;
    f_manager->openFile(database_now.data(), new_fileID);
    int pageID = get_table_ID(new_fileID, table_name);
    if (pageID == -1) {
        std::cout << "Error : table " << table_name << " doesn't exit!" << std::endl;
        f_manager->closeFile(new_fileID);
        return;
    }
    BufType table_buf = b_manager->getPage(new_fileID, pageID, new_bufID);

    // 解析表头
    Table_Header* t_h = (Table_Header *)table_buf;
    Table_Header new_table_header;
    int old_prikey_num = t_h->prikey_num;

    new_table_header.pro_num = t_h->pro_num; 
    new_table_header.prikey_num = t_h->prikey_num - 1; 
    new_table_header.forkey_num = t_h->forkey_num;
    new_table_header.index_num = t_h->index_num;
    memcpy(table_buf, &new_table_header, sizeof(Table_Header));

    // 查看对应主键是否存在于元素中
    bool exist = false;
    for (int i = 0; i < t_h->pro_num; i++) {
        Property* pro = (Property *)&table_buf[sizeof(Table_Header) + sizeof(Property) * i];
        string ori_name = pro->name;
        if (ori_name == prikey) {
            if (pro->prikey == false) {
                std::cout << ori_name << " is not a primary key!" << std::endl;
                b_manager->release(new_bufID);
                f_manager->closeFile(new_fileID);
                return;
            }
            pro->prikey = false;
            memcpy(&table_buf[sizeof(Table_Header) + sizeof(Property) * i], pro, sizeof(Property));
            exist = true;
            break;
        }
    }
    if (exist == false) {
        std::cout << "Error : " << prikey << " doesn't in table!" << std::endl;
        b_manager->release(new_bufID);
        f_manager->closeFile(new_fileID);
        return;
    }

    // 获取原主键和外键
    std::list<PriKey> pri_list;
    std::list<ForKey> for_list;
    std::list<Index>  index_list;
    for (int i = 0; i < old_prikey_num; i++) {
        PriKey* pri = (PriKey *)&table_buf[sizeof(Table_Header) + sizeof(Property) * t_h->pro_num + sizeof(PriKey) * i];
        string ori_name = pri->name;
        if (prikey == ori_name) {
            if (pri->fortable != -1) {
                std::cout << "Error : constraint by foreign key" << std::endl;
                b_manager->release(new_bufID);
                f_manager->closeFile(new_fileID);
                return;
            }
        } else {
            pri_list.push_back(*pri);
        }
    }
    for (int i = 0; i < t_h->forkey_num; i++) {
        ForKey* forkey = (ForKey *)&table_buf[sizeof(Table_Header) + sizeof(Property) * t_h->pro_num + sizeof(PriKey) * old_prikey_num + sizeof(ForKey) * i];
        for_list.push_back(*forkey);
    }
    for (int i = 0; i < t_h->index_num; i++) {
        Index* index = (Index *)&table_buf[sizeof(Table_Header) + sizeof(Property) * t_h->pro_num + sizeof(PriKey) * old_prikey_num + sizeof(ForKey) * t_h->forkey_num + sizeof(Index) * i];
        string i_n = index->name;
        index_list.push_back(*index);
    }

    // 将主键和外键信息重新写入页中
    int off = 0;
    for (auto it = index_list.begin(); it != index_list.end(); it++) {
        string i_n = (*it).name;
        memcpy(&table_buf[sizeof(Table_Header) + sizeof(Property) * t_h->pro_num + sizeof(PriKey) * new_table_header.prikey_num + sizeof(ForKey) * t_h->forkey_num + sizeof(Index) * off], &(*it), sizeof(Index));
        off ++;
    }   
    off = 0;
    for (auto it = for_list.begin(); it != for_list.end(); it++) {
        memcpy(&table_buf[sizeof(Table_Header) + sizeof(Property) * t_h->pro_num + sizeof(PriKey) * t_h->prikey_num + sizeof(ForKey) * off], &(*it), sizeof(ForKey));
        off ++;
    } 
    off = 0; 
    for (auto it = pri_list.begin(); it != pri_list.end(); it++) {
        memcpy(&table_buf[sizeof(Table_Header) + sizeof(Property) * t_h->pro_num + sizeof(PriKey) * off], &(*it), sizeof(PriKey));
        off ++;
    }  
    b_manager->markDirty(new_bufID);
    b_manager->writeBack(new_bufID);

    f_manager->closeFile(new_fileID);
}

void System_manager::add_foreignkey(std::string table_name, std::string forkey, std::string r_table, std::string prikey) {
    int new_fileID = 0;
    int new_bufID = 0;
    f_manager->openFile(database_now.data(), new_fileID);
    int pageID = get_table_ID(new_fileID, table_name);
    if (pageID == -1) {
        std::cout << "Error : table " << table_name << " doesn't exit!" << std::endl;
        f_manager->closeFile(new_fileID);
        return;
    }

    int pripage = get_table_ID(new_fileID, r_table);
    if (pripage == - 1) {
        std::cout << "Error : Reference table " << r_table << " doesn't exit!" << std::endl;
        f_manager->closeFile(new_fileID);
        return;        
    }
    BufType table_buf = b_manager->getPage(new_fileID, pageID, new_bufID);    
    Table_Header* t_h = (Table_Header *)table_buf;
    Table_Header new_table_header;
    new_table_header.pro_num = t_h->pro_num; 
    new_table_header.prikey_num = t_h->prikey_num; 
    new_table_header.index_num = t_h->index_num;
    int old_forkey_num = t_h->forkey_num; 
    new_table_header.forkey_num = t_h->forkey_num + 1;
    memcpy(table_buf, &new_table_header, sizeof(Table_Header));    

    // 查看对应列是否存在
    bool exist = false;
    for (int i = 0; i < t_h->pro_num; i++) {
        Property* pro = (Property *)&table_buf[sizeof(Table_Header) + sizeof(Property) * i];
        string ori_name = pro->name;
        if (ori_name == forkey) {
            if (pro->forkey == true) {
                std::cout << ori_name << " has been formary key!" << std::endl;
                b_manager->release(new_bufID);
                f_manager->closeFile(new_fileID);
                return;
            }
            pro->forkey = true;
            memcpy(&table_buf[sizeof(Table_Header) + sizeof(Property) * i], pro, sizeof(Property));
            exist = true;
            break;
        }
    }
    if (exist == false) {
        std::cout << "Error : " << forkey << " doesn't in table!" << std::endl;
        b_manager->release(new_bufID);
        f_manager->closeFile(new_fileID);
        return;
    } 
    bool insert_suc = insert_foreign_key(forkey, pageID, prikey, pripage);
    if (insert_suc == false) {
        std::cout << "Error : Reference key " << prikey << " doesn't exit!" << std::endl;
        f_manager->closeFile(new_fileID);
        return;           
    }

    std::list<Index>  index_list;
    for (int i = 0; i < t_h->index_num; i++) {
        Index* index = (Index *)&table_buf[sizeof(Table_Header) + sizeof(Property) * t_h->pro_num + sizeof(PriKey) * t_h->prikey_num + sizeof(ForKey) * old_forkey_num + sizeof(Index) * i];
        string i_n = index->name;
        index_list.push_back(*index);
    }

    // 重新写入index
    int off = 0;
    for (auto it = index_list.begin(); it != index_list.end(); it++) {
        string i_n = (*it).name;
        memcpy(&table_buf[sizeof(Table_Header) + sizeof(Property) * t_h->pro_num + sizeof(PriKey) * t_h->prikey_num + sizeof(ForKey) * t_h->forkey_num + sizeof(Index) * off], &(*it), sizeof(Index));
        off ++;
    } 

    // 直接添加外键
    ForKey new_forkey;
    for (int i = 0; i < 20; i++) {
        new_forkey.name[i] = 0;
        new_forkey.prename[i] = 0;
    }
    forkey.copy(new_forkey.name, 20, 0); new_forkey.pretable = pripage; prikey.copy(new_forkey.prename, 20, 0);
    memcpy(&table_buf[sizeof(Table_Header) + sizeof(Property) * t_h->pro_num + sizeof(PriKey) * t_h->prikey_num + sizeof(ForKey) * (t_h->forkey_num - 1)],
            &new_forkey, sizeof(ForKey));

    b_manager->markDirty(new_bufID);
    b_manager->writeBack(new_bufID);
    f_manager->closeFile(new_fileID);
}

void System_manager::drop_foreignkey(std::string table_name, std::string forkey) {
    int new_fileID = 0;
    int new_bufID = 0;
    f_manager->openFile(database_now.data(), new_fileID);
    int pageID = get_table_ID(new_fileID, table_name);
    if (pageID == -1) {
        std::cout << "Error : table " << table_name << "doesn't exit!" << std::endl;
        f_manager->closeFile(new_fileID);
        return;
    }

    BufType table_buf = b_manager->getPage(new_fileID, pageID, new_bufID);    
    Table_Header* t_h = (Table_Header *)table_buf;
    Table_Header new_table_header;
    int old_forkey_num =  t_h->forkey_num;

    new_table_header.pro_num = t_h->pro_num; 
    new_table_header.prikey_num = t_h->prikey_num; 
    new_table_header.forkey_num = t_h->forkey_num - 1;
    new_table_header.index_num = t_h->index_num;
    memcpy(table_buf, &new_table_header, sizeof(Table_Header));    

    // 查看对应列是否存在
    bool exist = false;
    for (int i = 0; i < t_h->pro_num; i++) {
        Property* pro = (Property *)&table_buf[sizeof(Table_Header) + sizeof(Property) * i];
        string ori_name = pro->name;
        if (ori_name == forkey) {
            if (pro->forkey == false) {
                std::cout << ori_name << " isn't foreign key!" << std::endl;
                b_manager->release(new_bufID);
                f_manager->closeFile(new_fileID);
                return;
            }
            pro->forkey = false;
            memcpy(&table_buf[sizeof(Table_Header) + sizeof(Property) * i], pro, sizeof(Property));
            exist = true;
            break;
        }
    }
    if (exist == false) {
        std::cout << "Error : " << forkey << "doesn't in table!" << std::endl;
        b_manager->release(new_bufID);
        f_manager->closeFile(new_fileID);
        return;
    } 
    std::list<Index>  index_list;
    for (int i = 0; i < t_h->index_num; i++) {
        Index* index = (Index *)&table_buf[sizeof(Table_Header) + sizeof(Property) * t_h->pro_num + sizeof(PriKey) * t_h->prikey_num + sizeof(ForKey) * old_forkey_num + sizeof(Index) * i];
        string i_n = index->name;
        index_list.push_back(*index);
    }
    // 直接删除外键
    std::list<ForKey> for_list;
    for (int i = 0; i < old_forkey_num; i++) {
        ForKey* foreignkey = (ForKey *)&table_buf[sizeof(Table_Header) + sizeof(Property)  * t_h->pro_num + sizeof(PriKey) * t_h->prikey_num + sizeof(ForKey) * i];
        string ori_name = foreignkey->name;
        if (forkey != ori_name) {
            for_list.push_back(*foreignkey);
        } else {
            string prikey = (*foreignkey).prename;
            int pripage = (*foreignkey).pretable;
            bool drop_suc = drop_foreign_key(forkey, pageID, prikey, pripage);
            if (drop_suc == false) {
                std::cout << "Error : Reference key " << prikey << "doesn't exit!" << std::endl;
                f_manager->closeFile(new_fileID);
                return;           
            }
        }
    }  
    // 重新写入index
    int off = 0;
    for (auto it = index_list.begin(); it != index_list.end(); it++) {
        string i_n = (*it).name;
        memcpy(&table_buf[sizeof(Table_Header) + sizeof(Property) * t_h->pro_num + sizeof(PriKey) * t_h->prikey_num + sizeof(ForKey) * t_h->forkey_num + sizeof(Index) * off], &(*it), sizeof(Index));
        off ++;
    }   
    off = 0;
    for (auto it = for_list.begin(); it != for_list.end(); it++) {
        memcpy(&table_buf[sizeof(Table_Header) + sizeof(Property) * t_h->pro_num + sizeof(PriKey) * t_h->prikey_num + sizeof(ForKey) * off], &(*it), sizeof(ForKey));
        off ++;
    }
    b_manager->markDirty(new_bufID);
    b_manager->writeBack(new_bufID);
    f_manager->closeFile(new_fileID);
}

void System_manager::update_Index(int tableID) {
    int b_ID = 0;
    int fID = 0;
    f_manager->openFile(database_now.data(), fID);
    BufType t_buf = b_manager->getPage(fID, tableID, b_ID);
    Table_Header * t_h = (Table_Header *)t_buf;
    t_h->max_priIndex++;
    b_manager->markDirty(b_ID);
    b_manager->writeBack(b_ID);
    f_manager->closeFile(fID);
}

int System_manager::get_Index(int tableID) {
    int b_ID = 0;
    int fID = 0;
    f_manager->openFile(database_now.data(), fID);
    BufType t_buf = b_manager->getPage(fID, tableID, b_ID);
    Table_Header * t_h = (Table_Header *)t_buf;
    int index_value = t_h->max_priIndex;
    b_manager->release(b_ID);
    f_manager->closeFile(fID);
    return index_value;
}

std::string System_manager::get_database_now() {
    return database_now;
}

void System_manager::add_index(std::string table_name, std::string index_name) {
    int new_fileID = 0;
    int new_bufID = 0;
    f_manager->openFile(database_now.data(), new_fileID);
    int pageID = get_table_ID(new_fileID, table_name);
    if (pageID == -1) {
        std::cout << "Error : table " << table_name << "doesn't exit!" << std::endl;
        f_manager->closeFile(new_fileID);
        return;
    }

    BufType table_buf = b_manager->getPage(new_fileID, pageID, new_bufID);    
    Table_Header* t_h = (Table_Header *)table_buf;
    Table_Header new_table_header;

    new_table_header.pro_num = t_h->pro_num; 
    new_table_header.prikey_num = t_h->prikey_num; 
    new_table_header.forkey_num = t_h->forkey_num;
    new_table_header.index_num = t_h->index_num + 1;
    memcpy(table_buf, &new_table_header, sizeof(Table_Header));

    // 查看对应列是否存在
    bool exit = false;
    for (int i = 0; i < t_h->pro_num; i++) {
        Index* index = (Index *)&table_buf[sizeof(Table_Header) + sizeof(Property) * i];
        string index_namei = index->name;
        if (index_name == index_namei) {
            exit = true;
            break;
        }
    }
    if (exit == false) {
        std::cout << "Error : " << index_name << " doesn't in table!" << std::endl;
        b_manager->release(new_bufID);
        f_manager->closeFile(new_fileID);
        return;        
    }

    // 直接写入Index
    Index new_index;
    for (int i = 0; i < 20; i++)
        new_index.name[i] = 0;
    index_name.copy(new_index.name, 20, 0);
    memcpy(&table_buf[sizeof(Table_Header) + sizeof(Property) * t_h->pro_num + sizeof(PriKey) * t_h->prikey_num + sizeof(ForKey) * t_h->forkey_num + sizeof(Index) * (t_h->index_num - 1)], &new_index, sizeof(Index));
    b_manager->markDirty(new_bufID);
    b_manager->writeBack(new_bufID);
    f_manager->closeFile(new_fileID);
}

void System_manager::drop_index(std::string table_name, std::string index_name) {
    int new_fileID = 0;
    int new_bufID = 0;
    f_manager->openFile(database_now.data(), new_fileID);
    int pageID = get_table_ID(new_fileID, table_name);
    if (pageID == -1) {
        std::cout << "Error : table " << table_name << "doesn't exit!" << std::endl;
        f_manager->closeFile(new_fileID);
        return;
    }

    BufType table_buf = b_manager->getPage(new_fileID, pageID, new_bufID);    
    Table_Header* t_h = (Table_Header *)table_buf;
    Table_Header new_table_header;

    new_table_header.pro_num = t_h->pro_num; 
    new_table_header.prikey_num = t_h->prikey_num; 
    new_table_header.forkey_num = t_h->forkey_num;
    new_table_header.index_num = t_h->index_num - 1;
    memcpy(table_buf, &new_table_header, sizeof(Table_Header));

    // 直接删除Index
    std::list<Index> index_list;
    bool has_del = false;
    for (int i = 0; i < t_h->index_num + 1; i++) {
        Index* index = (Index *)&table_buf[sizeof(Table_Header) + sizeof(Property)  * t_h->pro_num + sizeof(PriKey) * t_h->prikey_num + sizeof(ForKey) * t_h->forkey_num + sizeof(Index) * i];
        string index_namei = index->name;
        if (index_namei != index_name) {
            index_list.push_back(*index);
        } else {
            has_del = true;
        }
    }
    if (has_del == false) std::cout << "Error Index " << index_name << " doesn't exit!" << std::endl;
    
    int off = 0;
    for (auto it = index_list.begin(); it != index_list.end(); it++) {
        string i_n = (*it).name;
        memcpy(&table_buf[sizeof(Table_Header) + sizeof(Property) * t_h->pro_num + sizeof(PriKey) * t_h->prikey_num + sizeof(ForKey) * t_h->forkey_num + sizeof(Index) * off], &(*it), sizeof(Index));
        off ++;
    }   
    b_manager->markDirty(new_bufID);
    b_manager->writeBack(new_bufID);
    f_manager->closeFile(new_fileID);        
}