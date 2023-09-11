#include "../../../../include/data/data_objects/xtr_tracedata/trace_instruction.h"

ns_xtr::traceinstruction::traceinstruction(registers regs, std::vector<BYTE> opcodes, std::vector<mem_access> memdata)
{
	_regs = regs;
	_opcodes = opcodes;
	_memdata = memdata;
}
