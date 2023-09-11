#pragma once
#include "capstone/disasm_tool.h"
#include "regex/regex_tool.h"
#include "../data/data_objects/xtr_tracedata/xtr_tracedata.h"

namespace ns_xtr {


	class xtr_processor : public disasm_tool, public regex_tool {
	public:
		xtr_processor() : disasm_tool(), regex_tool() {};
		~xtr_processor();

		void doStuff(xtr_tracedata* xtr_trace);
	private:
		disasm_tool* _disasm_tool = nullptr;
		regex_tool* _regex_tool = nullptr;
	};



}