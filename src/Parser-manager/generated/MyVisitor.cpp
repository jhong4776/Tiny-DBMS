#include "MyVisitor.h"
#include "../../Check-manager/Check_manager.h"
#include "../../Record-manager/Record_manager.h"
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

antlrcpp::Any MyVisitor::visitDrop_db(SQLParser::Drop_dbContext *ctx) {
    System_manager s_m;
    s_m.drop_database(ctx->Identifier()->getText());   
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
    t_h.index_num = 0; t_h.max_page = 0;
    
    string name = ctx->Identifier()->getText();
    bool create_t = s_m.create_table(name, t_h, properties, prikeys, forkeys);
    
    if (create_t == false)
        return 0;

    int tableID = s_m.get_table_ID(-1, name);
    for (auto it = prikeys.begin(); it != prikeys.end(); it++) {
        string priname = (*it).name;
        IndexHandler i_m;
        s_m.add_index(name, priname);
        i_m.CreateIndex(db_now, tableID, priname);
        i_m.WriteBack();
    }

    Record_manager r_m;
    r_m.createFile(db_now, s_m.get_table_ID(-1, name));
    
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

    CheckTool tool(db_now);
    bool check = tool.checkDup(ctx->Identifier()[0]->getText(), ctx->identifiers()->getText());
    if (check == false) {
        cout << "Error : primary key duplicated!" << endl;
        return 0;
    }
    s_m.add_prikey(ctx->Identifier()[0]->getText(), ctx->identifiers()->getText());
    return visitChildren(ctx);
}

antlrcpp::Any MyVisitor::visitAlter_table_drop_pk(SQLParser::Alter_table_drop_pkContext *ctx) {
    System_manager s_m;
    s_m.use_database(db_now);

    bool suc = s_m.drop_prikey(ctx->Identifier()[0]->getText(), ctx->Identifier()[1]->getText());
    if (suc)
        s_m.drop_index(ctx->Identifier()[0]->getText(), ctx->Identifier()[1]->getText());

    return visitChildren(ctx);
}

antlrcpp::Any MyVisitor::visitAlter_add_index(SQLParser::Alter_add_indexContext *ctx) {
    System_manager s_m;
    s_m.use_database(db_now);

    string tablename = ctx->Identifier()->getText();
    int table_ID = s_m.get_table_ID(-1, tablename);
    string indexname = ctx->identifiers()->getText();
    vector<Property> pro_v = s_m.get_property_vector(table_ID);

    for (int i = 0; i < pro_v.size(); i++) {
        string proname = pro_v[i].name;
        if (proname == indexname) {
            if (pro_v[i].type != 0) {
                cout << "Can't build string index or float index" << endl;
                return 0;
            } else {
                break;
            }
        }
    }

    CheckTool tool(db_now);
    tool.checkDup(tablename, indexname);
    return visitChildren(ctx);
}

antlrcpp::Any MyVisitor::visitAlter_drop_index(SQLParser::Alter_drop_indexContext *ctx) {
    System_manager s_m;
    s_m.use_database(db_now);

    s_m.drop_index(ctx->Identifier()->getText(), ctx->identifiers()->getText());
    return visitChildren(ctx);    
}

antlrcpp::Any MyVisitor::visitInsert_into_table(SQLParser::Insert_into_tableContext *ctx) {
    System_manager s_m;
    s_m.use_database(db_now);

    CheckTool ctool(db_now);

    string table_name = ctx->Identifier()->getText();
    int table_ID = s_m.get_table_ID(-1, table_name);
    vector<Property> p_v = s_m.get_property_vector(table_ID);
    char input[1024];
    for (int i = 0; i < 1024; i++)
        input[i] = 0;
    int len = 0;

    vector<Index> i_v = s_m.get_index_vector(table_ID);
    vector<Index_record> ir_v;

    for (int i = 0; i < ctx->value_lists()->value_list()[0]->value().size(); i++) {
        SQLParser::ValueContext* vlx = ctx->value_lists()->value_list()[0]->value()[i];
        if(vlx->Integer() != nullptr){
            if(p_v[i].type != 0) {
                cout << "Error : type doesn't match" << endl;
                return 0;
            } 
            int num = atoi(vlx->Integer()->getText().c_str());
            memcpy(&input[len], &num, sizeof(int));

            string coloum = p_v[i].name;
            if(!ctool.checkPrikey(table_name, coloum, num)) {
                cout << "Reference error!" << endl;
                return 0;
            }
            
            for (int j = 0; j < i_v.size(); j++) {
                string indexname = i_v[j].name;
                if (coloum == indexname) {
                    if (!ctool.indexCheckDup(table_ID, coloum, num)) {
                        cout << "Error : Index Duplicated!" << endl;
                        return 0;
                    }

                    Index_record ir;
                    ir.index_name = indexname; ir.k = num;
                    ir_v.push_back(ir);
                    break;
                }
            }
        }
        if(vlx->String() != nullptr) {
            if(p_v[i].type != 1) {
                cout << "Error : type doesn't match" << endl;
                return 0;
            }     
            if (vlx->String()->getText().size() > p_v[i].len) {
                cout << "Error : varchar " << vlx->String()->getText() << " is too large." << endl;
            }
            memcpy(&input[len], vlx->String()->getText().data(), p_v[i].len);

            string pri = vlx->String()->getText();

            string coloum = p_v[i].name;
            if(!ctool.checkPrikey(table_name, coloum, pri)) {
                cout << "Reference error!" << endl;
                return 0;
            }      
        }
        if(vlx->Float() != nullptr) {
            if(p_v[i].type != 2) {
                cout << "Error : type doesn't match" << endl;
                return 0;
            }  
            float num = (float)atof(vlx->Float()->getText().c_str());
            memcpy(&input[len], &num, sizeof(float));

            string coloum = p_v[i].name;
            if(!ctool.checkPrikey(table_name, coloum, num)) {
                cout << "Reference error!" << endl;
                return 0;
            }            
        }
        if(vlx->Null() != nullptr) {
            if (p_v[i].be_null == false) {
                cout << "Error : " << p_v[i].name << " can not be null" << endl;
                return 0;
            }
        }
        len += p_v[i].len;
    }

    Record_manager r_m;
    r_m.openFile(db_now, table_ID);
    int rid = r_m.insertRecord(input);

    IndexHandler i_m;
    for (int i = 0; i < ir_v.size(); i++) {
        i_m.OpenIndex(db_now, table_ID, ir_v[i].index_name);
        i_m.InsertRecoder(ir_v[i].k, rid);
        i_m.WriteBack();
    }
    r_m.closeFile();
    return visitChildren(ctx); 
}

antlrcpp::Any MyVisitor::visitDelete_from_table(SQLParser::Delete_from_tableContext *ctx) {
    System_manager s_m;
    s_m.use_database(db_now);

    CheckTool tool(db_now);

    string table_name = ctx->Identifier()->getText();
    SQLParser::Where_operator_expressionContext* w_e = dynamic_cast<SQLParser::Where_operator_expressionContext*> (ctx->where_and_clause()->where_clause()[0]);
    string colunm_name = w_e->column()->Identifier()[1]->getText();
    SQLParser::ValueContext* v_c = dynamic_cast<SQLParser::ValueContext*>(w_e->expression()->value());
    if (w_e->operator_()->getText() == "=") {
        if(v_c->Integer() != nullptr) {
            int num = atoi(v_c->getText().c_str());

            if (tool.getColunmType(table_name, colunm_name) != 0) {
                cout << "colunm error!" << endl;
                return 0;
            }
            tool.checkForkey(table_name, colunm_name, num);
        }
        if(v_c->String() != nullptr) {
            string data = v_c->getText();
            if (tool.getColunmType(table_name, colunm_name) != 1) {
                cout << "colunm error!" << endl;
                return 0;
            }

            tool.checkForkey(table_name, colunm_name, data);

            tool.mark_equa(s_m.get_table_ID(-1, table_name), colunm_name, data);              
        }
        if(v_c->Float() != nullptr) {
            float num = (float)atof(v_c->getText().c_str());

            if (tool.getColunmType(table_name, colunm_name) != 2) {
                cout << "colunm error!" << endl;
                return 0;
            }
            tool.checkForkey(table_name, colunm_name, num);

            tool.mark_equa(s_m.get_table_ID(-1, table_name), colunm_name, num);            
        }
    }
    return visitChildren(ctx);     
}

antlrcpp::Any MyVisitor::visitUpdate_table(SQLParser::Update_tableContext *ctx) {
    System_manager s_m;
    s_m.use_database(db_now);

    string table1 = ctx->Identifier()->getText();
    string column1 = ctx->set_clause()->Identifier()[0]->getText();
    string value1 = ctx->set_clause()->value()[0]->getText();
    SQLParser::Where_operator_expressionContext* w_e = dynamic_cast<SQLParser::Where_operator_expressionContext*> (ctx->where_and_clause()->where_clause()[0]);
    string table2 = w_e->column()->Identifier()[0]->getText();
    string column2 = w_e->column()->Identifier()[1]->getText();
    string value2 = w_e->expression()->value()->getText();
    if (w_e->operator_()->getText() == "=") {
    }

    return visitChildren(ctx);     
}

antlrcpp::Any MyVisitor::visitSelect_table_(SQLParser::Select_table_Context *ctx) {
    System_manager s_m;
    s_m.use_database(db_now);

    string table_name = ctx->select_table()->identifiers()->getText();
    int tableID = s_m.get_table_ID(-1, table_name);
    if (ctx->select_table()->selectors()->getText() == "*" && ctx->select_table()->where_and_clause() == nullptr) {
        Record_manager r_m;
        r_m.openFile(db_now, tableID);
        r_m.select_all(db_now, tableID);
        r_m.closeFile();
    } else {
        SQLParser::Where_operator_expressionContext* w_e = dynamic_cast<SQLParser::Where_operator_expressionContext*>(ctx->select_table()->where_and_clause()->where_clause()[0]);
        string table_name1 = w_e->column()->Identifier()[0]->getText();
        int table_ID1 = s_m.get_table_ID(-1, table_name1);
        if (w_e != nullptr) {
            if (w_e->operator_()->getText() == "=") {
                string colunm1 = w_e->column()->Identifier()[1]->getText();
                SQLParser::ValueContext* v_c = dynamic_cast<SQLParser::ValueContext *>(w_e->expression()->value());
                if (v_c != nullptr) {
                    Record_manager r_m1;
                    r_m1.openFile(db_now, table_ID1);
                    if (v_c->Integer() != nullptr) {
                        int num = atoi(v_c->getText().c_str());
                        vector<Index> i_v = s_m.get_index_vector(table_ID1);
                        bool is_index = false;
                        for (int i = 0; i < i_v.size(); i++) {
                            string i_name = i_v[i].name;
                            if (i_name == colunm1) {
                                is_index = true;
                                break;
                            }
                        }
                        if (is_index == true) {
                            IndexHandler i_m;
                            i_m.OpenIndex(db_now, table_ID1, colunm1);
                            int rid = i_m.Seainth(num, num+1);

                            char output[1024];
                            if(r_m1.getRecord(rid + sizeof(Record_Header), output) == LIVE) {
                                vector<Property> pro_v = s_m.get_property_vector(table_ID1);
                                int offset = 0;
                                for (int i = 0; i < pro_v.size(); i++) {
                                    string pro_name = pro_v[i].name;
                                    if (pro_v[i].type == 0) {
                                        int* target = (int *)&output[offset];
                                        cout << pro_name << " = " << *target << " ";
                                        offset += sizeof(int);                                    
                                    }
                                    else if (pro_v[i].type == 1) {
                                        int size = pro_v[i].len;
                                        char target[size];
                                        memcpy(target, &output[offset], size);
                                        string s_target = target;
                                        cout << pro_name << " = " << s_target << " ";
                                        offset += size;
                                    }
                                    else if (pro_v[i].type == 2) {
                                        float* target = (float *)&output[offset];
                                        cout << pro_name << " = " << *target << " ";
                                        offset += sizeof(float);
                                    }                                
                                }
                                cout << endl;
                            }
                        } else {
                            r_m1.select_equa(db_now, table_ID1, num, colunm1);
                        }
                    }
                    else if (v_c->String() != nullptr) {
                        r_m1.select_equa(db_now, table_ID1, v_c->getText(), colunm1);                        
                    }                    
                    else if (v_c->Float() != nullptr) {
                        float num = (float)atof(v_c->getText().c_str());
                        r_m1.select_equa(db_now, table_ID1, num, colunm1);                        
                    }
                    r_m1.closeFile();
                }
            }
        }
    }

    return visitChildren(ctx);
}