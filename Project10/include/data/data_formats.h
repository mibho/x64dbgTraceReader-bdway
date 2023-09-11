#pragma once
/*
* 
* **********************************************************************
*   Namespaces
* **********************************************************************
*       
*       ns_dtypes - basic data types just given names i'm comfy with
* 
* 
* 
* 
* 
* **********************************************************************
*   Custom data structs
* **********************************************************************
*   
*/
#include <unordered_map>
#include <vector>

#include <iostream>
#include <string>
#include <stdlib.h>


namespace ns_xtr {

	namespace ns_dtypes {
		typedef __int8 int_8;
		typedef __int16 int_16;
		typedef __int32 int_32;
		typedef __int64 int_64;
		typedef unsigned __int8 uint_8;
		typedef unsigned __int16 uint_16;
		typedef unsigned __int32 uint_32;
		typedef unsigned __int64 uint_64;
        
		typedef unsigned __int8  BYTE;
		typedef unsigned __int16 WORD;
		typedef unsigned __int32 DWORD;
		typedef unsigned __int64 QWORD;

        typedef void* HANDLE;

	}
    using namespace ns_dtypes;
    namespace misc {

       

        namespace bitstuff {
            constexpr BYTE THREAD_ID_MASK_SET = 0x80;	 // 128 (dec)		| 1000 0000 (binary) 
            constexpr BYTE OPCODE_LENGTH_MASK = 0xF; // 15  (dec)		| 0000 1111	(binary)
        }

        constexpr DWORD X64DBG_TRACE_MAGIC_BYTES_LE = 1128354388;	// in hex: 0x43415254	T R A C = 0x54 0x52 0x41 0x43 
        constexpr WORD X64DBG_MULTIPLE_OF_512 = 512;
        constexpr BYTE X64DBG_ALL_REGISTERS_SAVED = 172; // 0xAC 
        constexpr BYTE XTR_ALL_REGISTERS_SAVED = 30;
        constexpr BYTE X64DBG_TRACE_MAGIC_SIG_LENGTH = 4;
        constexpr BYTE JSON_BLOB_HEADER_SIZE = 4;

        constexpr BYTE X64DBG_BLOCKTYPE_ID = 0;
        constexpr BYTE X64DBG_ZERO = 0;

 

    }

    namespace ns_trace {

        constexpr unsigned short NUM_OF_REGS_I_WANT_TO_LOOK_AT = 30;

        typedef enum REGISTERCONTEXT {

            RAX = 0,
            RCX,    // 1
            RDX,    // 2
            RBX,    // 3
            RSP,    // 4
            RBP,    // 5
            RSI,    // 6
            RDI,    // 7
            R8,     // 8
            R9,     // 9
            R10,    // 10
            R11,    // 11
            R12,    // 12
            R13,    // 13
            R14,    // 14
            R15,    // 15
            RIP,    // 16
            RFLAGS, // 17
            GS,     // 18
            FS,     // 19
            ES,     // 20
            DS,     // 21
            CS,     // 22
            SS,     // 23
            DR0,    // 24
            DR1,    // 25
            DR2,    // 26
            DR3,    // 27
            DR6,    // 28
            DR7     // 29
        } REGISTERCONTEXT;

    }
	
    // labelmapper
    template <typename T, typename Y>
    class umap {

        // on top for visibility; default private
        typedef typename std::unordered_map<T, Y>::iterator umap_iterator;
        typedef typename std::unordered_map<T, Y>::const_iterator umap_const_iterator;

    public:
        umap() {};
        umap(const std::unordered_map<T, Y>& umapobj) : _umap(umapobj) {};

 
        Y& at(T& argtype1) { return _umap.at(argtype1); };
        const Y& at(const T& argtype1) const { return _umap.at(argtype1); };

        void insert(T index, Y output) { _umap.insert(std::make_pair(index, output)); };
        void insert(std::pair<T, Y> input) { _umap.insert(input); };

        void clear() { _umap.clear(); };

        bool empty() { return _umap.empty(); };

        //see: https://stackoverflow.com/questions/60459011/contains-is-not-a-member-of-stdmap-msvc
        bool contains(const T& target) const { return (_umap.count(target) == 1) ? true : false; };// requires C++20 standard { return _umap.contains(target); }

        const uint_64 size() const { return _umap.size(); };

        umap_iterator erase(umap_iterator pos) { return _umap.erase(pos); };
        umap_const_iterator erase(umap_const_iterator pos) { return _umap.erase(pos); };
        umap_const_iterator erase(umap_const_iterator first, umap_const_iterator last) { return _umap.erase(first, last); };
        const uint_64 erase(const T& target) { return _umap.erase(target); };

        umap_iterator find(const T& target) { return _umap.find(target); };
        umap_const_iterator find(const T& target) const { return _umap.find(target); };

        umap_iterator begin() noexcept { return _umap.begin(); };
        umap_iterator end() noexcept { return _umap.end(); };



    private:
        std::unordered_map<T, Y> _umap;
    };


	

}