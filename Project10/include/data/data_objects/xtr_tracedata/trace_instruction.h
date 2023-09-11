#pragma once
/*
*
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
* 
*/
#include "../../data_formats.h"
#include <string>
namespace ns_xtr {

    

    using namespace ns_trace;
    using namespace ns_dtypes;

    class registers {
    public:
        registers() : _values(NUM_OF_REGS_I_WANT_TO_LOOK_AT) { };

        QWORD& operator[](REGISTERCONTEXT regname) {
            return _values[regname];
        };

    
        registers& operator=(registers data) {
            _values = data._values;
            return *this;
        }

        void update(REGISTERCONTEXT regname, QWORD value) { _values[regname] = value; };
    private:
        std::vector<QWORD> _values;
    };

    struct mem_access {

        mem_access(bool hasnewval, BYTE access, QWORD involved_addr, QWORD oldval, QWORD newval) {
            has_new_val = hasnewval;
            access_type = access;
            target_addr = involved_addr;
            prev_val = oldval;
            new_val = newval;
        };
        bool has_new_val = false;
        BYTE access_type  { 0 };
        QWORD target_addr { 0 };
        QWORD prev_val    { 0 };
        QWORD new_val     { 0 };
    };

    class traceinstruction {
    public:
        traceinstruction(registers regs, std::vector<BYTE> opcodes, std::vector<mem_access> memdata);
        
        QWORD& getRIP()    { return _regs[RIP]; };
        QWORD& getRFLAGS() { return _regs[RFLAGS]; };
        QWORD& getRegister(REGISTERCONTEXT regname) { return _regs[regname]; };
        const uint_8& getOpcodeCount() { return _opcodes.size(); };
        std::vector<BYTE>& getOpcodes() { return _opcodes; }
        const uint_8& getMemAccessCount() { return _memdata.size(); };
        
        registers& getRegistersObject() { return _regs; };
        std::vector<mem_access>& getMemAccessObject() { return _memdata; }
        //const std::string& get_disasm_line() const { return _disasm_line; }
    private:
        registers _regs;
        std::vector<BYTE> _opcodes;
        std::vector<mem_access> _memdata;
        //std::string _disasm_line;
    };
}