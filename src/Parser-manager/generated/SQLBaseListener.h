
// Generated from SQL.g4 by ANTLR 4.9.3

#pragma once


#include "../third_party/antlr4-cpp-runtime-4.7.2/src/antlr4-runtime.h"
#include "SQLListener.h"


/**
 * This class provides an empty implementation of SQLListener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class  SQLBaseListener : public SQLListener {
public:

  virtual void enterProgram(SQLParser::ProgramContext * /*ctx*/) override { }
  virtual void exitProgram(SQLParser::ProgramContext * /*ctx*/) override { }

  virtual void enterStatement(SQLParser::StatementContext * /*ctx*/) override { }
  virtual void exitStatement(SQLParser::StatementContext * /*ctx*/) override { }

  virtual void enterCreate_db(SQLParser::Create_dbContext * /*ctx*/) override { }
  virtual void exitCreate_db(SQLParser::Create_dbContext * /*ctx*/) override { }

  virtual void enterDrop_db(SQLParser::Drop_dbContext * /*ctx*/) override { }
  virtual void exitDrop_db(SQLParser::Drop_dbContext * /*ctx*/) override { }

  virtual void enterShow_dbs(SQLParser::Show_dbsContext * /*ctx*/) override { }
  virtual void exitShow_dbs(SQLParser::Show_dbsContext * /*ctx*/) override { }

  virtual void enterUse_db(SQLParser::Use_dbContext * /*ctx*/) override { }
  virtual void exitUse_db(SQLParser::Use_dbContext * /*ctx*/) override { }

  virtual void enterShow_tables(SQLParser::Show_tablesContext * /*ctx*/) override { }
  virtual void exitShow_tables(SQLParser::Show_tablesContext * /*ctx*/) override { }

  virtual void enterShow_indexes(SQLParser::Show_indexesContext * /*ctx*/) override { }
  virtual void exitShow_indexes(SQLParser::Show_indexesContext * /*ctx*/) override { }

  virtual void enterLoad_data(SQLParser::Load_dataContext * /*ctx*/) override { }
  virtual void exitLoad_data(SQLParser::Load_dataContext * /*ctx*/) override { }

  virtual void enterDump_data(SQLParser::Dump_dataContext * /*ctx*/) override { }
  virtual void exitDump_data(SQLParser::Dump_dataContext * /*ctx*/) override { }

  virtual void enterCreate_table(SQLParser::Create_tableContext * /*ctx*/) override { }
  virtual void exitCreate_table(SQLParser::Create_tableContext * /*ctx*/) override { }

  virtual void enterDrop_table(SQLParser::Drop_tableContext * /*ctx*/) override { }
  virtual void exitDrop_table(SQLParser::Drop_tableContext * /*ctx*/) override { }

  virtual void enterDescribe_table(SQLParser::Describe_tableContext * /*ctx*/) override { }
  virtual void exitDescribe_table(SQLParser::Describe_tableContext * /*ctx*/) override { }

  virtual void enterInsert_into_table(SQLParser::Insert_into_tableContext * /*ctx*/) override { }
  virtual void exitInsert_into_table(SQLParser::Insert_into_tableContext * /*ctx*/) override { }

  virtual void enterDelete_from_table(SQLParser::Delete_from_tableContext * /*ctx*/) override { }
  virtual void exitDelete_from_table(SQLParser::Delete_from_tableContext * /*ctx*/) override { }

  virtual void enterUpdate_table(SQLParser::Update_tableContext * /*ctx*/) override { }
  virtual void exitUpdate_table(SQLParser::Update_tableContext * /*ctx*/) override { }

  virtual void enterSelect_table_(SQLParser::Select_table_Context * /*ctx*/) override { }
  virtual void exitSelect_table_(SQLParser::Select_table_Context * /*ctx*/) override { }

  virtual void enterSelect_table(SQLParser::Select_tableContext * /*ctx*/) override { }
  virtual void exitSelect_table(SQLParser::Select_tableContext * /*ctx*/) override { }

  virtual void enterAlter_add_index(SQLParser::Alter_add_indexContext * /*ctx*/) override { }
  virtual void exitAlter_add_index(SQLParser::Alter_add_indexContext * /*ctx*/) override { }

  virtual void enterAlter_drop_index(SQLParser::Alter_drop_indexContext * /*ctx*/) override { }
  virtual void exitAlter_drop_index(SQLParser::Alter_drop_indexContext * /*ctx*/) override { }

  virtual void enterAlter_table_drop_pk(SQLParser::Alter_table_drop_pkContext * /*ctx*/) override { }
  virtual void exitAlter_table_drop_pk(SQLParser::Alter_table_drop_pkContext * /*ctx*/) override { }

  virtual void enterAlter_table_drop_foreign_key(SQLParser::Alter_table_drop_foreign_keyContext * /*ctx*/) override { }
  virtual void exitAlter_table_drop_foreign_key(SQLParser::Alter_table_drop_foreign_keyContext * /*ctx*/) override { }

  virtual void enterAlter_table_add_pk(SQLParser::Alter_table_add_pkContext * /*ctx*/) override { }
  virtual void exitAlter_table_add_pk(SQLParser::Alter_table_add_pkContext * /*ctx*/) override { }

  virtual void enterAlter_table_add_foreign_key(SQLParser::Alter_table_add_foreign_keyContext * /*ctx*/) override { }
  virtual void exitAlter_table_add_foreign_key(SQLParser::Alter_table_add_foreign_keyContext * /*ctx*/) override { }

  virtual void enterAlter_table_add_unique(SQLParser::Alter_table_add_uniqueContext * /*ctx*/) override { }
  virtual void exitAlter_table_add_unique(SQLParser::Alter_table_add_uniqueContext * /*ctx*/) override { }

  virtual void enterField_list(SQLParser::Field_listContext * /*ctx*/) override { }
  virtual void exitField_list(SQLParser::Field_listContext * /*ctx*/) override { }

  virtual void enterNormal_field(SQLParser::Normal_fieldContext * /*ctx*/) override { }
  virtual void exitNormal_field(SQLParser::Normal_fieldContext * /*ctx*/) override { }

  virtual void enterPrimary_key_field(SQLParser::Primary_key_fieldContext * /*ctx*/) override { }
  virtual void exitPrimary_key_field(SQLParser::Primary_key_fieldContext * /*ctx*/) override { }

  virtual void enterForeign_key_field(SQLParser::Foreign_key_fieldContext * /*ctx*/) override { }
  virtual void exitForeign_key_field(SQLParser::Foreign_key_fieldContext * /*ctx*/) override { }

  virtual void enterType_(SQLParser::Type_Context * /*ctx*/) override { }
  virtual void exitType_(SQLParser::Type_Context * /*ctx*/) override { }

  virtual void enterValue_lists(SQLParser::Value_listsContext * /*ctx*/) override { }
  virtual void exitValue_lists(SQLParser::Value_listsContext * /*ctx*/) override { }

  virtual void enterValue_list(SQLParser::Value_listContext * /*ctx*/) override { }
  virtual void exitValue_list(SQLParser::Value_listContext * /*ctx*/) override { }

  virtual void enterValue(SQLParser::ValueContext * /*ctx*/) override { }
  virtual void exitValue(SQLParser::ValueContext * /*ctx*/) override { }

  virtual void enterWhere_and_clause(SQLParser::Where_and_clauseContext * /*ctx*/) override { }
  virtual void exitWhere_and_clause(SQLParser::Where_and_clauseContext * /*ctx*/) override { }

  virtual void enterWhere_operator_expression(SQLParser::Where_operator_expressionContext * /*ctx*/) override { }
  virtual void exitWhere_operator_expression(SQLParser::Where_operator_expressionContext * /*ctx*/) override { }

  virtual void enterWhere_operator_select(SQLParser::Where_operator_selectContext * /*ctx*/) override { }
  virtual void exitWhere_operator_select(SQLParser::Where_operator_selectContext * /*ctx*/) override { }

  virtual void enterWhere_null(SQLParser::Where_nullContext * /*ctx*/) override { }
  virtual void exitWhere_null(SQLParser::Where_nullContext * /*ctx*/) override { }

  virtual void enterWhere_in_list(SQLParser::Where_in_listContext * /*ctx*/) override { }
  virtual void exitWhere_in_list(SQLParser::Where_in_listContext * /*ctx*/) override { }

  virtual void enterWhere_in_select(SQLParser::Where_in_selectContext * /*ctx*/) override { }
  virtual void exitWhere_in_select(SQLParser::Where_in_selectContext * /*ctx*/) override { }

  virtual void enterWhere_like_string(SQLParser::Where_like_stringContext * /*ctx*/) override { }
  virtual void exitWhere_like_string(SQLParser::Where_like_stringContext * /*ctx*/) override { }

  virtual void enterColumn(SQLParser::ColumnContext * /*ctx*/) override { }
  virtual void exitColumn(SQLParser::ColumnContext * /*ctx*/) override { }

  virtual void enterExpression(SQLParser::ExpressionContext * /*ctx*/) override { }
  virtual void exitExpression(SQLParser::ExpressionContext * /*ctx*/) override { }

  virtual void enterSet_clause(SQLParser::Set_clauseContext * /*ctx*/) override { }
  virtual void exitSet_clause(SQLParser::Set_clauseContext * /*ctx*/) override { }

  virtual void enterSelectors(SQLParser::SelectorsContext * /*ctx*/) override { }
  virtual void exitSelectors(SQLParser::SelectorsContext * /*ctx*/) override { }

  virtual void enterSelector(SQLParser::SelectorContext * /*ctx*/) override { }
  virtual void exitSelector(SQLParser::SelectorContext * /*ctx*/) override { }

  virtual void enterIdentifiers(SQLParser::IdentifiersContext * /*ctx*/) override { }
  virtual void exitIdentifiers(SQLParser::IdentifiersContext * /*ctx*/) override { }

  virtual void enterOperator_(SQLParser::Operator_Context * /*ctx*/) override { }
  virtual void exitOperator_(SQLParser::Operator_Context * /*ctx*/) override { }

  virtual void enterAggregator(SQLParser::AggregatorContext * /*ctx*/) override { }
  virtual void exitAggregator(SQLParser::AggregatorContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

