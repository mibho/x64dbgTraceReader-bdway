#pragma once

#include "../data/data_objects/xtr_tracedata/xtr_tracedata.h"
#include "../data/data_objects/x64dbg_tracedata/x64dbg_tracedata.h"
#include <utility>

namespace ns_xtr {

	using namespace ns_dtypes;

	namespace misc {
	
	}
	
	class data_manager {
	public:

		void testfn();

		data_manager(const char* file_name);
		~data_manager();

		// checks for 'TRAC' magic bytes and JSON blob data associated w/ trace64 file
		bool confirmTrace64File();

		// 
		void parseTraceBlocks();
		
		void parseToEnd();

		std::string rcToEnglish(uint_8 index);

		// process buf w/ trace64 data 
		bool trace64ToXTR();
		// convert analyzed data into readable trace64 file 
		bool xtrToTrace64File(const char* filename);
		

		// ( •̀ᴗ•́ )
		// input: list of vals for what regs changed. output: corresponding reg, new val
		std::vector<uint_8> getAbsoRelativeForm(const std::vector<REGISTERCONTEXT>& changelist) {
			std::vector<uint_8> absorelative;
			uint_8 absorelativeindex = 0;
			for (int i = 0; i < changelist.size(); i++) {

				// i == 0? ret 1st element. else: ret (curr element - prev element - 1)
				absorelativeindex = (i == 0) ? static_cast<uint_8>(changelist[0]) : (static_cast<uint_8>(changelist[i]) - static_cast<uint_8>(changelist[i - 1]) - 1);
				absorelative.push_back(absorelativeindex);
				}
		
			return absorelative;
		}
		// convert x64dbg 'absorelative' format 
		std::vector<REGISTERCONTEXT> getRCForm(const std::vector<BYTE>& changelist) {
			BYTE absorelativeindex = 0;
			BYTE prev_val = 0;
			std::vector<REGISTERCONTEXT> affected_list;

			for (int i = 0; i < changelist.size(); i++) {
				absorelativeindex = (i == 0) ? changelist[0] : prev_val + changelist[i] + 1;
				prev_val = absorelativeindex;

				switch (absorelativeindex) {
					case 0:  affected_list.push_back(RAX);break;
					case 1:	 affected_list.push_back(RCX);break;
					case 2:	 affected_list.push_back(RDX);break;
					case 3:	 affected_list.push_back(RBX);break;
					case 4:	 affected_list.push_back(RSP);break;
					case 5:	 affected_list.push_back(RBP);break;
					case 6:  affected_list.push_back(RSI);break;
					case 7:	 affected_list.push_back(RDI);break;
					case 8:	 affected_list.push_back(R8); break;
					case 9:	 affected_list.push_back(R9); break;
					case 10: affected_list.push_back(R10);break;
					case 11: affected_list.push_back(R11);break;
					case 12: affected_list.push_back(R12);break;
					case 13: affected_list.push_back(R13);break;
					case 14: affected_list.push_back(R14);break;
					case 15: affected_list.push_back(R15);break;
					case 16: affected_list.push_back(RIP);break;
					case 17: affected_list.push_back(RFLAGS);break;
					case 18: affected_list.push_back(GS);break;
					case 19: affected_list.push_back(FS);break;
					case 20: affected_list.push_back(ES);break;
					case 21: affected_list.push_back(DS);break;
					case 22: affected_list.push_back(CS);break;
					case 23: affected_list.push_back(SS);break;
					case 24: affected_list.push_back(DR0);break;
					case 25: affected_list.push_back(DR1);break;
					case 26: affected_list.push_back(DR2);break;
					case 27: affected_list.push_back(DR3);break;
					case 28: affected_list.push_back(DR6);break;
					case 29: affected_list.push_back(DR7);break;
					default:
						break;
					}
				}
			

			return affected_list;
		};

		
	private:
		x64dbg_tracedata* _provided_file = nullptr;	// contains INPUT trace data. NOT YET MAPPED TO USABLE FORMAT (must call trace64ToXTR() )
		xtr_tracedata* _xtr_trace = nullptr;
		DWORD _prev_TID{ 0 };

		DWORD _trace_entry{ 0 };

		

		bool _createNewTrace64File(const char* filename);
		bool _copyBlobData();
		bool _setupXTRTracedata();

		// lol
		std::vector<BYTE> _newfile_databuf;
		bool _appendToNewFileBuf(BYTE* srcval, QWORD valsize);
		bool _appendToNewFileBuf(BYTE srcval);
		bool _appendToNewFileBuf(DWORD srcval);
		bool _appendToNewFileBuf(QWORD srcval);
		void _clearNewFileBuf();
		HANDLE _newfile = nullptr;


		registers* _logged_regs = nullptr;
		bool _isSetUp = false;
		bool _preserveData = false;

		std::string _newblob;
		bool _processJSONBlob();
		bool _isMagicBytesSigFound();



		xtr_data _TRACE64FORMAT_convertToXTREntry(x64dbg_trace_block& entry);

		/*
		* fns to convert .trace64 data into data usable by x64dbgTraceReader 
		*/
		void _TRACE64FORMAT_BYTE1_readBlockType(x64dbg_trace_block& entry);
		void _TRACE64FORMAT_BYTE2_readRegisterChanges(x64dbg_trace_block& entry);
		void _TRACE64FORMAT_BYTE3_readMemoryAccesses(x64dbg_trace_block& entry);
		void _TRACE64FORMAT_BYTE4_readBlockFlagsAndOpcodeSize(x64dbg_trace_block& entry);
		void _TRACE64FORMAT_BYTE5_readRegisterChangePosition(x64dbg_trace_block& entry);
		void _TRACE64FORMAT_QWORD6_readRegisterChangeNewData(x64dbg_trace_block& entry);
		void _TRACE64FORMAT_BYTE7_readMemoryAccessFlags(x64dbg_trace_block& entry);
		void _TRACE64FORMAT_QWORD8_readMemoryAccessAddresses(x64dbg_trace_block& entry);
		void _TRACE64FORMAT_QWORD9_readMemoryAccessOldData(x64dbg_trace_block& entry);
		void _TRACE64FORMAT_QWORD10_readMemoryAccessNewData(x64dbg_trace_block& entry);


		/*
		* fns to convert x64dbgTraceReader into data usable by .trace64 files
		*/
		 DWORD _TRACE64FORMAT_writeAsTrace64Entry(DWORD Nth_Entry);

		 void _TRACE64FORMAT_HEADER1_writeMagicBytes();
		 void _TRACE64FORMAT_HEADER2_writeJsonData();

		 void _TRACE64FORMAT_BYTE1_writeBlockType(DWORD Nth_Entry);
		 void _TRACE64FORMAT_BYTE2_writeRegisterChanges(DWORD Nth_Entry);
		 void _TRACE64FORMAT_BYTE3_writeMemoryAccesses(DWORD Nth_Entry);
		 void _TRACE64FORMAT_BYTE4_writeBlockFlagsAndOpcodeSize(DWORD Nth_Entry);
		 void _TRACE64FORMAT_BYTE5_writeRegisterChangePosition(DWORD Nth_Entry);
		 void _TRACE64FORMAT_QWORD6_writeRegisterChangeNewData(DWORD Nth_Entry);
		 void _TRACE64FORMAT_BYTE7_writeMemoryAccessFields(DWORD Nth_Entry);

		 
		 //void _TRACE64FORMAT_QWORD8_writeMemoryAccessAddresses(DWORD Nth_Entry);
		 //void _TRACE64FORMAT_QWORD9_writeMemoryAccessOldData(DWORD Nth_Entry);
		 //void _TRACE64FORMAT_QWORD10_writeMemoryAccessNewData(DWORD Nth_Entry);
	};
}