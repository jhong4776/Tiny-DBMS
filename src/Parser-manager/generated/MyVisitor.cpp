#include "MyVisitor.h"
#include <iostream>

using namespace std;

antlrcpp::Any MyVisitor::visitCreate_db(SQLParser::Create_dbContext *ctx) {
    cout << "MyVisitor::visitCreate_db : " << ctx->Identifier()->getText() << endl;
    return visitChildren(ctx);
}