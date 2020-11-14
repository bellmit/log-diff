#ifndef VTHASH_HPP_INCLUDED
#define VTHASH_HPP_INCLUDED

#include "vtdiff/vtdiffcontext.h"


#include <stdint.h>
#include <stdlib.h>

namespace vtdiff{

	typedef const unsigned char* cucstr_t;

	static inline uint32_t& fnv_32_buf(uint32_t& hval, cucstr_t buf, const uint64_t len){
		cucstr_t bp = buf;
		cucstr_t be = bp + len;
		while(bp < be){
			hval += (hval<<1) + (hval<<4) + (hval<<7) + (hval<<8) + (hval<<24);
			hval ^= *bp++;
		};
		return hval;
	}

	static inline uint32_t& fnv_32_chr(uint32_t& hval, const uint8_t chr)	{
		hval += (hval<<1) + (hval<<4) + (hval<<7) + (hval<<8) + (hval<<24);
		hval ^= chr;
		return hval;
	}

	static inline uint32_t& fnv_32_str(uint32_t& hval, cucstr_t ucstr)	{
		cucstr_t s = ucstr;
		while(*s){
			hval += (hval<<1) + (hval<<4) + (hval<<7) + (hval<<8) + (hval<<24);
			hval ^= *s++;
		};
		return hval;
	}

	static inline uint32_t& fnv_32a_buf(uint32_t& hval, cucstr_t buf, const uint64_t len)	{
		cucstr_t bp = buf;
		cucstr_t be = bp + len;
		while (bp < be) {
			hval ^= *bp++;
			hval += (hval<<1) + (hval<<4) + (hval<<7) + (hval<<8) + (hval<<24);
		};
		return hval;
	}

	static inline uint32_t& fnv_32a_chr(uint32_t& hval, const uint8_t chr)	{
		hval ^= chr;
		hval += (hval<<1) + (hval<<4) + (hval<<7) + (hval<<8) + (hval<<24);
		return hval;
	}

	static inline uint32_t& fnv_32a_str(uint32_t& hval, cucstr_t ucstr)	{
		cucstr_t s = ucstr;
		while(*s){
			hval ^= *s++;
			hval += (hval<<1) + (hval<<4) + (hval<<7) + (hval<<8) + (hval<<24);
		};
		return hval;
	}

	static inline uint64_t& fnv_64_buf(uint64_t& hval, cucstr_t buf, const uint64_t len)	{
		cucstr_t bp = buf;
		cucstr_t be = bp + len;
		while(bp < be){
			hval += (hval << 1) + (hval << 4) + (hval << 5) +
				(hval << 7) + (hval << 8) + (hval << 40);
			hval ^= *bp++;
		};
		return hval;
	}

	static inline uint64_t& fnv_64_chr(uint64_t& hval, const uint8_t chr)	{
		hval += (hval << 1) + (hval << 4) + (hval << 5) +
			(hval << 7) + (hval << 8) + (hval << 40);
		hval ^= chr;
		return hval;
	}

	static inline uint64_t& fnv_64_str(uint64_t& hval, cucstr_t str)	{
		cucstr_t s = str;
		while(*s){
			hval += (hval << 1) + (hval << 4) + (hval << 5) +
				(hval << 7) + (hval << 8) + (hval << 40);
			hval ^= *s++;
		};
		return hval;
	}

	static inline uint64_t& fnv_64a_buf(uint64_t& hval, cucstr_t buf, const uint64_t len)	{
		cucstr_t bp = buf;
		cucstr_t be = bp + len;
		while(bp < be){
			hval ^= *bp++;
			hval += (hval << 1) + (hval << 4) + (hval << 5) +
				(hval << 7) + (hval << 8) + (hval << 40);
		};
		return hval;
	}

	static inline uint64_t& fnv_64a_chr(uint64_t& hval, const uint8_t chr) {
		hval ^= chr;
		hval += (hval << 1) + (hval << 4) + (hval << 5) +
			(hval << 7) + (hval << 8) + (hval << 40);
		return hval;
	}

	static inline uint64_t& fnv_64a_str(uint64_t& hval, cucstr_t str) {
		cucstr_t s = str;
		while(*s){
			hval ^= *s++;
			hval += (hval << 1) + (hval << 4) + (hval << 5) +
				(hval << 7) + (hval << 8) + (hval << 40);
		};
		return hval;
	}

	typedef uint32_t fixed_hash_table_32_t[256];
	typedef uint64_t fixed_hash_table_64_t[256];

	static inline void init_fixed_hash_table(fixed_hash_table_32_t& fixed_hash_table){
		uint32_t hval = 2166136261U;
		for(uint32_t i = 0; i < 256; i ++)
			fixed_hash_table[i] = fnv_32a_chr(hval, (uint8_t)i);
	}

	static inline void init_fixed_hash_table(fixed_hash_table_64_t& fixed_hash_table){
		uint64_t hval = 14695981039346656037LLU;
		for(uint32_t i = 0; i < 256; i ++)
			fixed_hash_table[i] = fnv_64a_chr(hval, (uint8_t)i);
	}

	/**
	 *  left shift hash with 32 bytes history
	 **/
	static inline uint32_t& fnv_ls(uint32_t& hval, const fixed_hash_table_32_t& fixed_hash_table, const uint8_t chr){
		return (hval = (hval << 1) ^ fixed_hash_table[chr]);
	}

	static inline uint32_t& fnv_32_ls(uint32_t& hval, const uint8_t chr) {
	    hval ^= chr;

        //  left shift hash with 32 bytes history
		//return (hval = ((hval<<1) + (hval<<4) + (hval<<7) + (hval<<8) + (hval<<24)) ^ chr);

	    /** FIX BUG on Nov 25, 2015 **/
		//left shift hash with 8 bytes history
		//return (hval = ((hval<<4) + (hval<<7) + (hval<<8) + (hval<<24)) ^ chr);
        
	    /** Enhancement on Dec 26, 2015 **/
        return (hval = ((hval << vt_diff_fnv_32_ls_bits) + (hval << 7) + (hval << 8) + (hval << 24)) ^ chr);
	}

	/**
	 *  left shift hash with 64 bytes histroy
	 **/
	static inline uint64_t& fnv_ls(uint64_t& hval, const fixed_hash_table_64_t& fixed_hash_table, const uint8_t chr){
		return (hval = (hval << 1) ^ fixed_hash_table[chr]);
	}

	/**
	 *  left shift hash with 64 bytes histroy
	 **/
	static inline uint64_t& fnv_64_ls(uint64_t& hval, const uint8_t chr) {
	    hval ^= chr;
		return (hval = ((hval << 1) + (hval << 4) + (hval << 5) + (hval << 7) + (hval << 8) + (hval << 40)) ^ chr);
	}

	/**
	 * left cycle shift hash for 32 bits
	**/
	static inline uint32_t& left_cycle_shift_hash(uint32_t& hval, const fixed_hash_table_32_t& fixed_hash_table, const uint8_t chr){
		return (hval = ((hval << 1) | (hval >> 31)) ^ fixed_hash_table[chr]);
	}

	/**
	 * left cycle shift hash for 64 bits
	**/
	static inline uint64_t& left_cycle_shift_hash(uint64_t& hval, const fixed_hash_table_64_t& fixed_hash_table, const uint8_t chr){
		return (hval = ((hval << 1) | (hval >> 63)) ^ fixed_hash_table[chr]);
	}

}


#endif // VTHASH_HPP_INCLUDED
