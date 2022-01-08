#include <iostream>
#include "generated/SQLLexer.h"
#include "generated/SQLParser.h"
#include "generated/SQLBaseVisitor.h"
#include "generated/MyVisitor.h"
#include "../Index-manager/Index_manager.h"
#include "../System-manager/System_manager.h"
#include <string>

class Parser_manager {
    public:
    Parser_manager();
    ~Parser_manager();
    void parse(std::string sSQL, MyVisitor* lbase);
};