#include "../../../include/processor/capstone/capstone_engine.h"

using namespace ns_xtr;

const BYTE& ns_xtr::capstone_engine::getOperandCount() const
{
	if (_instructions != nullptr) {
		std::cout << "op count: " << static_cast<int>(_instructions->detail->x86.op_count) << std::endl;

		return _instructions->detail->x86.op_count;
	}

	return X86_REG_INVALID;
}

const BYTE& ns_xtr::capstone_engine::getOperandType() const
{
	if (_instructions != nullptr) {
		std::cout << "op type: {}\n" << _instructions->detail->x86.operands->type << std::endl;
		return _instructions->detail->x86.operands->type;
	}
}

cs_x86_op& ns_xtr::capstone_engine::getNthOperand(uint_8 index) 
{
	if (_instructions != nullptr) {

		return _instructions->detail->x86.operands[index];
	}

}

void ns_xtr::capstone_engine::disasm(uint64_t ins_addr, const BYTE* code, size_t code_size, size_t limit)
{
	_ins_count = _disassemble(ins_addr, code, code_size, limit);

	std::cout << "errno: " << cs_errno(_capstone_handle) << std::endl;
	std::cout << "ins count: " << _ins_count << std::endl;
	if (_instructions == nullptr) {
		std::cout << "instructions is nullptr" << std::endl;
	}
	else {
		std::cout << "access: " << static_cast<int>(_instructions->detail->x86.operands->access) << std::endl;
		std::cout << "instr: " << _instructions->mnemonic << " " << _instructions->op_str << std::endl;
		getOperandCount();
		getOperandType();
	}
	_csdata_reset();
}

// clear any csh resources (if applicable) and free handle
void ns_xtr::capstone_engine::cleanup()
{
	_csdata_reset();

	if (_open_success)
		cs_close(&_capstone_handle);
}

ns_xtr::capstone_engine::~capstone_engine()
{
#ifdef DEBUG_ON

#endif

	cleanup();
}

void ns_xtr::capstone_engine::_csdata_reset()
{
	if (_open_success) {

		if (_instructions != nullptr) {

			if (_ins_count > 0) {
				cs_free(_instructions, _ins_count);
				_ins_count = 0;
			}

			_instructions = nullptr;
		}

	}
}

size_t ns_xtr::capstone_engine::_disassemble(uint64_t ins_addr, const BYTE* code, size_t code_size, size_t limit=0)
{
	return cs_disasm(_capstone_handle, reinterpret_cast<const uint8_t*>(code), code_size, ins_addr, limit, &_instructions);
}

void ns_xtr::capstone_engine::init_capstone(cs_mode hw_mode)
{
 

	// cs_open returns CS_ERR_OK on success; handle successfully initialized if _open_success == CS_ERR_OK.
	_open_success = cs_open(CS_ARCH_X86, hw_mode, &_capstone_handle) == CS_ERR_OK ? true : false;

	std::cout << "errno: " << cs_strerror(cs_errno(_capstone_handle)) << std::endl;
	if (!_open_success) {

		auto cs_error = cs_errno(_capstone_handle);
		std::cout << "[!] - Error setting up Capstone: " << cs_strerror(cs_error) << std::endl << "Exiting..." << std::endl;
		return;
	}

	set_capstone_option(CS_OPT_DETAIL, CS_OPT_ON);

}

void ns_xtr::capstone_engine::set_capstone_option(cs_opt_type option, cs_opt_value enabled)
{
	if (!_open_success)
		return;

	switch (option) {
	case CS_OPT_DETAIL:
		cs_option(_capstone_handle, CS_OPT_DETAIL, enabled);
		break;
	case CS_OPT_SKIPDATA:
		cs_option(_capstone_handle, CS_OPT_SKIPDATA, enabled);
		break;
	default:

		break;
	}
}
