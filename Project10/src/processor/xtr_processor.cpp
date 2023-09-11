#include "../../include/processor/xtr_processor.h"

ns_xtr::xtr_processor::~xtr_processor()
{
	if (_disasm_tool != nullptr) {
		delete _disasm_tool;
		_disasm_tool = nullptr;
	}
	if (_regex_tool != nullptr) {
		delete _regex_tool;
		_regex_tool = nullptr;
	}
}
