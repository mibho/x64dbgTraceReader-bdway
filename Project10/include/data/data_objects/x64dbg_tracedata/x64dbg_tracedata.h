#pragma once

#include <Windows.h>
#include "data_buf.h"
namespace ns_xtr {

	using namespace ns_dtypes;

	struct x64dbg_trace_block {

		/*
		void output() {
			std::cout << "blocktype: " << static_cast<int>(blocktype) << std::endl;
			std::cout << "regchanges: " << static_cast<int>(RegisterChanges) << std::endl;
			std::cout << "memaccs: " << static_cast<int>(MemoryAccesses) << std::endl;
			std::cout << "tid: " << static_cast<int>(ThreadID) << std::endl;
			std::cout << "opc len: " << opcodes.size() << std::endl;
			std::cout << "regchanges len: " << regchanges.size() << std::endl;
			std::cout << "regdata len: " << regdata.size() << std::endl;
			std::cout << "memaccflags len: " << mem_acc_flags.size() << std::endl;
			std::cout << "memaccaddr len: " << mem_acc_addr.size() << std::endl;
			std::cout << "mem_acc_old len: " << mem_acc_old.size() << std::endl;
			std::cout << "mem_acc_new len: " << mem_acc_new.size() << std::endl;

		}
		*/
		BYTE blocktype = 0;
		BYTE RegisterChanges = 0;
		BYTE MemoryAccesses = 0;
		DWORD ThreadID = 0;
		std::vector<BYTE> opcodes;
		std::vector<BYTE> regchanges;
		std::vector<QWORD> regdata;
		std::vector<bool> mem_acc_flags;
		std::vector<QWORD> mem_acc_addr;
		std::vector<QWORD> mem_acc_old;
		std::vector<QWORD> mem_acc_new;
		DWORD mem_access_entries = 0;
	};
	
	class x64dbg_tracedata {
	public:

		x64dbg_tracedata(const char* file_name);
		~x64dbg_tracedata();

		// read BYTE, WORD, DWORD, or QWORD sized data
		const QWORD readDataSize(const BYTE data_size) const;
		// read byte at a time
		const BYTE readByte() const;

		
		bool readFile();
		bool atEndOfFile();


		const std::string& getFileName() { return _file_name;};
		const std::string& getJSONBlob() { return _jsonblob; };
		const QWORD& getFileSize() { return _file_size; };

		void saveJsonData(const std::string jsonblob);
		bool saveFileSize();
		bool saveFileHandle();
	private:

		void _cleanup();
		void _cleanupIfNeededThenForceExit();

		data_buf* _raw_tracefile_buf = nullptr;
		HANDLE _tracefile_handle = nullptr;

		std::string _file_name;	// char* _file_name = nullptr;
		std::string _jsonblob;
		QWORD _file_size {0};
		DWORD _bytes_read {0};
	 

	};
}