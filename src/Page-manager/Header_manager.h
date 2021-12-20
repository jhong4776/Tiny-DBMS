/**
 * 用于解析非系统页面头格式
 */
#ifndef HM
#define HM

#include "../Data_define.h"
#include "../Header_define.h"

class Header_manager {
    uint8_t file_input[PAGE_SIZE >> 2];
    Index_Header_info header_info;
public:
    Header_manager();
    ~Header_manager();
    void read_from_file(const char* filename);
    void input_header_info();
    void header_initialize(int i_page_ID, int i_nextPage, int i_prevPage, 
                           int i_objID, int i_level, int i_index_ID,
                           int i_pminlen);
    Index_Header_info get_header_info();
};

#endif