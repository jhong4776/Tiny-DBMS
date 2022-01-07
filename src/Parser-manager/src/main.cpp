#include <iostream>
#include "../generated/SQLLexer.h"
#include "../generated/SQLParser.h"
#include "../generated/SQLBaseVisitor.h"
#include "../generated/MyVisitor.h"
using namespace antlr4;
using namespace std;
int main(int argc, const char* argv[]) {
    const char* filepath = argv[1];
    std::ifstream ifs;
    ifs.open(filepath);
    ANTLRInputStream input(ifs);
    SQLLexer lexer(&input);
    CommonTokenStream tokens(&lexer);
    SQLParser parser(&tokens);
    SQLParser::ProgramContext* pro = parser.program();
    if (parser.getNumberOfSyntaxErrors() > 0) {
        cout<<"sql file syntax error"<<endl;
        return 0;
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
    ifs.close();
    return 0;
}