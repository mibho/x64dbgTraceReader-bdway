#include "../../include/data/data_manager.h"

#include <nlohmann/json.hpp>

// for convenience
using json = nlohmann::json;

using namespace ns_xtr;

void ns_xtr::data_manager::testfn()
{
	if (_xtr_trace != nullptr) {
		_xtr_trace->removeNthEntry(3);
		_xtr_trace->someInfo();
	}
}

ns_xtr::data_manager::data_manager(const char* file_name)
{
	// init xtr_tracedata later. first need to format trace64 data
	if (_provided_file == nullptr) {
		_provided_file = new x64dbg_tracedata(file_name);
	}

}

ns_xtr::data_manager::~data_manager()
{
	if (_provided_file != nullptr) {
		delete _provided_file;
		_provided_file = nullptr;
	}

	if (_xtr_trace != nullptr) {
		delete _xtr_trace;
		_xtr_trace = nullptr;
	}
	if (_newfile != nullptr && _newfile != INVALID_HANDLE_VALUE) {
		CloseHandle(_newfile);
		_newfile = nullptr;
	}
	if (_logged_regs != nullptr) {
		delete _logged_regs;
		_logged_regs = nullptr;
	}
}

// check that input file header adheres to trace64 file format
bool ns_xtr::data_manager::confirmTrace64File()
{
	return (_isMagicBytesSigFound() && _processJSONBlob());//(_isMagicBytesSigFound() && _processJSONBlob());

}


/*
* NOTE: current implementation renders most of the data from REGDUMP struct useless.
*		ONLY "essential" registers from REGISTERCONTEXT and FLAGS members are preserved.
*/

void ns_xtr::data_manager::parseToEnd()
{
	if (!_setupXTRTracedata())
		return;

	while (!_provided_file->atEndOfFile()) {
		trace64ToXTR();
		//std::cout << "hmm" << std::endl;
	}
}

std::string ns_xtr::data_manager::rcToEnglish(uint_8 index)
{
	switch (index) {

	}
	return std::string();
}

bool ns_xtr::data_manager::trace64ToXTR()
{
	

	//see https://github.com/x64dbg/docs/blob/master/developers/tracefile.md 
	x64dbg_trace_block trace64_data;
	std::vector<REGISTERCONTEXT> changes;
	

	_TRACE64FORMAT_BYTE1_readBlockType(trace64_data); // can 'ignore' for now
 	_TRACE64FORMAT_BYTE2_readRegisterChanges(trace64_data);

	//x64dbg saves all reg data at very start of a trace/every 512th
	if (trace64_data.RegisterChanges == misc::X64DBG_ALL_REGISTERS_SAVED || ((_xtr_trace->size() % misc::X64DBG_MULTIPLE_OF_512) == 0)) {

		std::cout << "Every 512 instructions OR at the beginning of each tracefile, x64dbg saves data. this is one of them." << std::endl;
		_preserveData = true;
	}

	_TRACE64FORMAT_BYTE3_readMemoryAccesses(trace64_data);
	_TRACE64FORMAT_BYTE4_readBlockFlagsAndOpcodeSize(trace64_data);
	_TRACE64FORMAT_BYTE5_readRegisterChangePosition(trace64_data);
	_TRACE64FORMAT_QWORD6_readRegisterChangeNewData(trace64_data);
	_TRACE64FORMAT_BYTE7_readMemoryAccessFlags(trace64_data);
	_TRACE64FORMAT_QWORD8_readMemoryAccessAddresses(trace64_data);
	_TRACE64FORMAT_QWORD9_readMemoryAccessOldData(trace64_data);
	_TRACE64FORMAT_QWORD10_readMemoryAccessNewData(trace64_data);


	// check if threadID is already 'registered'. if not, create container.
	if (!_xtr_trace->isEncounteredThreadID(trace64_data.ThreadID)) {
		_xtr_trace->createThreadIDEntry(trace64_data.ThreadID);
	}
	
	_xtr_trace->addInstructionEntryForThreadID(trace64_data.ThreadID, _TRACE64FORMAT_convertToXTREntry(trace64_data));

	_preserveData = false; // reset


	return false; // fix
}

bool ns_xtr::data_manager::xtrToTrace64File(const char* filename)
{
	// if it's not initialized, then trace64 data wasnt processed properly
	if (_xtr_trace != nullptr) {

		if (_prev_TID) // reset val if was used when processing input data 
			_prev_TID = 0;

		// if more than 1 entry for threads, true.
		bool multiple_threadIDs = (_xtr_trace->getThreadIDCount() > 1) ? true : false;
		DWORD bytes_written = 0;
		std::vector<BYTE> tmp_trace64buf;

		// create new output file, get its handle and original json data.
		if (_createNewTrace64File(filename) && _copyBlobData()) {

			_TRACE64FORMAT_HEADER1_writeMagicBytes();
			_TRACE64FORMAT_HEADER2_writeJsonData();

			QWORD index = 0;
			QWORD size = 0;
			
			if (!multiple_threadIDs) {
				while (index < _xtr_trace->getInstructionCount()) {
					_TRACE64FORMAT_writeAsTrace64Entry(index);
					index++;
				}
			}
			else {

			}
			

			std::cout << "file successfully written" << std::endl;
			return true;
		}
	}
	return false;
}

bool ns_xtr::data_manager::_createNewTrace64File(const char* filename)
{
	if (filename == nullptr || strlen(filename) == 0) {
		std::cout << "output name not provided. file will be created with default name: processed_trace.trace64" << std::endl;
		std::string tmpname = "processed_trace.trace64";

		_newfile = CreateFileA(tmpname.c_str(), GENERIC_WRITE, FILE_SHARE_WRITE,
								NULL, CREATE_ALWAYS,
								FILE_ATTRIBUTE_NORMAL, NULL);
	}
	else {
		_newfile = CreateFileA(filename, GENERIC_WRITE, FILE_SHARE_WRITE,
								NULL, CREATE_ALWAYS,
								FILE_ATTRIBUTE_NORMAL, NULL);
	}
	bool success = (_newfile != INVALID_HANDLE_VALUE);

	return success;
}

bool ns_xtr::data_manager::_copyBlobData()
{
	if (_provided_file != nullptr) {

		_newblob = _provided_file->getJSONBlob();
		return true;
	}

	return false;
}

bool ns_xtr::data_manager::_setupXTRTracedata()
{
	if (!_isSetUp) {

		if (_xtr_trace == nullptr) {
			_xtr_trace = new xtr_tracedata();
		}
		if (_logged_regs == nullptr) {
			_logged_regs = new registers();
		}

		_isSetUp = true;
	}
	
	return _isSetUp;
}

bool ns_xtr::data_manager::_appendToNewFileBuf(BYTE* srcval, QWORD valsize)
{
	if (srcval != nullptr && valsize != 0) 
	{
		for (int i = 0; i < valsize; i++) {
			_newfile_databuf.push_back((*(srcval + i) & 0xFF));
		}
		return true;
	}

	std::cout << "failed to append to buffer required to write to new trace64 file." << std::endl;
	return false; 
}

bool ns_xtr::data_manager::_appendToNewFileBuf(ns_xtr::QWORD srcval) {

	_newfile_databuf.push_back(((srcval) & 0xFF));
	_newfile_databuf.push_back(((srcval >> 8) & 0xFF));
	_newfile_databuf.push_back(((srcval >> 16) & 0xFF));
	_newfile_databuf.push_back(((srcval >> 24) & 0xFF));
	_newfile_databuf.push_back(((srcval >> 32) & 0xFF));
	_newfile_databuf.push_back(((srcval >> 40) & 0xFF));
	_newfile_databuf.push_back(((srcval >> 48) & 0xFF));
	_newfile_databuf.push_back(((srcval >> 56) & 0xFF));

	return true; //fix
}

bool ns_xtr::data_manager::_appendToNewFileBuf(ns_xtr::DWORD srcval) {

	_newfile_databuf.push_back(((srcval) & 0xFF));
	_newfile_databuf.push_back(((srcval >> 8) & 0xFF));
	_newfile_databuf.push_back(((srcval >> 16) & 0xFF));
	_newfile_databuf.push_back(((srcval >> 24) & 0xFF));

	return true; //fix
}

bool ns_xtr::data_manager::_appendToNewFileBuf(BYTE srcval)
{
	_newfile_databuf.push_back(((srcval) & 0xFF));
	return false; //fix
}

void ns_xtr::data_manager::_clearNewFileBuf() 
{
	_newfile_databuf.clear();
}

bool ns_xtr::data_manager::_processJSONBlob()
{
	bool success = false;
	std::string blob;
	uint_32 blob_length = _provided_file->readDataSize(misc::JSON_BLOB_HEADER_SIZE);

	if (!_provided_file->atEndOfFile()) {

		
		for (int i = 0; i < blob_length; i++) {
			blob += _provided_file->readByte();
		}

		_provided_file->saveJsonData(blob);

		// ensure supposed blob len is equal to real blob len. 
		success = (_newblob.length() == blob_length) ? true : false;

	}

	return success;
}

bool ns_xtr::data_manager::_isMagicBytesSigFound()
{
	bool success = false;
	DWORD magic_bytes = 0;

	magic_bytes = _provided_file->readDataSize(misc::X64DBG_TRACE_MAGIC_SIG_LENGTH);

	// ensure 'magic' sig TRAC is present: checking 1st 4 magic bytes (is == TRAC?) 
	success = (magic_bytes == misc::X64DBG_TRACE_MAGIC_BYTES_LE) ? true : false;

	return success;
}

xtr_data ns_xtr::data_manager::_TRACE64FORMAT_convertToXTREntry(x64dbg_trace_block& entry)
{
	std::vector<REGISTERCONTEXT> affregs;
	std::vector<mem_access> memstuff;
	bool hasnew = false;
	// very start of trace OR multiple of 512
	if (_preserveData) {

		// save ALL context (only 0-30 rn) 
		for (int i = 0; i < ns_trace::NUM_OF_REGS_I_WANT_TO_LOOK_AT; i++) {
			_logged_regs->update(static_cast<REGISTERCONTEXT>(i), entry.regdata[i]); //update all regs
			affregs.push_back(static_cast<REGISTERCONTEXT>(i));		// keep track of what updated
		}


	}
	else { // normal update
 
			affregs = getRCForm(entry.regchanges);
			
			// only update regs that correspond to appropriate RegChange register
			for (int j = 0; j < affregs.size(); j++) {
				_logged_regs->update(affregs[j], entry.regdata[j]);
			}
		 
	}
	for (int i = 0; i < entry.mem_acc_flags.size(); i++) {

		if (entry.mem_acc_flags[i]) {
			memstuff.push_back(mem_access(true, 0, entry.mem_acc_addr[i], entry.mem_acc_old[i], entry.mem_acc_new[i]));
		}
		else {
			memstuff.push_back(mem_access(false, 0, entry.mem_acc_addr[i], entry.mem_acc_old[i], 0));
		}
		
	}
	
	return xtr_data(affregs, *_logged_regs, entry.opcodes, memstuff);
	}



void ns_xtr::data_manager::_TRACE64FORMAT_BYTE1_readBlockType(x64dbg_trace_block& entry)
{
	entry.blocktype = _provided_file->readByte(); // 1st field = BlockType. currently only 0 defined.
}

void ns_xtr::data_manager::_TRACE64FORMAT_BYTE2_readRegisterChanges(x64dbg_trace_block& entry)
{
	entry.RegisterChanges = _provided_file->readByte(); // Field 2: len(RegisterChangePosition) and len(RegisterChangeNewData). eg: if RegisterChanges = 4, len(RegisterChangePosition) = 4 and len(RegisterChangeNewData) = 32 [4 QWORDs]
}

void ns_xtr::data_manager::_TRACE64FORMAT_BYTE3_readMemoryAccesses(x64dbg_trace_block& entry)
{
	entry.MemoryAccesses = _provided_file->readByte(); // Field 3: len(MemoryAccessFlags)
}

void ns_xtr::data_manager::_TRACE64FORMAT_BYTE4_readBlockFlagsAndOpcodeSize(x64dbg_trace_block& entry)
{
	BYTE BlockFlagsAndOpcodeSize = _provided_file->readByte();
	BYTE opcode_size = 0;
	
	// BlockFlagsAndOpcodeSize - MSB(byte) set? Y - entry for ThreadID exists. N - next.
	if (BlockFlagsAndOpcodeSize & misc::bitstuff::THREAD_ID_MASK_SET) {

		entry.ThreadID = _provided_file->readDataSize(misc::LEN_DWORD);
		_prev_TID = entry.ThreadID; // save entry to check
	}
	else { // MSB not set; same thread ID

		if (!_prev_TID) // TID == 0 should never happen if valid trace64 block
		{
			std::cout << "threadID == 0 or invalid block format" << std::endl;
			exit(EXIT_FAILURE);
		}

		entry.ThreadID = _prev_TID; // same TID as prior block
	}

	// lower 4 bits represent length of opcode bytes
	opcode_size = BlockFlagsAndOpcodeSize & misc::bitstuff::OPCODE_LENGTH_MASK;
	for (int i = 0; i < opcode_size; i++) {
		entry.opcodes.push_back(_provided_file->readByte());
	}
}

void ns_xtr::data_manager::_TRACE64FORMAT_BYTE5_readRegisterChangePosition(x64dbg_trace_block& entry) 
{
	for (int i = 0; i < entry.RegisterChanges; i++) {
		entry.regchanges.push_back(_provided_file->readByte());
	}
}

void ns_xtr::data_manager::_TRACE64FORMAT_QWORD6_readRegisterChangeNewData(x64dbg_trace_block& entry)
{
	// RegisterChanges # of QWORDs
	for (int i = 0; i < entry.RegisterChanges; i++) {
		entry.regdata.push_back(_provided_file->readDataSize(misc::LEN_QWORD));
	}
}

void ns_xtr::data_manager::_TRACE64FORMAT_BYTE7_readMemoryAccessFlags(x64dbg_trace_block& entry)
{
	BYTE value = 0;
	// MemoryAccessFlags array
	for (int i = 0; i < entry.MemoryAccesses; i++) {
		value = _provided_file->readByte();
		std::cout << static_cast<int>(value) << std::endl;

		if ((value & 1) == 0) { // When bit 0 is set, it indicates the memory is not changed (This could mean it is read, or it is overwritten with identical value). bit 0 not set = mem change
			//std::cout << "position " << i << " " << "has a mem change!" << std::endl;
			entry.mem_access_entries += 1;
			entry.mem_acc_flags.push_back(true);
		}
		else {
			entry.mem_acc_flags.push_back(false);
		}

	}

}

void ns_xtr::data_manager::_TRACE64FORMAT_QWORD8_readMemoryAccessAddresses(x64dbg_trace_block& entry)
{
	// MemoryAccesses # of QWORDs
	for (int i = 0; i < entry.MemoryAccesses; i++) {
		entry.mem_acc_addr.push_back(_provided_file->readDataSize(misc::LEN_QWORD));
	}
}

void ns_xtr::data_manager::_TRACE64FORMAT_QWORD9_readMemoryAccessOldData(x64dbg_trace_block& entry)
{
	// MemoryAccesses # of QWORDs
	for (int i = 0; i < entry.MemoryAccesses; i++) {
		entry.mem_acc_old.push_back(_provided_file->readDataSize(misc::LEN_QWORD));
	}
}

void ns_xtr::data_manager::_TRACE64FORMAT_QWORD10_readMemoryAccessNewData(x64dbg_trace_block& entry)
{
	// mem_access_entries # of QWORDs. DIFFERENT THAN ABOVE 2
	for (int i = 0; i < entry.mem_access_entries; i++) {
		entry.mem_acc_new.push_back(_provided_file->readDataSize(misc::LEN_QWORD));
	}
}



ns_xtr::DWORD ns_xtr::data_manager::_TRACE64FORMAT_writeAsTrace64Entry(ns_xtr::DWORD Nth_Entry)
{
	DWORD bytes_written = 0;

	if (_xtr_trace->doesNthEntryExist(Nth_Entry)) {
		BYTE RegChangeCount = _xtr_trace->_TRACE64FORMAT_getNthInstructionRegChangesCount(Nth_Entry);
		bool success = false;

		if (RegChangeCount == misc::X64DBG_ALL_REGISTERS_SAVED || ((Nth_Entry % misc::X64DBG_MULTIPLE_OF_512) == 0)) {

			std::cout << "WRITE - Every 512 instructions OR at the beginning of each tracefile, x64dbg saves data. this is one of them." << std::endl;
			_preserveData = true;
		}
		// does the hard work! : )
		_TRACE64FORMAT_BYTE1_writeBlockType(Nth_Entry);
		_TRACE64FORMAT_BYTE2_writeRegisterChanges(Nth_Entry);
		_TRACE64FORMAT_BYTE3_writeMemoryAccesses(Nth_Entry);
		_TRACE64FORMAT_BYTE4_writeBlockFlagsAndOpcodeSize(Nth_Entry);
		_TRACE64FORMAT_BYTE5_writeRegisterChangePosition(Nth_Entry);
		_TRACE64FORMAT_QWORD6_writeRegisterChangeNewData(Nth_Entry);
		_TRACE64FORMAT_BYTE7_writeMemoryAccessFields(Nth_Entry);

		success = WriteFile(_newfile, &_newfile_databuf[0], _newfile_databuf.size(), reinterpret_cast<LPDWORD>(&bytes_written), NULL);

		_preserveData = false; // reset
		_clearNewFileBuf();

		/*
		if (success) {
			std::cout << "number of bytes written: " << bytes_written << std::endl;
		}
		else {
			std::cout << "WriteFile failed. bytes written = " << bytes_written << std::endl;
		}
		*/
	}
	return bytes_written;
}

void ns_xtr::data_manager::_TRACE64FORMAT_HEADER1_writeMagicBytes()
{
	std::string trace64_magic = "TRAC";

	_appendToNewFileBuf(reinterpret_cast<BYTE*>(&trace64_magic[0]), trace64_magic.length());
}


void ns_xtr::data_manager::_TRACE64FORMAT_HEADER2_writeJsonData()
{
	DWORD bloblen = _newblob.length();

	// DWORD indicating size of upcoming json blob data
	_appendToNewFileBuf(reinterpret_cast<BYTE*>(&bloblen), 4);

	// json blob data
	_appendToNewFileBuf(reinterpret_cast<BYTE*>(&_newblob[0]), bloblen);
}

void ns_xtr::data_manager::_TRACE64FORMAT_BYTE1_writeBlockType(ns_xtr::DWORD Nth_Entry)
{
	// start entry with 0
	_appendToNewFileBuf(misc::X64DBG_BLOCKTYPE_ID);
}

void ns_xtr::data_manager::_TRACE64FORMAT_BYTE2_writeRegisterChanges(ns_xtr::DWORD Nth_Entry)
{
	BYTE RegisterChangesValue = _xtr_trace->_TRACE64FORMAT_getNthInstructionRegChangesCount(Nth_Entry);
	_appendToNewFileBuf(RegisterChangesValue);
}

void ns_xtr::data_manager::_TRACE64FORMAT_BYTE3_writeMemoryAccesses(ns_xtr::DWORD Nth_Entry)
{
	BYTE MemoryAccessesValue = _xtr_trace->_TRACE64FORMAT_getNthInstructionMemAccessesCount(Nth_Entry);
	_appendToNewFileBuf(MemoryAccessesValue);
}

void ns_xtr::data_manager::_TRACE64FORMAT_BYTE4_writeBlockFlagsAndOpcodeSize(ns_xtr::DWORD Nth_Entry)
{
	bool setMSB = false;
	BYTE BlockFlagsAndOpcodeSizeValue = 0;

	if (!_prev_TID) { // _prev_TID should never be 0 unless we just started to output data as trace64
		_prev_TID = _xtr_trace->_TRACE64FORMAT_getNthInstructionThreadID(Nth_Entry);
		setMSB = true;
	}
	else // _prev_TID = nonzero; check if diff
	{
		DWORD curr_TID = _xtr_trace->getThreadIDForInstruction(Nth_Entry);

		if ((_prev_TID != curr_TID) || _preserveData) { // ids diff or (n*512)th entry
			_prev_TID = curr_TID;
			setMSB = true;
		}
		// otherwise, tids are same; keep default val
	}

	// get opcode bytes of curr ins
	std::vector<BYTE> op_bytes = _xtr_trace->_TRACE64FORMAT_getNthInstructionOpcodes(Nth_Entry);

	BlockFlagsAndOpcodeSizeValue = op_bytes.size();

	if (setMSB) {// if MSB set, need write TID. else dw
		BlockFlagsAndOpcodeSizeValue |= misc::bitstuff::THREAD_ID_MASK_SET; // set MSB; entry exists so next write is a DWORD

		_appendToNewFileBuf(BlockFlagsAndOpcodeSizeValue);
		_appendToNewFileBuf(_prev_TID);	// thread val added since MSB set

	}
	else 
	{
		_appendToNewFileBuf(BlockFlagsAndOpcodeSizeValue);
	}

	_appendToNewFileBuf(&op_bytes[0], op_bytes.size());
}

void ns_xtr::data_manager::_TRACE64FORMAT_BYTE5_writeRegisterChangePosition(ns_xtr::DWORD Nth_Entry)
{
	if (_preserveData) 
	{
		// 0xAC = 172 #s of 0s; see absorelative form lol
		for (int i = 0; i < misc::X64DBG_ALL_REGISTERS_SAVED; i++) {
			_appendToNewFileBuf(misc::X64DBG_ZERO);
		}
	}
	else {

		std::vector<REGISTERCONTEXT> RegChangePositions = _xtr_trace->_TRACE64FORMAT_getNthInstructionRegChangesIndices(Nth_Entry);
		std::vector<uint_8> proper_form = getAbsoRelativeForm(RegChangePositions);	// convert back to form understandable by x64dbg

		for (int i = 0; i < proper_form.size(); i++) {
			_appendToNewFileBuf(proper_form[i]);
		}

	}
	
}

void ns_xtr::data_manager::_TRACE64FORMAT_QWORD6_writeRegisterChangeNewData(ns_xtr::DWORD Nth_Entry)
{
	// get new values
	std::vector<QWORD> RegChangeNewData =  _xtr_trace->_TRACE64FORMAT_getNthInstructionRegChangesData(Nth_Entry);

	for (int i = 0; i < RegChangeNewData.size(); i++) {
		_appendToNewFileBuf(RegChangeNewData[i]); 
	}


	// need to fill in space to match 172 slots.
	if (_preserveData) {
		DWORD remaining = ((misc::X64DBG_ALL_REGISTERS_SAVED - RegChangeNewData.size()) * misc::LEN_QWORD);
		for (int i = 0; i < remaining; i++) {
			_appendToNewFileBuf(misc::X64DBG_ZERO);
		}
	}



}

void ns_xtr::data_manager::_TRACE64FORMAT_BYTE7_writeMemoryAccessFields(ns_xtr::DWORD Nth_Entry)
{
	std::vector<mem_access> memdata = _xtr_trace->_TRACE64FORMAT_getNthInstructionMemAccessesData(Nth_Entry);

	umap<DWORD, QWORD> cheekydata;
	uint_8 new_addr_count = 0;

	constexpr DWORD START_INDEX_0_FLAGS = 0;
	constexpr DWORD START_INDEX_1_ADDRS = 1000;
	constexpr DWORD START_INDEX_2_NEW   = 2000;
	constexpr DWORD START_INDEX_2_OLD   = 3000;
	
	// get needed data w/ cheeky mapping
	for (int i = 0; i < memdata.size(); i++) {

		cheekydata.insert(START_INDEX_1_ADDRS + i, memdata[i].target_addr); // mem access addr data 
		cheekydata.insert(START_INDEX_2_OLD + i, memdata[i].prev_val);	// mem access old val

		if (memdata[i].has_new_val) {
			cheekydata.insert(START_INDEX_0_FLAGS + i, 0); // memAccFlags
			cheekydata.insert(START_INDEX_2_NEW + i, memdata[i].new_val); // only append if exists
			//_appendToNewFileBuf(0); // bit NOT set = entry exists
			new_addr_count++;
		}
		else {
			cheekydata.insert(START_INDEX_0_FLAGS + i, 1); // bit SET = DNE 
		}

	}

	for (int i = 0; i < memdata.size(); i++) {
		if (cheekydata.contains(START_INDEX_0_FLAGS + i))
			_appendToNewFileBuf(static_cast<BYTE>(cheekydata.at(START_INDEX_0_FLAGS + i)));
	}

	for (int i = 0; i < memdata.size(); i++) {
		if (cheekydata.contains(START_INDEX_1_ADDRS + i)) {
			 _appendToNewFileBuf(cheekydata.at(START_INDEX_1_ADDRS + i));
		}
	}
 

	for (int i = 0; i < memdata.size(); i++) {
		if (cheekydata.contains(START_INDEX_2_OLD + i)) {
			_appendToNewFileBuf(cheekydata.at(START_INDEX_2_OLD + i));
		}
	}

	for (int i = 0; i < memdata.size(); i++) {
		if (cheekydata.contains(START_INDEX_2_NEW + i)) {
			_appendToNewFileBuf(cheekydata.at(START_INDEX_2_NEW + i));
		}
	}

}


/*
void ns_xtr::data_manager::parseTraceBlocks()
{
	//see https://github.com/x64dbg/docs/blob/master/developers/tracefile.md
	x64dbg_trace_block trace64_data;
	BYTE BlockFlagsAndOpcodeSize = 0;
	BYTE opcode_size = 0;

	// BlockType - always 0 for now
	trace64_data.blocktype = _provided_file->readByte();

	if (trace64_data.blocktype == 0) {
		std::cout << "start of blocktype!" << std::endl;
	}

	// RegisterChanges - # of elements in arr. RegisterChangePosition and RegisterChangeNewData
	trace64_data.RegisterChanges = _provided_file->readByte();

	//x64dbg saves all reg data at very start of a trace/every 512th
	if (trace64_data.RegisterChanges == misc::X64DBG_ALL_REGISTERS_SAVED || ((_trace_entry % misc::X64DBG_MULTIPLE_OF_512) == 0)  ) {

		std::cout << "Every 512 instructions, x64dbg saves data. this is one of them." << std::endl;
	}

	std::cout << "\t\tRegisterChanges byte read. value: " << static_cast<int>(trace64_data.RegisterChanges) << std::endl;
	// MemoryAccesses - # of elements in arr. MemoryAccessFlags
	trace64_data.MemoryAccesses  = _provided_file->readByte();

	std::cout << "\t\tMemoryAccesses byte read. value: " << static_cast<int>(trace64_data.MemoryAccesses) << std::endl;
	// BlockFlagsAndOpcodeSize - MSB(byte) set? Y - entry for ThreadID exists. N - next.
	BlockFlagsAndOpcodeSize = _provided_file->readByte();
	if (BlockFlagsAndOpcodeSize & misc::bitstuff::THREAD_ID_MASK) {
		std::cout << "ThreadID field exists!" << std::endl;

		trace64_data.ThreadID = _provided_file->readDataSize(misc::LEN_DWORD);
		std::cout << "Thread ID: " << std::hex << trace64_data.ThreadID << std::endl;

		_prev_TID = trace64_data.ThreadID;
	}
	else {

		if (_prev_TID)
			trace64_data.ThreadID = _prev_TID;
	}

	opcode_size = BlockFlagsAndOpcodeSize & misc::bitstuff::OPCODE_LENGTH_MASK;
	std::cout << "Opcode length: " << static_cast<int>(opcode_size) << " " << std::endl;
	for (int i = 0; i < opcode_size; i++) {
		trace64_data.opcodes.push_back(_provided_file->readByte());
	}

	for (int i = 0; i < opcode_size; i++) {
		std::cout << std::hex << static_cast<int>(trace64_data.opcodes[i]) << " ";
	}
	std::cout << std::endl;

	//_disasm_tool->disasm(0x1000, &trace64_data.opcodes[0], opcode_size, 0);//disasm(0x1000, &trace64_data.opcodes[0], opcode_size, 0);

	// RegisterChangePosition - read RegisterChanges amount of bytes.
	std::cout << "next bytes should correspond to RegisterChangePosition. looping " << static_cast<int>(trace64_data.RegisterChanges) << " times." << std::endl;

	for (int i = 0; i < trace64_data.RegisterChanges; i++) {
		trace64_data.regchanges.push_back(_provided_file->readByte());

		std::cout << "index: " << i << " = " << static_cast<int>(trace64_data.regchanges[i]) << std::endl;

	}
	get_absorelative_mapping(trace64_data.regchanges);

	std::cout << "next bytes should correspond to RegisterChangeNewData. looping " << static_cast<int>(trace64_data.RegisterChanges) << " times." << std::endl;
	int pos = 0;
	// RegisterChangeNewData pointers
	for (int i = 0; i < trace64_data.RegisterChanges; i++) {
		trace64_data.regdata.push_back(_provided_file->readDataSize(misc::LEN_QWORD));
		std::cout << pos << " "  << std::hex << trace64_data.regdata[i] << ")" << std::endl;
		pos += 1;

	}

	BYTE value = 0;
	// MemoryAccessFlags array
	std::cout << "next bytes should correspond to MemoryAccesses. looping " << static_cast<int>(trace64_data.MemoryAccesses) << " times." << std::endl;
	for (int i = 0; i < trace64_data.MemoryAccesses; i++) {
		value = _provided_file->readByte();
		std::cout << static_cast<int>(value) << std::endl;

		if ((value & 1) == 0) {
			std::cout << "position " << i << " " << "has a mem change!" << std::endl;
			trace64_data.mem_access_entries += 1;
		}

	}
	std::cout << "num of mem changes " << trace64_data.mem_access_entries << std::endl;

	QWORD ptrval = 0;
	// MemoryAccessAddresses
	for (int i = 0; i < trace64_data.MemoryAccesses; i++) {
		ptrval = _provided_file->readDataSize(misc::LEN_QWORD);
		std::cout << "MemoryAccessAddresses: " << std::hex << ptrval << std::endl;
	}

	// MemoryAccessOldData
	for (int i = 0; i < trace64_data.MemoryAccesses; i++) {
		ptrval = _provided_file->readDataSize(misc::LEN_QWORD);
		std::cout << "MemoryAccessOldData: " << std::hex << ptrval << std::endl;
	}

	// MemoryAccessNewData - entry only exists if memory acc flags not set.
	for (int i = 0; i < trace64_data.mem_access_entries; i++) {
		ptrval = _provided_file->readDataSize(misc::LEN_QWORD);
		std::cout << "MemoryAccessNewData: " << std::hex << ptrval << std::endl;

	}

	_trace_entry += 1;

}
*/