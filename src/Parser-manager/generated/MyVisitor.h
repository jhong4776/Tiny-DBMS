#include "SQLBaseVisitor.h"
#include "../../Index-manager/Index_manager.h"
#include "../../System-manager/System_manager.h"
#include "../../Record-manager/Record_manager.h"
#include "../../Header_define.h"
#include <string>

struct Index_record {
    int k;
    std::string index_name;
};

class MyVisitor : public SQLBaseVisitor{
public:
    std::string db_now;
    MyVisitor();
    ~MyVisitor();
    virtual antlrcpp::Any visitCreate_db(SQLParser::Create_dbContext *ctx);
    virtual antlrcpp::Any visitDrop_db(SQLParser::Drop_dbContext *ctx);
    virtual antlrcpp::Any visitShow_dbs(SQLParser::Show_dbsContext *ctx);
    virtual antlrcpp::Any visitUse_db(SQLParser::Use_dbContext *ctx);
    virtual antlrcpp::Any visitCreate_table(SQLParser::Create_tableContext *ctx);
    virtual antlrcpp::Any visitField_list(SQLParser::Field_listContext *ctx);
    virtual antlrcpp::Any visitDescribe_table(SQLParser::Describe_tableContext *ctx);
    virtual antlrcpp::Any visitDrop_table(SQLParser::Drop_tableContext *ctx);
    virtual antlrcpp::Any visitAlter_table_add_foreign_key(SQLParser::Alter_table_add_foreign_keyContext *ctx);
    virtual antlrcpp::Any visitAlter_table_drop_foreign_key(SQLParser::Alter_table_drop_foreign_keyContext *ctx);
    virtual antlrcpp::Any visitShow_tables(SQLParser::Show_tablesContext *ctx);
    virtual antlrcpp::Any visitAlter_table_add_pk(SQLParser::Alter_table_add_pkContext *ctx);
    virtual antlrcpp::Any visitAlter_table_drop_pk(SQLParser::Alter_table_drop_pkContext *ctx);
    virtual antlrcpp::Any visitAlter_add_index(SQLParser::Alter_add_indexContext *ctx);
    virtual antlrcpp::Any visitAlter_drop_index(SQLParser::Alter_drop_indexContext *ctx);
    virtual antlrcpp::Any visitInsert_into_table(SQLParser::Insert_into_tableContext *ctx);
    virtual antlrcpp::Any visitDelete_from_table(SQLParser::Delete_from_tableContext *ctx);
    virtual antlrcpp::Any visitUpdate_table(SQLParser::Update_tableContext *ctx);
    virtual antlrcpp::Any visitSelect_table_(SQLParser::Select_table_Context *ctx);
};