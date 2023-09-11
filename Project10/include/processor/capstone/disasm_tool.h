#pragma once

#include "capstone_engine.h"

namespace ns_xtr {

	using namespace ns_dtypes;

	namespace misc {
	
	}

	class disasm_tool {
	public:

		disasm_tool();
		~disasm_tool();

		void disasm(uint64_t ins_addr, const BYTE* code, size_t code_size, size_t limit);
		capstone_engine* _capstone = nullptr;
	private:
		

	};
}