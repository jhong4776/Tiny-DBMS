#include <iostream>
#include <unistd.h>
#include <string.h>
#include "Index_manager.h"

using namespace std;

int main() {
    IndexHandler index_handler;
    index_handler.CreateIndex("test");
    for (int i = 0; i < 10; i++){
        index_handler.InsertRecoder(i, i*100+9);
    }
    index_handler.WriteBack();
}