#include <iostream>
#include "Index-manager/BplusTree.h"

int main() {
    BplusTree tree1;
    tree1.get_tree_root(1);
    std::cout << "output" << std::endl;
}