#pragma once



#include <stdint.h>

#include "../../data_formats.h"

#define TEST

namespace ns_xtr {

	using namespace ns_dtypes;

	namespace misc {


		constexpr uint_8 LEN_BYTE = 1;
		constexpr uint_8 LEN_WORD = 2;
		constexpr uint_8 LEN_DWORD = 4;
		constexpr uint_8 LEN_QWORD = 8;

	}

	

	class data_buf {
	public:
		data_buf(const uint_64 total_bytes);
		~data_buf();

		// BYTE, WORD, DWORD, OR QWORD (1, 2, 4, 8 bytes)
		const QWORD read_data_size(const uint_8 read_amt);

		// read 1 byte at a time
		const BYTE read_byte();

		BYTE* get_buffer_address();

		
		uint_64 curr_file_position() { return _current_pos; };
		uint_64 buffer_size() { return _total_bytes_read; };

	private:

		bool _can_read(const uint_8 read_amt);
		const BYTE& _read_next_byte();
		const WORD& _read_next_word();
		const DWORD& _read_next_dword();
		const QWORD& _read_next_qword();
		void zero_out_buf();

		bool _allgood = false;
		BYTE* _contents = nullptr;
		uint_64 _total_bytes_read = 0;	// len of total bytes read.
		uint_64 _current_pos = 0;
		uint_32 _current_ins_block_processed = 0;

	};
}