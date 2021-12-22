typedef int RC;

class IndexScan {
    public:
        IndexScan(); // 构造函数
        ~IndexScan(); // 析构函数
        RC GetNextEntry (int &rid); // 类似于Python中的迭代器，不断调用获取下一个记录的位置
};

class IndexHandler {
    public:
        IndexHandler();
        ~IndexHandler();
        RC CreateIndex (const char *fileName); // 创建索引
        RC DestroyIndex (const char * fileName); // 删除索引
        RC OpenInex (const char *fileName); // 通过缓存管理模块打开索引，并获得其句柄
        RC CloseIndex (); // 关闭索引
        RC Seainth (int lowerBound, int upperBound, IndexScan &indexScan); // 查找某个范围内的记录，通过迭代器访问
        RC DeleteRecord (int lowerBound, int upperBound); // 删除某个范围内的记录
        RC InsertRecoder (int key, const int &rid);
        RC UpdateRecoder (int oldKey, const int &oldint, int newKey, const int &newint); // 更新特定记录的关键字或位置
};