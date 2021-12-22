#ifndef HD
#define HD

#define PAGE_SIZE 8192
#define LEAF_NODE 0
#define INDEX_DATA_PAGE 0
#define INDEX_PAGE 1
#define ROOT_PAGEID 0
#define MAX_RECORD 1024

struct Index_Header_info
{
    int pageID; // 该页面的文件编号
    int nextPage; // 下一个页面的文件编号
    int prevPage; // 前一个页面编号
    int objID; // 该页面所属对象的ID
    int lsn;   // 用于更改和更新该页面的日志序号值
    int slotCnt; // 该页面中所用的总行数（记录条数）
    int level; // 级别：叶节点为0，依次向上排
    int indexID; // 该页面的索引ID（数据页面通常为0）
    int freeData; // 该页面第一个自由空间的字节偏移量
    int pminlen;  // 行的固定长度部分的字节数
    int freeCnt; // 页面中自由字节数
    int reservedCnt; // 由所有事务预留的字节数
    int xactresenved; // 由最近启动的事务预留的字节数
    int nullmap; // 位图(0表示该行为空) #TODO：nullmap暂定为32位（一页最多放32条数据）
}; 

#endif