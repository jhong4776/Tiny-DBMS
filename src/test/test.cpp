#include <iostream>
#include <unistd.h>
#include <string.h>
#include "../Parser-manager/Parser_manager.h"

using namespace std;

int main() {
    Parser_manager p_m;
    MyVisitor* lbase = new MyVisitor();
    string input;
    string temp;
    string colom = ";";
    while(true) {
        input.clear();
        int i = 0;
        while(true) {
            temp.clear();
            if (i == 0)
                cout << "mysql> ";
            else
                cout << "    -> ";
            getline(cin,temp);

            if (temp == "quit")
                break;

            input = input + temp + " ";
            if (temp[temp.size() - 1] == colom[0])
                break;
            i++;
        }
        if (input == "quit")
            break;
        
        cout << input << endl;
        p_m.parse(input, lbase);
    }

    lbase->~MyVisitor();
    return 0;
}