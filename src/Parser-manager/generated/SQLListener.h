
// Generated from SQL.g4 by ANTLR 4.9.3

#pragma once


#include "../third_party/antlr4-cpp-runtime-4.7.2/src/antlr4-runtime.h"
#include "SQLParser.h"


/**
 * This interface defines an abstract listener for a parse tree produced by SQLParser.
 */
class  SQLListener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterProgram(SQLParser::ProgramContext *ctx) = 0;
  virtual void exitProgram(SQLParser::ProgramContext *ctx) = 0;

  virtual void enterStatement(SQLParser::StatementContext *ctx) = 0;
  virtual void exitStatement(SQLParser::StatementContext *ctx) = 0;

  virtual void enterCreate_db(SQLParser::Create_dbContext *ctx) = 0;
  virtual void exitCreate_db(SQLParser::Create_dbContext *ctx) = 0;

  virtual void enterDrop_db(SQLParser::Drop_dbContext *ctx) = 0;
  virtual void exitDrop_db(SQLParser::Drop_dbContext *ctx) = 0;

  virtual void enterShow_dbs(SQLParser::Show_dbsContext *ctx) = 0;
  virtual void exitShow_dbs(SQLParser::Show_dbsContext *ctx) = 0;

  virtual void enterUse_db(SQLParser::Use_dbContext *ctx) = 0;
  virtual void exitUse_db(SQLParser::Use_dbContext *ctx) = 0;

  virtual void enterShow_tables(SQLParser::Show_tablesContext *ctx) = 0;
  virtual void exitShow_tables(SQLParser::Show_tablesContext *ctx) = 0;

  virtual void enterShow_indexes(SQLParser::Show_indexesContext *ctx) = 0;
  virtual void exitShow_indexes(SQLParser::Show_indexesContext *ctx) = 0;

  virtual void enterLoad_data(SQLParser::Load_dataContext *ctx) = 0;
  virtual void exitLoad_data(SQLParser::Load_dataContext *ctx) = 0;

  virtual void enterDump_data(SQLParser::Dump_dataContext *ctx) = 0;
  virtual void exitDump_data(SQLParser::Dump_dataContext *ctx) = 0;

  virtual void enterCreate_table(SQLParser::Create_tableContext *ctx) = 0;
  virtual void exitCreate_table(SQLParser::Create_tableContext *ctx) = 0;

  virtual void enterDrop_table(SQLParser::Drop_tableContext *ctx) = 0;
  virtual void exitDrop_table(SQLParser::Drop_tableContext *ctx) = 0;

  virtual void enterDescribe_table(SQLParser::Describe_tableContext *ctx) = 0;
  virtual void exitDescribe_table(SQLParser::Describe_tableContext *ctx) = 0;

  virtual void enterInsert_into_table(SQLParser::Insert_into_tableContext *ctx) = 0;
  virtual void exitInsert_into_table(SQLParser::Insert_into_tableContext *ctx) = 0;

  virtual void enterDelete_from_table(SQLParser::Delete_from_tableContext *ctx) = 0;
  virtual void exitDelete_from_table(SQLParser::Delete_from_tableContext *ctx) = 0;

  virtual void enterUpdate_table(SQLParser::Update_tableContext *ctx) = 0;
  virtual void exitUpdate_table(SQLParser::Update_tableContext *ctx) = 0;

  virtual void enterSelect_table_(SQLParser::Select_table_Context *ctx) = 0;
  virtual void exitSelect_table_(SQLParser::Select_table_Context *ctx) = 0;

  virtual void enterSelect_table(SQLParser::Select_tableContext *ctx) = 0;
  virtual void exitSelect_table(SQLParser::Select_tableContext *ctx) = 0;

  virtual void enterAlter_add_index(SQLParser::Alter_add_indexContext *ctx) = 0;
  virtual void exitAlter_add_index(SQLParser::Alter_add_indexContext *ctx) = 0;

  virtual void enterAlter_drop_index(SQLParser::Alter_drop_indexContext *ctx) = 0;
  virtual void exitAlter_drop_index(SQLParser::Alter_drop_indexContext *ctx) = 0;

  virtual void enterAlter_table_drop_pk(SQLParser::Alter_table_drop_pkContext *ctx) = 0;
  virtual void exitAlter_table_drop_pk(SQLParser::Alter_table_drop_pkContext *ctx) = 0;

  virtual void enterAlter_table_drop_foreign_key(SQLParser::Alter_table_drop_foreign_keyContext *ctx) = 0;
  virtual void exitAlter_table_drop_foreign_key(SQLParser::Alter_table_drop_foreign_keyContext *ctx) = 0;

  virtual void enterAlter_table_add_pk(SQLParser::Alter_table_add_pkContext *ctx) = 0;
  virtual void exitAlter_table_add_pk(SQLParser::Alter_table_add_pkContext *ctx) = 0;

  virtual void enterAlter_table_add_foreign_key(SQLParser::Alter_table_add_foreign_keyContext *ctx) = 0;
  virtual void exitAlter_table_add_foreign_key(SQLParser::Alter_table_add_foreign_keyContext *ctx) = 0;

  virtual void enterAlter_table_add_unique(SQLParser::Alter_table_add_uniqueContext *ctx) = 0;
  virtual void exitAlter_table_add_unique(SQLParser::Alter_table_add_uniqueContext *ctx) = 0;

  virtual void enterField_list(SQLParser::Field_listContext *ctx) = 0;
  virtual void exitField_list(SQLParser::Field_listContext *ctx) = 0;

  virtual void enterNormal_field(SQLParser::Normal_fieldContext *ctx) = 0;
  virtual void exitNormal_field(SQLParser::Normal_fieldContext *ctx) = 0;

  virtual void enterPrimary_key_field(SQLParser::Primary_key_fieldContext *ctx) = 0;
  virtual void exitPrimary_key_field(SQLParser::Primary_key_fieldContext *ctx) = 0;

  virtual void enterForeign_key_field(SQLParser::Foreign_key_fieldContext *ctx) = 0;
  virtual void exitForeign_key_field(SQLParser::Foreign_key_fieldContext *ctx) = 0;

  virtual void enterType_(SQLParser::Type_Context *ctx) = 0;
  virtual void exitType_(SQLParser::Type_Context *ctx) = 0;

  virtual void enterValue_lists(SQLParser::Value_listsContext *ctx) = 0;
  virtual void exitValue_lists(SQLParser::Value_listsContext *ctx) = 0;

  virtual void enterValue_list(SQLParser::Value_listContext *ctx) = 0;
  virtual void exitValue_list(SQLParser::Value_listContext *ctx) = 0;

  virtual void enterValue(SQLParser::ValueContext *ctx) = 0;
  virtual void exitValue(SQLParser::ValueContext *ctx) = 0;

  virtual void enterWhere_and_clause(SQLParser::Where_and_clauseContext *ctx) = 0;
  virtual void exitWhere_and_clause(SQLParser::Where_and_clauseContext *ctx) = 0;

  virtual void enterWhere_operator_expression(SQLParser::Where_operator_expressionContext *ctx) = 0;
  virtual void exitWhere_operator_expression(SQLParser::Where_operator_expressionContext *ctx) = 0;

  virtual void enterWhere_operator_select(SQLParser::Where_operator_selectContext *ctx) = 0;
  virtual void exitWhere_operator_select(SQLParser::Where_operator_selectContext *ctx) = 0;

  virtual void enterWhere_null(SQLParser::Where_nullContext *ctx) = 0;
  virtual void exitWhere_null(SQLParser::Where_nullContext *ctx) = 0;

  virtual void enterWhere_in_list(SQLParser::Where_in_listContext *ctx) = 0;
  virtual void exitWhere_in_list(SQLParser::Where_in_listContext *ctx) = 0;

  virtual void enterWhere_in_select(SQLParser::Where_in_selectContext *ctx) = 0;
  virtual void exitWhere_in_select(SQLParser::Where_in_selectContext *ctx) = 0;

  virtual void enterWhere_like_string(SQLParser::Where_like_stringContext *ctx) = 0;
  virtual void exitWhere_like_string(SQLParser::Where_like_stringContext *ctx) = 0;

  virtual void enterColumn(SQLParser::ColumnContext *ctx) = 0;
  virtual void exitColumn(SQLParser::ColumnContext *ctx) = 0;

  virtual void enterExpression(SQLParser::ExpressionContext *ctx) = 0;
  virtual void exitExpression(SQLParser::ExpressionContext *ctx) = 0;

  virtual void enterSet_clause(SQLParser::Set_clauseContext *ctx) = 0;
  virtual void exitSet_clause(SQLParser::Set_clauseContext *ctx) = 0;

  virtual void enterSelectors(SQLParser::SelectorsContext *ctx) = 0;
  virtual void exitSelectors(SQLParser::SelectorsContext *ctx) = 0;

  virtual void enterSelector(SQLParser::SelectorContext *ctx) = 0;
  virtual void exitSelector(SQLParser::SelectorContext *ctx) = 0;

  virtual void enterIdentifiers(SQLParser::IdentifiersContext *ctx) = 0;
  virtual void exitIdentifiers(SQLParser::IdentifiersContext *ctx) = 0;

  virtual void enterOperator_(SQLParser::Operator_Context *ctx) = 0;
  virtual void exitOperator_(SQLParser::Operator_Context *ctx) = 0;

  virtual void enterAggregator(SQLParser::AggregatorContext *ctx) = 0;
  virtual void exitAggregator(SQLParser::AggregatorContext *ctx) = 0;


};

