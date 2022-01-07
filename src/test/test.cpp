#include <iostream>
#include <unistd.h>
#include <string.h>
#include "../Parser-manager/Parser_manager.h"

using namespace std;

int main() {
    Parser_manager p_m;
    p_m.parse("CREATE DATABASE akina;");
    return 0;
}