#include "Parser_manager.h"
#include <iostream>

using namespace antlr4;
using namespace std;

Parser_manager::Parser_manager() {

}

Parser_manager::~Parser_manager() {
    
}

void Parser_manager::parse(std::string sSQL, MyVisitor* lbase) {
    ANTLRInputStream sInputStream(sSQL);
    SQLLexer lexer(&sInputStream);
    CommonTokenStream tokens(&lexer);
    SQLParser parser(&tokens);
    
    SQLParser::ProgramContext* pro = parser.program();

    lbase->visit(pro);    
}