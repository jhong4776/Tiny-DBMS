#include<stdint.h>

#define MAX_COLUMN_SIZE 32
// both table name and column name
#define MAX_NAME_LEN 128
#define MAX_DATA_SIZE 3000
#define MAX_CHECK 16
#define MAX_FOREIGN_KEY 24


extern bool initMode;

struct Check {
    int col;
    int offset;
    OpType op;
    RelType rel;
};

struct ForeignKey {
    unsigned int col;
    unsigned int foreign_table_id;
    unsigned int foreign_col;
};

enum ColumnType {
    CT_INT, CT_VARCHAR, CT_FLOAT, CT_DATE
};

struct TableHead {
    int8_t columnTot, primaryCount, checkTot, foreignKeyTot;
    int pageTot, recordByte, dataArrUsed;
    unsigned int nextAvail, notNull, hasIndex, isPrimary;

    char columnName[MAX_COLUMN_SIZE][MAX_NAME_LEN];
    int columnOffset[MAX_COLUMN_SIZE];
    ColumnType columnType[MAX_COLUMN_SIZE];
    int columnLen[MAX_COLUMN_SIZE];
    int defaultOffset[MAX_COLUMN_SIZE];
    Check checkList[MAX_CHECK];
    ForeignKey foreignKeyList[MAX_FOREIGN_KEY];
    char dataArr[MAX_DATA_SIZE];
};

class RecordHandler {
    TableHead head;
    bool ready;
    int fileID, permID;
    std::string fileName;
  public:
       RecordHandler  ();                     // 构造函数
       ~RecordHandler ();                     // 析构函数
    int CreateFile     (const char *fileName); // 创建文件
    int DestroyFile    (const char *fileName); // 删除文件
    int OpenFile       (const char *fileName); // 通过缓存管理模块打开文件，并获取其句柄
    int CloseFile      ();                     // 关闭fileID对应文件
    
    int GetRecord      (const int &rid, char *&pData);            
                                              // 通过页号和槽号访问记录后，相应字节序列可以通过pData访问
    int DeleteRecord   (const int &rid);       // 删除特定记录

    void allocPage();
    int InsertRecord   (const int &rid, const char *pData); 
                                              // 将字节序列pData插入特定位置
    int UpdateRecord   (const int &rid, const char *pData);
                                              // 将特定位置记录更新为字节序列pData
    std::string    setTempRecord(int col, const char *data);
    void  initTempRecord();

};