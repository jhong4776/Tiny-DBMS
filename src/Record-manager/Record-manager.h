//实现数据文件的创建与删除（由表的定义与删除引起）、记录的插入、删除、查找操作，并对外提供相应的接口。
//其中记录的查找操作要求能够支持不带条件的查找和带一个条件的查找（包括等值查找、不等值查找和区间查找）


#ifndef RecordManager_hpp
#define RecordManager_hpp

#include<iostream>
#include<string>
#include <vector>
#include"BufPageManager.h"
#include "exception.h"

class RecordManager {
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
    int InsertRecord   (const int &rid, const char *pData); 
                                              // 将字节序列pData插入特定位置
    int UpdateRecord   (const int &rid, const char *pData);
                                              // 将特定位置记录更新为字节序列pData
};

#endif // !record_hpp