#include "../../../../include/data/data_objects/x64dbg_tracedata/data_buf.h"	

#include <algorithm>

using namespace ns_xtr;

ns_xtr::data_buf::data_buf(const uint_64 total_bytes)
{
	if (total_bytes > 0) {
		_total_bytes_read = total_bytes;
		_contents = new BYTE[_total_bytes_read];
		zero_out_buf();
	}

#ifdef TEST
#endif
}

ns_xtr::data_buf::~data_buf()
{
	if (_contents != nullptr) {
		delete[] _contents;

#ifdef TEST
#endif
	}
}

const QWORD ns_xtr::data_buf::read_data_size(const uint_8 read_amt)
{
	QWORD result = 0LL;
 
	switch (read_amt) {
	case misc::LEN_BYTE:
		result = _read_next_byte();
		break;
	case misc::LEN_WORD:
		result = _read_next_word();
		break;
	case misc::LEN_DWORD:
		result = _read_next_dword();
		break;
	case misc::LEN_QWORD:
		result = _read_next_qword();
		break;
	default:
		_allgood = false;
		break;
	}

	if (_allgood) {
		_current_pos += read_amt;
	}
	else {
		std::cout << "failed to read @ " << _current_pos << "/" << _total_bytes_read << std::endl;
	}

	//std::cout << "curr pos: " << _current_pos << " " << static_cast<int>(result) << std::endl;
	return result;

}

const BYTE ns_xtr::data_buf::read_byte()
{
	const BYTE content = _read_next_byte();
	
	if (_allgood) {
		_current_pos += misc::LEN_BYTE;
	}
	else {
		std::cout << "ERROR! EXITING..." << std::endl;
		std::cout << "current file position: " << _current_pos << std::endl;
		exit(EXIT_FAILURE);
	}

	return content;
}

BYTE* ns_xtr::data_buf::get_buffer_address()
{
	if (_contents != nullptr) {
		return _contents;
	}
}

bool ns_xtr::data_buf::_can_read(const uint_8 read_amt)
{
	return ((((_current_pos + read_amt)) <= _total_bytes_read)  // ( •̀ᴗ•́ ) this just checks if end of buf reached
		&& (_current_pos < _total_bytes_read)) ? true : false;
}

const BYTE& ns_xtr::data_buf::_read_next_byte()
{
	if (_can_read(misc::LEN_BYTE))
	{
		_allgood = true;
		return *reinterpret_cast<BYTE*>((_contents + _current_pos));
	}

	// if ret val 0 and allgood == false; invalid.
	_allgood = false;
	return 0;
}

const WORD& ns_xtr::data_buf::_read_next_word()
{
	if (_can_read(misc::LEN_WORD))
	{
		_allgood = true;
		return *reinterpret_cast<WORD*>((_contents + _current_pos));
	}

	// if ret val 0 and allgood == false; invalid.
	_allgood = false;
	return 0;
}

const DWORD& ns_xtr::data_buf::_read_next_dword()
{
	if (_can_read(misc::LEN_DWORD))
	{
		_allgood = true;
		return *reinterpret_cast<DWORD*>((_contents + _current_pos));
	}

	// if ret val 0 and allgood == false; invalid.
	_allgood = false;
	return 0;
}

const QWORD& ns_xtr::data_buf::_read_next_qword()
{
	if (_can_read(misc::LEN_QWORD))
	{
		_allgood = true;
		return *reinterpret_cast<QWORD*>((_contents + _current_pos));
	}

	// if ret val 0 and allgood == false; invalid.
	_allgood = false;
	return 0;
}

void ns_xtr::data_buf::zero_out_buf()
{
	if (_total_bytes_read > 0)
	{
		std::fill((_contents),
				 (_contents + (_total_bytes_read - 1)),
				 0);

		_current_pos = 0;
	}
}
