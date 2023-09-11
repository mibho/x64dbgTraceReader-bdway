#include "../../../../include/data/data_objects/x64dbg_tracedata/x64dbg_tracedata.h"

//#include <Windows.h>

using namespace ns_xtr;

ns_xtr::x64dbg_tracedata::x64dbg_tracedata(const char* file_name) : _file_name(file_name)
{
	/*
	if (_file_name == nullptr) {
		uint_8 name_len = strlen(file_name) + 1;

		_file_name = new char[name_len];	// name stored to use as part of output file name later
		strcpy_s(_file_name, name_len, file_name);

	}
	*/

	// couldn't get file handle or size? something broken
	if (!saveFileHandle() || !saveFileSize()) {
		std::cout << "\tthis message is not supposed to be displayed! Exiting..." << std::endl;
		_cleanupIfNeededThenForceExit();
	}

	std::cout << "[+] - File handle obtained! " << std::endl;

	if (_raw_tracefile_buf == nullptr) {

		if (_file_size <= 0) {
			std::cout << "[!] - Supplied file is empty/blank or failed to detect its actual size! Exiting..." << std::endl;
			_cleanupIfNeededThenForceExit();
		}

		_raw_tracefile_buf = new data_buf(_file_size);

		if (readFile()) {
			std::cout << "\tSuccessfully read " << _bytes_read << " bytes." << std::endl;
		}
		else {
			std::cout << "OOF something wrong" << std::endl;
		}
		
	}
	std::cout << "\tx64dbg_tracedata object successfully created!" << std::endl;
}

ns_xtr::x64dbg_tracedata::~x64dbg_tracedata()
{
	_cleanup();
}

const QWORD ns_xtr::x64dbg_tracedata::readDataSize(const BYTE data_size) const
{
	return _raw_tracefile_buf->read_data_size(data_size);
}

const BYTE ns_xtr::x64dbg_tracedata::readByte() const
{
	return _raw_tracefile_buf->read_byte();
}

// get size of file to see how many bytes we reading
bool ns_xtr::x64dbg_tracedata::saveFileSize()
{
	// if failed to get file size, get error code [not interpreted; TODO later..?]
	if (!GetFileSizeEx(_tracefile_handle, reinterpret_cast<PLARGE_INTEGER>(&_file_size))) {

		DWORD error = GetLastError();

		std::cout << "[!] - Error code " << error << ": " << "Failed to get file size!" << std::endl;

		return false;
	}

	std::cout << "file size: " << _file_size << std::endl;

	return true;
}

bool ns_xtr::x64dbg_tracedata::saveFileHandle()
{
	bool success = true;

	/*
	param2: DWORD dwShareMode
	-----------------------------
	0 - prevent other processes from opening a file/dev if del/read/write access requested.

	0x4 - FILE_SHARE_DELETE - enable subsequent open operations on file/dev to request delete access.

	0x1 - FILE_SHARE_READ - enable subsequent open operations on file or dev to request read access.

	0x2 - FILE_SHARE_WRITE - enable subsequent open operations on file or dev to request write access.
	-----------------------------

	param4: DWORD dwCreationDisposition

	2 - CREATE_ALWAYS
	1 - CREATE_NEW
	4 - OPEN_ALWAYS
	3 - OPEN_EXISTING
	5 - TRUNCATE_EXISTING
	-----------------------------
	*/
	// get handle to file w/ read rights
	_tracefile_handle = CreateFileA(_file_name.c_str(),
									GENERIC_READ,
									FILE_SHARE_READ,
									NULL,
									OPEN_EXISTING,
									FILE_ATTRIBUTE_NORMAL, NULL);

	// if failed to open, get error code and set to false...
	if (_tracefile_handle == INVALID_HANDLE_VALUE) {

		DWORD error = GetLastError();

		// TODO way later: provide info corresponding to error code?
		if (error == ERROR_FILE_NOT_FOUND)
			std::cout << "[!] - Error code " << error << ": file not found!" << std::endl;
		else
			std::cout << "[!] - Error code " << error << std::endl;


		success = false;
	}

	

	return success;
}

bool ns_xtr::x64dbg_tracedata::readFile()
{
	bool success = false;

	if (_raw_tracefile_buf != nullptr) {

		success = ReadFile(_tracefile_handle,
			_raw_tracefile_buf->get_buffer_address(),
			_file_size,
			reinterpret_cast<LPDWORD>(&_bytes_read), NULL);

	}
	
	return success;
}

bool ns_xtr::x64dbg_tracedata::atEndOfFile()
{
	return (_raw_tracefile_buf->curr_file_position() >= _raw_tracefile_buf->buffer_size());
}

void ns_xtr::x64dbg_tracedata::saveJsonData(const std::string jsonblob)
{
	_jsonblob = jsonblob;
}

void ns_xtr::x64dbg_tracedata::_cleanup()
{
	if (_tracefile_handle != nullptr) {

		if (_tracefile_handle != INVALID_HANDLE_VALUE)
			CloseHandle(_tracefile_handle);

		_tracefile_handle = nullptr;
	}

	if (_raw_tracefile_buf != nullptr) {
		delete _raw_tracefile_buf;

		_raw_tracefile_buf = nullptr;
	}

}

void ns_xtr::x64dbg_tracedata::_cleanupIfNeededThenForceExit()
{
	_cleanup();
	exit(EXIT_FAILURE);

}
