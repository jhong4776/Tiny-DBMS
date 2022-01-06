#include "Header_manager.h"
#include "../Header_define.h"

Header_manager::Header_manager() {}

Header_manager::~Header_manager() {}

void Header_manager::read_from_file(const char* filename) {

}

void Header_manager::input_header_info() {
    Index_Header_info* h_i = (Index_Header_info*)file_input;
    header_info.pageID = h_i->pageID;
    header_info.nextPage = h_i->nextPage;
    header_info.prevPage = h_i->prevPage;
    header_info.objID = h_i->objID;
    header_info.lsn = h_i->lsn;
    header_info.slotCnt = h_i->slotCnt;    
    header_info.level = h_i->level;
    header_info.indexID = h_i->indexID;
    header_info.freeData = h_i->freeData;
    header_info.pminlen = h_i->pminlen;
    header_info.freeCnt = h_i->freeCnt;
    header_info.reservedCnt = h_i->reservedCnt; 
    header_info.xactresenved = h_i->xactresenved;
}

void Header_manager::header_initialize(int i_page_ID, int i_nextPage, int i_prevPage, 
    int i_objID, int i_level, int i_index_ID, int i_pminlen) {
    header_info.pageID = i_page_ID;
    header_info.nextPage = i_nextPage;
    header_info.prevPage = i_prevPage;
    header_info.objID = i_objID;
    header_info.lsn = 0;
    header_info.slotCnt = 0;
    header_info.level = i_level;
    header_info.indexID = i_index_ID;
    header_info.freeData = sizeof(Index_Header_info);
    header_info.pminlen = i_pminlen;
    header_info.freeCnt = PAGE_SIZE - sizeof(Index_Header_info);
    header_info.reservedCnt = 0;
    header_info.xactresenved = 0;
}

Index_Header_info Header_manager::get_header_info() {
    return header_info;
}