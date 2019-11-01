#include <unistd.h>
#include <iostream>
using namespace std;

#include "CCommandShell.h"

CMD_STATUS fun1(char* const argv[]) {
    cout << "inside fun1: " << argv[1] << endl;
    return CMD_OK;
}

CMD_STATUS fun2(char* const argv[]) {
    sleep(2);
    cout << "fun2: " << argv[1] << " " << argv[2] << endl;
    return CMD_ERROR;
}

int main() {    
    CCommandShell a1;

    a1.addCommand("fun1", fun1, false);
    a1.addCommand("fun2", fun2, true);

    a1.shellInit();
    a1.runShell();

    return 0;
}
