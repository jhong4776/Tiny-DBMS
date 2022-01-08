#include "MyVisitor.h"
#include <iostream>

using namespace std;

MyVisitor::MyVisitor(): SQLBaseVisitor(){ 
}

MyVisitor::~MyVisitor() {
}

antlrcpp::Any MyVisitor::visitCreate_db(SQLParser::Create_dbContext *ctx) {
    System_manager s_m;
    s_m.create_database(ctx->Identifier()->getText());
    return visitChildren(ctx);
}


antlrcpp::Any MyVisitor::visitShow_dbs(SQLParser::Show_dbsContext *ctx) {
    System_manager s_m;
    s_m.show_database();
    return visitChildren(ctx);
}

antlrcpp::Any MyVisitor::visitUse_db(SQLParser::Use_dbContext *ctx) {
    System_manager s_m;
    s_m.use_database(ctx->Identifier()->getText());
    db_now = ctx->Identifier()->getText();
    return visitChildren(ctx);
}

antlrcpp::Any MyVisitor::visitCreate_table(SQLParser::Create_tableContext *ctx) {
    cout << db_now << endl;
    System_manager s_m;
    s_m.use_database(db_now);
    Table_Header t_h;
    list<Property> properties;
    list<PriKey> prikeys;
    list<ForKey> forkeys;
    std::vector<SQLParser::FieldContext *> f_l = ctx->field_list()->field();

    for (int i = 0; i < f_l.size(); i++) {
        SQLParser::Normal_fieldContext* normal_field = dynamic_cast<SQLParser::Normal_fieldContext*>(f_l[i]);
        if (normal_field != nullptr) {
            Property pro;
            for (int i = 0; i < 20; i++) pro.name[i] = 0;
            normal_field->Identifier()->getText().copy(pro.name, 20, 0);
            cout << normal_field->type_()->getText() << endl;
            if (normal_field->type_()->getText() == "INT") {
                pro.type = 0;
                pro.len = sizeof(int);
            }
            else if (normal_field->type_()->getText() == "FLOAT") {
                pro.type = 2;
                pro.len = sizeof(float);
            }
            else {
                pro.type = 1;
                pro.len = atoi(normal_field->type_()->Integer()->getText().c_str()); 
            }
            
            if (normal_field->Null() != nullptr)
                pro.be_null = true;
            else
                pro.be_null = false;
            
            pro.prikey = 0; pro.forkey = 0;
            properties.push_back(pro);
        }

        SQLParser::Primary_key_fieldContext* pri_field = dynamic_cast<SQLParser::Primary_key_fieldContext *>(f_l[i]);
        if (pri_field != nullptr) {
            PriKey pri;
            for (int i = 0; i < 20; i++) {
                pri.name[i] = 0;
                pri.forname[i] = 0;
            }
            pri_field->identifiers()->getText().copy(pri.name, 20, 0);      
            pri.fortable = -1;

            prikeys.push_back(pri);

            for (auto it = properties.begin(); it != properties.end(); it++) {
                string pro_name = (*it).name;
                if (pro_name == pri_field->identifiers()->getText()) {
                    (*it).prikey = true;
                    break;
                }
            }
        }

        SQLParser::Foreign_key_fieldContext* for_field = dynamic_cast<SQLParser::Foreign_key_fieldContext *>(f_l[i]);
        if (for_field != nullptr) {
            ForKey forkey;
            for (int i = 0; i < 20; i++) {
                forkey.name[i] = 0;
                forkey.prename[i] = 0;
            }   
            for_field->identifiers()[0]->getText().copy(forkey.name, 20, 0); 
            for_field->identifiers()[1]->getText().copy(forkey.prename, 20, 0);
            forkey.pretable = s_m.get_table_ID(-1, for_field->Identifier()[0]->getText());

            forkeys.push_back(forkey);

            for (auto it = properties.begin(); it != properties.end(); it++) {
                string for_name = (*it).name;
                if (for_name ==for_field->identifiers()[0]->getText()) {
                    (*it).forkey = true;
                    break;
                }
            }
        }
    }
    t_h.pro_num = properties.size(); t_h.prikey_num = prikeys.size(); t_h.forkey_num = forkeys.size();
    
    string name = ctx->Identifier()->getText();
    s_m.create_table(name, t_h, properties, prikeys, forkeys);
    return visitChildren(ctx);
}

antlrcpp::Any MyVisitor::visitField_list(SQLParser::Field_listContext *ctx) {
    // cout << test << endl;
    // test ++;
    return visitChildren(ctx);
}

antlrcpp::Any MyVisitor::visitDescribe_table(SQLParser::Describe_tableContext *ctx) {
    System_manager s_m;
    s_m.use_database(db_now);

    s_m.table_schema(ctx->Identifier()->getText());
    return visitChildren(ctx);
}

antlrcpp::Any MyVisitor::visitDrop_table(SQLParser::Drop_tableContext *ctx) {
    System_manager s_m;
    s_m.use_database(db_now);

    s_m.drop_table(ctx->Identifier()->getText());
    return visitChildren(ctx);
}

antlrcpp::Any MyVisitor::visitShow_tables(SQLParser::Show_tablesContext *ctx) {
    System_manager s_m;
    s_m.use_database(db_now);
    s_m.show_table();
    return visitChildren(ctx);
}

antlrcpp::Any MyVisitor::visitAlter_table_add_foreign_key(SQLParser::Alter_table_add_foreign_keyContext *ctx) {
    System_manager s_m;
    s_m.use_database(db_now);

    cout << ctx->Identifier()[0]->getText() << " " << ctx->Identifier()[1]->getText() << " " << ctx->Identifier()[2]->getText() << endl;
    cout << ctx->identifiers()[0]->getText() << " " << ctx->identifiers()[1]->getText() << endl;

    string table_name = ctx->Identifier()[0]->getText();
    string forkey = ctx->identifiers()[0]->getText();
    string r_table = ctx->Identifier()[2]->getText();
    string prikey = ctx->identifiers()[1]->getText();

    s_m.add_foreignkey(table_name, forkey, r_table, prikey);
    return visitChildren(ctx);
}

antlrcpp::Any MyVisitor::visitAlter_table_drop_foreign_key(SQLParser::Alter_table_drop_foreign_keyContext *ctx) {
    System_manager s_m;
    s_m.use_database(db_now);

    s_m.drop_foreignkey(ctx->Identifier()[0]->getText(), ctx->Identifier()[1]->getText());
    return visitChildren(ctx);
}

antlrcpp::Any MyVisitor::visitAlter_table_add_pk(SQLParser::Alter_table_add_pkContext *ctx) {
    System_manager s_m;
    s_m.use_database(db_now);

    s_m.add_prikey(ctx->Identifier()[0]->getText(), ctx->identifiers()->getText());
    return visitChildren(ctx);
}

antlrcpp::Any MyVisitor::visitAlter_table_drop_pk(SQLParser::Alter_table_drop_pkContext *ctx) {
    System_manager s_m;
    s_m.use_database(db_now);

    s_m.drop_prikey(ctx->Identifier()[0]->getText(), ctx->Identifier()[1]->getText());

    return visitChildren(ctx);
}