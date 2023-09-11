#pragma once

/*
* get needed data regarding instructions so that writing actual analyses is much easier
* 
* 
* 
* 
* 
* 
* 
*/
#include <capstone/capstone.h>
#include "../../data/data_formats.h"

//#include "fmt-10.1.1/include/fmt/core.h"
//#include "fmt-10.1.1/include/fmt/format.h"
//#include "fmt-10.1.1/include/fmt/format-inl.h"
//#include "fmt-10.1.1/src/format.cc"
namespace ns_xtr {

	using namespace ns_dtypes;

	namespace misc {
		constexpr size_t NONE = 0;
	}

	class capstone_engine {
	public:

		capstone_engine() {
			std::cout << "calling capstone_engine constructor" << std::endl; 
			init_capstone();
		};
		capstone_engine(cs_mode hw_mode) { init_capstone(hw_mode); }
		~capstone_engine();

		// default setting = x86-64 
		

		const BYTE& getOperandCount() const;
		const BYTE& getOperandType() const;
		cs_x86_op& getNthOperand(uint_8 index);

		void disasm(uint64_t ins_addr, const BYTE* code, size_t code_size, size_t limit = misc::NONE);
		
		

	private:
		csh _capstone_handle;
		cs_insn* _instructions = nullptr;
		size_t _ins_count = 0;
		bool _open_success = false;

		void _csdata_reset();
		size_t _disassemble(uint64_t ins_addr, const BYTE* code, size_t code_size, size_t limit);


		// see constructor
		void init_capstone(cs_mode hw_mode = CS_MODE_64);
		void set_capstone_option(cs_opt_type option, cs_opt_value enabled);

		// see destructor
		void cleanup();
		

	};
}