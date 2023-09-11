#include "../include/x64dbgTraceReader.h"

ns_xtr::x64dbgTraceReader::x64dbgTraceReader(const char* file_name)
{
	if (_trace_data_manager == nullptr) {
		_trace_data_manager = new data_manager(file_name);
	}

	if (_xtr_processor == nullptr) {
		_xtr_processor = new xtr_processor();
	}
 
}

ns_xtr::x64dbgTraceReader::~x64dbgTraceReader()
{
	if (_trace_data_manager != nullptr) {
		delete _trace_data_manager;
		_trace_data_manager = nullptr;
	}
 

	if (_xtr_processor != nullptr) {
		delete _xtr_processor;
		_xtr_processor = nullptr;
	}
 
}

void ns_xtr::x64dbgTraceReader::testfn()
{
	_trace_data_manager->testfn();
}

bool ns_xtr::x64dbgTraceReader::processTraceFile()
{
	return _trace_data_manager->confirmTrace64File();
}


bool ns_xtr::x64dbgTraceReader::processTraceBlocks()
{
	_trace_data_manager->parseToEnd();
	return true; //fix
}

bool ns_xtr::x64dbgTraceReader::createNewTrace64File(const char* file_name)
{
	return _trace_data_manager->xtrToTrace64File(file_name);
}
