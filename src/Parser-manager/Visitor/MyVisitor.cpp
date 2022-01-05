#include <string>
#include "src/antlr4-runtime.h"

// 包含你完成的Visitor类
#include "MyVisitor.h"

using namespace antlr4;

antlrcpp::Any MyVisitor::visitProgram(SQLParser::ProgramContext *ctx) {
    return visitChildren(ctx);
}

antlrcpp::Any MyVisitor::visitStatement(SQLParser::StatementContext *ctx) {
    return visitChildren(ctx);
}

antlrcpp::Any MyVisitor::visitCreate_db(SQLParser::Create_dbContext *ctx) {
    command = "create";
    object = "database";
    return visitChildren(ctx);
}

antlrcpp::Any MyVisitor::visitDrop_db(SQLParser::Drop_dbContext *ctx) {
    command = "drop";
    object = "database";
    return visitChildren(ctx);
}

antlrcpp::Any MyVisitor::visitShow_dbs(SQLParser::Show_dbsContext *ctx) {
    command = "show";
    object = "database";
    return visitChildren(ctx);
}

antlrcpp::Any MyVisitor::visitUse_db(SQLParser::Use_dbContext *ctx) {
    command = "use";
    object = "database";
    return visitChildren(ctx);
}

antlrcpp::Any MyVisitor::visitShow_tables(SQLParser::Show_tablesContext *ctx) {
    command = "show";
    object = "table";
    return visitChildren(ctx);
}

antlrcpp::Any MyVisitor::visitShow_indexes(SQLParser::Show_indexesContext *ctx) {
    command = "show";
    object = "index";
    return visitChildren(ctx);
}

antlrcpp::Any MyVisitor::visitLoad_data(SQLParser::Load_dataContext *ctx) {
    command = "load";
    object = "table";
    
    return visitChildren(ctx);
}

// 返回类型根据你的visitor决定
auto parse(std::string sSQL) {
 // 解析SQL语句sSQL的过程
 // 转化为输入流
 ANTLRInputStream sInputStream(sSQL);
 // 设置Lexer
 SQLLexer iLexer(&sInputStream);
 CommonTokenStream sTokenStream(&iLexer);
 // 设置Parser
 SQLParser iParser(&sTokenStream);
 SQLParser::ProgramContext* iTree = iParser.program();
 // 构造你的visitor
 MyVisitor iVisitor;
 // visitor模式下执行SQL解析过程
 // --如果采用解释器方式可以在解析过程中完成执行过程（相对简单，但是很难进行进一步优化，功能上已经达到实验要求）
 // --如果采用编译器方式则需要生成自行设计的物理执行执行计划（相对复杂，易于进行进一步优化，希望有能力的同学自行调研尝试）
 auto iRes = iVisitor.visit(iTree);
 return iRes;
}