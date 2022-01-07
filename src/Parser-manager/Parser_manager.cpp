#include "Parser_manager.h"
#include <iostream>

using namespace antlr4;
using namespace std;

Parser_manager::Parser_manager() {

}

Parser_manager::~Parser_manager() {
    
}

void Parser_manager::parse(std::string sSQL) {
    ANTLRInputStream sInputStream(sSQL);
    SQLLexer lexer(&sInputStream);
    CommonTokenStream tokens(&lexer);
    SQLParser parser(&tokens);
    
    SQLParser::ProgramContext* pro = parser.program();
    if (parser.getNumberOfSyntaxErrors() > 0) {
        cout<<"sql file syntax error"<<endl;
        return;
    }

    for (auto t : tokens.getTokens()) {
        cout<<t->toString()<<endl;
    }
    for (int i = 0; i < pro->children.size(); ++i) {
        auto child = pro->children.at(i);
        cout<<child->toString()<<endl;
    }

    MyVisitor lbase; 
    lbase.visit(pro);    
}