# x64dbgTraceReader
[WIP] tool to analyze/manipulate a .trace64 file and output the data back into a working .trace64 file or w/e format desired

ideally 'user' only needs to deal with the following:

data_manager.h/cpp - fns to manage data 

xtr_processor.h/cpp - analysis functions 

x64dbgTraceReader.h/cpp - access data via data_manager and analyze via xtr_processor

main.cpp - init x64dbgTraceReader and run functions
