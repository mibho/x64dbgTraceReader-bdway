#pragma once
#include "trace_instruction.h"
namespace ns_xtr {

	using namespace ns_dtypes;

	struct xtr_data {

		xtr_data(std::vector<REGISTERCONTEXT> affected_regs, 
				registers regs, 
				std::vector<BYTE> opcodes, 
				std::vector<mem_access> memdata) : _ins_data(regs, opcodes, memdata) {_affected_regs = affected_regs;};

		std::vector<REGISTERCONTEXT> _affected_regs;
		traceinstruction _ins_data;

		/*
		void outRIP() { std::cout << "RIP: " << _ins_data.getRIP() << " " << std::hex << std::endl; }
		void output() {
			std::cout << "---------------------------------------------------------------------------------\n" << std::endl;
			std::cout << "affected regs len: " << _affected_regs.size() << std::endl;
			std::cout << "registers values: " << std::endl;
			
			std::cout << std::hex << "RAX: " << _ins_data.getRegister(RAX) << std::endl;
			std::cout << "RCX: " << _ins_data.getRegister(RCX) << std::endl;
			std::cout << "RDX: " << _ins_data.getRegister(RDX) << std::endl;
			std::cout << "RBX: " << _ins_data.getRegister(RBX) << std::endl;
			std::cout << "RSP: " << _ins_data.getRegister(RSP) << std::endl;
			std::cout << "RBP: " << _ins_data.getRegister(RBP) << std::endl;
			std::cout << "RSI: " << _ins_data.getRegister(RSI) << std::endl;
			std::cout << "RDI: " << _ins_data.getRegister(RDI) << std::endl;
			std::cout << "R8: " << _ins_data.getRegister(R8) << std::endl;
			std::cout << "R9: " << _ins_data.getRegister(R9) << std::endl;
			std::cout << "R10: " << _ins_data.getRegister(R10) << std::endl;
			std::cout << "RIP: " << _ins_data.getRegister(RIP) << std::endl;

			std::cout << "---------------------------------------------------------------------------------\n" << std::endl;
		}
		*/
	};

	// _history[threadID] = lookup table
	//	
	class xtr_tracedata {
	public:
		xtr_tracedata();
		~xtr_tracedata();

		void someInfo(); // useless
		
		traceinstruction& getInstructionFromNthEntry(DWORD Nth_entry);
		bool doesNthEntryExist(DWORD Nth_entry);
		xtr_data& getNthEntry(DWORD Nth_entry);
		bool updateNthEntry(xtr_data& Nth_ins) {};
		bool removeNthEntry(DWORD Nth_entry);
		bool addNthEntry(DWORD Nth_entry) {};
		umap<DWORD, xtr_data>& getTraceEntryForThreadID(DWORD threadID);
		bool createThreadIDEntry(DWORD threadID);

		bool isEncounteredThreadID(DWORD threadID) { return _history.contains(threadID); };
		
		bool checkIfSameThreadID(DWORD threadID) { return (_currTID == threadID); };

		bool addInstructionEntry(xtr_data ins_info);
		bool addInstructionEntryForThreadID(DWORD threadID, xtr_data ins_info);
		const DWORD& getInstructionCount() const { return _history.at(_currTID).size(); };

		// get # of threads that had their instructions recorded in curr trace file.
		const DWORD& getThreadIDCount() const { return _history.size(); };

		const DWORD& size() const { return _history.size(); };
		const DWORD& getCurrentThreadID() const { return _currTID; };

		
		const DWORD& getInstructionCountForThreadID(DWORD threadID);

		const DWORD getThreadIDForInstruction(DWORD instructionID);
		


		/*
		* fns solely for formatting. can ignore
		*/
		const uint_8& _TRACE64FORMAT_getNthInstructionMemAccessesCount(DWORD instructionID) { return getInstructionFromNthEntry(instructionID).getMemAccessCount(); };
		const uint_8 _TRACE64FORMAT_getNthInstructionRegChangesCount(DWORD instructionID) 
		{ 
			uint_8 amt = getNthEntry(instructionID)._affected_regs.size();
			return (amt == misc::XTR_ALL_REGISTERS_SAVED) ? misc::X64DBG_ALL_REGISTERS_SAVED : amt; // if len(affected_regs) == 30, return 172 aka 0xAC. otherwise ret the size from fn
		};
		const DWORD& _TRACE64FORMAT_getNthInstructionThreadID(DWORD instructionID) 
		{
			return getThreadIDForInstruction(instructionID);
		};

		const std::vector<uint_8>& _TRACE64FORMAT_getNthInstructionOpcodes(DWORD instructionID) { return getInstructionFromNthEntry(instructionID).getOpcodes()	; };
		const uint_8& _TRACE64FORMAT_getNthInstructionOpcodesLen(DWORD instructionID) { return getInstructionFromNthEntry(instructionID).getOpcodes().size(); };

		const std::vector<REGISTERCONTEXT> _TRACE64FORMAT_getNthInstructionRegChangesIndices(DWORD instructionID) 
		{
			
			xtr_data& nth_entry = getNthEntry(instructionID);
			std::vector<REGISTERCONTEXT> changed = nth_entry._affected_regs;
			
			return changed;
		}

		const std::vector<QWORD> _TRACE64FORMAT_getNthInstructionRegChangesData(DWORD instructionID)
		{
			xtr_data& nth_entry = getNthEntry(instructionID);
			std::vector<REGISTERCONTEXT> changed = nth_entry._affected_regs;
			std::vector<QWORD> registerdata;

			for (int i = 0; i < changed.size(); i++) {
				registerdata.push_back(nth_entry._ins_data.getRegister(changed[i]));
			}

			return registerdata;

		}

		const std::vector<mem_access> _TRACE64FORMAT_getNthInstructionMemAccessesData(DWORD instructionID) {

			traceinstruction nth_ins = getInstructionFromNthEntry(instructionID);
			const std::vector<mem_access> memdata = nth_ins.getMemAccessObject();

			return memdata;
		};

	private:
		//umap<ThreadID, umap<execution #, xtr_data>> - could've used umap<DWORD, vector<xtr_data>> but removing elements affects original order
		umap<DWORD, umap<DWORD, xtr_data>> _history;
		DWORD _currTID = 0;

	};
}