#ifndef MYVISITOR
#define MYVISITOR
#include "SQLBaseVisitor.h"
#include "SQLLexer.h"
#include "SQLParser.h"
#include <string>
#include <vector>

struct DataList {
  std::string name; // 变量名
  std::string type; // 变量类型
  int len; // 长度
  std::string str_val; // 字符值
  int int_val; //整型值
  bool bool_val; // 布尔值
  float float_val; // 小数值

  bool pri_key; // 是否是主键
  bool for_key; //是否是外键
};

class MyVisitor : public SQLBaseVisitor{
  std::string command; // create drop show use load dump describe insert delete update select alter
  std::string object; // database table index
  std::string name; // name of database, table, variable
  std::string type; // integer string float null
  std::string relation; // > < >= <= != ==
  std::string aggregator; // count average max min sum
  std::string filename;

  std::vector<DataList> data_list;
  public:
  int visit(SQLParser::ProgramContext* iTree);
  
  virtual antlrcpp::Any visitProgram(SQLParser::ProgramContext *ctx);

  virtual antlrcpp::Any visitStatement(SQLParser::StatementContext *ctx);

  virtual antlrcpp::Any visitCreate_db(SQLParser::Create_dbContext *ctx);

  virtual antlrcpp::Any visitDrop_db(SQLParser::Drop_dbContext *ctx);

  virtual antlrcpp::Any visitShow_dbs(SQLParser::Show_dbsContext *ctx);

  virtual antlrcpp::Any visitUse_db(SQLParser::Use_dbContext *ctx);

  virtual antlrcpp::Any visitShow_tables(SQLParser::Show_tablesContext *ctx);

  virtual antlrcpp::Any visitShow_indexes(SQLParser::Show_indexesContext *ctx);

  virtual antlrcpp::Any visitLoad_data(SQLParser::Load_dataContext *ctx);

  virtual antlrcpp::Any visitDump_data(SQLParser::Dump_dataContext *ctx);

  virtual antlrcpp::Any visitCreate_table(SQLParser::Create_tableContext *ctx);

  virtual antlrcpp::Any visitDrop_table(SQLParser::Drop_tableContext *ctx);

  virtual antlrcpp::Any visitDescribe_table(SQLParser::Describe_tableContext *ctx);

  virtual antlrcpp::Any visitInsert_into_table(SQLParser::Insert_into_tableContext *ctx);

  virtual antlrcpp::Any visitDelete_from_table(SQLParser::Delete_from_tableContext *ctx);

  virtual antlrcpp::Any visitUpdate_table(SQLParser::Update_tableContext *ctx);

  virtual antlrcpp::Any visitSelect_table_(SQLParser::Select_table_Context *ctx);

  virtual antlrcpp::Any visitSelect_table(SQLParser::Select_tableContext *ctx);

  virtual antlrcpp::Any visitAlter_add_index(SQLParser::Alter_add_indexContext *ctx);

  virtual antlrcpp::Any visitAlter_drop_index(SQLParser::Alter_drop_indexContext *ctx);

  virtual antlrcpp::Any visitAlter_table_drop_pk(SQLParser::Alter_table_drop_pkContext *ctx);

  virtual antlrcpp::Any visitAlter_table_drop_foreign_key(SQLParser::Alter_table_drop_foreign_keyContext *ctx);

  virtual antlrcpp::Any visitAlter_table_add_pk(SQLParser::Alter_table_add_pkContext *ctx);

  virtual antlrcpp::Any visitAlter_table_add_foreign_key(SQLParser::Alter_table_add_foreign_keyContext *ctx);

  virtual antlrcpp::Any visitAlter_table_add_unique(SQLParser::Alter_table_add_uniqueContext *ctx);

  virtual antlrcpp::Any visitField_list(SQLParser::Field_listContext *ctx);

  virtual antlrcpp::Any visitNormal_field(SQLParser::Normal_fieldContext *ctx);

  virtual antlrcpp::Any visitPrimary_key_field(SQLParser::Primary_key_fieldContext *ctx);

  virtual antlrcpp::Any visitForeign_key_field(SQLParser::Foreign_key_fieldContext *ctx);

  virtual antlrcpp::Any visitType_(SQLParser::Type_Context *ctx);

  virtual antlrcpp::Any visitValue_lists(SQLParser::Value_listsContext *ctx);

  virtual antlrcpp::Any visitValue_list(SQLParser::Value_listContext *ctx);

  virtual antlrcpp::Any visitValue(SQLParser::ValueContext *ctx);

  virtual antlrcpp::Any visitWhere_and_clause(SQLParser::Where_and_clauseContext *ctx);

  virtual antlrcpp::Any visitWhere_operator_expression(SQLParser::Where_operator_expressionContext *ctx);

  virtual antlrcpp::Any visitWhere_operator_select(SQLParser::Where_operator_selectContext *ctx);

  virtual antlrcpp::Any visitWhere_null(SQLParser::Where_nullContext *ctx);

  virtual antlrcpp::Any visitWhere_in_list(SQLParser::Where_in_listContext *ctx);

  virtual antlrcpp::Any visitWhere_in_select(SQLParser::Where_in_selectContext *ctx);

  virtual antlrcpp::Any visitWhere_like_string(SQLParser::Where_like_stringContext *ctx);

  virtual antlrcpp::Any visitColumn(SQLParser::ColumnContext *ctx);

  virtual antlrcpp::Any visitExpression(SQLParser::ExpressionContext *ctx);

  virtual antlrcpp::Any visitSet_clause(SQLParser::Set_clauseContext *ctx);

  virtual antlrcpp::Any visitSelectors(SQLParser::SelectorsContext *ctx);

  virtual antlrcpp::Any visitSelector(SQLParser::SelectorContext *ctx);

  virtual antlrcpp::Any visitIdentifiers(SQLParser::IdentifiersContext *ctx);

  virtual antlrcpp::Any visitOperator_(SQLParser::Operator_Context *ctx);

  virtual antlrcpp::Any visitAggregator(SQLParser::AggregatorContext *ctx);
};

#endif