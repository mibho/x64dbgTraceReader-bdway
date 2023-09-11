#include "../../../../include/data/data_objects/xtr_tracedata/xtr_tracedata.h"

using namespace ns_xtr;

ns_xtr::xtr_tracedata::xtr_tracedata()
{
}

ns_xtr::xtr_tracedata::~xtr_tracedata()
{
}

void ns_xtr::xtr_tracedata::someInfo()
{
	std::cout << "Total threadID entries: " << size();
	for (auto umap_itr = _history.begin(); umap_itr != _history.end(); ++umap_itr) {
		auto tID = umap_itr->first;
		std::cout << " " << tID << std::endl;
	}
}

traceinstruction& ns_xtr::xtr_tracedata::getInstructionFromNthEntry(DWORD Nth_entry)
{
	return getNthEntry(Nth_entry)._ins_data;
}

bool ns_xtr::xtr_tracedata::doesNthEntryExist(DWORD Nth_entry)
{
	return _history.at(_currTID).contains(Nth_entry);
}

xtr_data& ns_xtr::xtr_tracedata::getNthEntry(DWORD Nth_entry)
{
	// make sure _currTID set or dont do
	if (isEncounteredThreadID(_currTID)) 
	{
		return _history.at(_currTID).at(Nth_entry);
	}
}

bool ns_xtr::xtr_tracedata::removeNthEntry(DWORD Nth_entry)
{
	if (isEncounteredThreadID(_currTID))
	{
		return _history.at(_currTID).erase(Nth_entry);
	}
}

bool ns_xtr::xtr_tracedata::createThreadIDEntry(DWORD threadID)
{
	if (_history.contains(threadID)) {
		std::cout << "threadID: " << threadID << " is already registered!" << std::endl;
		return false;
	}

	_history.insert(threadID, umap<DWORD, xtr_data>());
	_currTID = threadID;

	return true;

}

bool ns_xtr::xtr_tracedata::addInstructionEntry(xtr_data ins_info)
{
	if (isEncounteredThreadID(_currTID)) {

	}
	return false;
}

bool ns_xtr::xtr_tracedata::addInstructionEntryForThreadID(DWORD threadID, xtr_data ins_info)
{
	if (isEncounteredThreadID(threadID)) {
		_history.at(threadID).insert(getInstructionCountForThreadID(threadID), ins_info);
		//ins_info.output();
		return true;
	}
	return false;
}

umap<DWORD, xtr_data>& ns_xtr::xtr_tracedata::getTraceEntryForThreadID(DWORD threadID)
{
	if (isEncounteredThreadID(threadID))
	{
		_currTID = threadID;
		return _history.at(threadID);
	}
	
}

const DWORD& ns_xtr::xtr_tracedata::getInstructionCountForThreadID(DWORD threadID) 
{
	if (isEncounteredThreadID(threadID)) {
		return _history.at(threadID).size();
	}
	
	return 0;
}

const DWORD ns_xtr::xtr_tracedata::getThreadIDForInstruction(DWORD instructionID)
{
	for (auto umap_itr = _history.begin(); umap_itr != _history.end(); ++umap_itr) {
		auto tID = umap_itr->first;
		//std::cout << "val is: " << tID << std::endl;
		if (_history.at(tID).contains(instructionID)) {
			return tID;
		}
	}
}
