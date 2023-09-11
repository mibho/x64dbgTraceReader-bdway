#pragma once
#include "data/data_manager.h"
#include "processor/xtr_processor.h"
namespace ns_xtr {

	
	class x64dbgTraceReader {
	public:
		x64dbgTraceReader() {};
		x64dbgTraceReader(const char* file_name);
		~x64dbgTraceReader();

		void testfn();
		bool processTraceFile();

		bool processTraceBlocks();

		bool createNewTrace64File(const char* file_name);
		
	private:
		data_manager* _trace_data_manager = nullptr;
		xtr_processor* _xtr_processor = nullptr;
	};



}