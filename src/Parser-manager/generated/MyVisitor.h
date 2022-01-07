#include "SQLBaseVisitor.h"

class MyVisitor : public SQLBaseVisitor{
    virtual antlrcpp::Any visitCreate_db(SQLParser::Create_dbContext *ctx);
};