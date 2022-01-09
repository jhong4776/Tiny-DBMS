#ifndef CHECK_M
#define CHECK_M

#include <iostream>
#include <string>
#include "../System-manager/System_manager.h"
#include "../Record-manager/Record_manager.h"

class CheckTool {
    std::string db_now;
public:
    CheckTool(std::string database_now);
    ~CheckTool();
    int getKey(std::string table_name, std::string index, int rid);
    bool record_equa(int tableID, std::string colunm, int num);
    bool record_equa(int tableID, std::string colunm, std::string num);
    bool record_equa(int tableID, std::string colunm, float num);

    bool checkPrikey(std::string table_name, std::string forkey, int num);
    bool checkPrikey(std::string table_name, std::string forkey, float num);
    bool checkPrikey(std::string table_name, std::string forkey, std::string num);
};

#endif