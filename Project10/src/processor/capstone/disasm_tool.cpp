
#include "../../../include/processor/capstone/disasm_tool.h"

ns_xtr::disasm_tool::disasm_tool()
{
	if (_capstone == nullptr) {
		_capstone = new capstone_engine(CS_MODE_64);	// default x86-64
		std::cout << "created new capstone_engine obj" << std::endl;
	}
}

ns_xtr::disasm_tool::~disasm_tool()
{
	if (_capstone != nullptr) {
		delete _capstone;
		_capstone = nullptr;
	}
}

void ns_xtr::disasm_tool::disasm(uint64_t ins_addr, const BYTE* code, size_t code_size, size_t limit)
{
	std::cout << "provided addr: " << ins_addr << std::endl;
	std::cout << "code: " << code << std::endl;
	std::cout << "code size: " << code_size << std::endl;
	_capstone->disasm(ins_addr, code, code_size, limit);
}
