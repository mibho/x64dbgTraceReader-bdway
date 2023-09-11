#include <iostream>
//#include "file_handler.h"
#include "../include/x64dbgTraceReader.h"
#include <Windows.h>

int main(int argc, char* argv[])
{
 
    if (argc != 2) {
        std::cout << "Incorrect number of arguments provided.\nExample usage:\n " << std::endl;
        std::cout << "\t" << argv[0] << " <x64dbg trace file name>" << std::endl;
        return -1;
    }
    
    char* file_name = argv[1];
    DWORD file_size = 0;
    DWORD bytes_read = 0;
    ns_xtr::x64dbgTraceReader xtest = ns_xtr::x64dbgTraceReader(file_name);
    xtest.processTraceFile();
    xtest.processTraceBlocks();
    xtest.testfn();
    xtest.createNewTrace64File("name.trace64");
 



    return 0;
}
