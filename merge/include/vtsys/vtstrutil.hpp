#ifndef VTSTRUTIL_HPP_INCLUDED
#define VTSTRUTIL_HPP_INCLUDED

/**  2015.07 Dehua Tan **/

#include <string>
#include <functional>
#include <algorithm>

#include "vtsys/vtdef.h"

namespace vtsys{

	inline std::string& ltrim(std::string& s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
		return s;
	}

	inline std::string& rtrim(std::string& s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
		return s;
	}

	inline std::string& trim(std::string& s) {
		return ltrim(rtrim(s));
	}

	inline std::string& tolower(std::string& str){
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);
		return str;
	}

	inline std::string& toupper(std::string& str){
		std::transform(str.begin(), str.end(), str.begin(), ::toupper);
		return str;
	}

	bool isdigits(const std::string& str);
	bool isxdigits(const std::string& str);

	/**
	 * for toBool():
	 * 		return true if param str is one of "true", "on", "yes", "ok", "t", ".t." for case insensitive.
	 * 		return true if param str are all digits, and (toInt64(str) > 0).
	 * 		return false otherwise.
	 **/
	bool toBool(const std::string& str);
	int8_t toInt8(const std::string& str);
	uint8_t toUint8(const std::string& str);
	int16_t toInt16(const std::string& str);
	uint16_t toUint16(const std::string& str);
	int32_t toInt32(const std::string& str);
	uint32_t toUint32(const std::string& str);
	int64_t toInt64(const std::string& str);
	uint64_t toUint64(const std::string& str);
	float toFloat(const std::string& str);
	double toDouble(const std::string& str);
	long double toLongDouble(const std::string& str);

	int64_t kmgtpStrToInt64(const std::string& str);
	uint64_t kmgtpStrToUint64(const std::string& str);

	std::string& toBinString(std::string& str, const uint64_t i);
	std::string& toBinString(std::string& str, const int64_t i);
	std::string& toBinString(std::string& str, const uint32_t i);
	std::string& toBinString(std::string& str, const int32_t i);
	std::string& toBinString(std::string& str, const uint16_t i);
	std::string& toBinString(std::string& str, const int16_t i);
	std::string& toBinString(std::string& str, const uint8_t i);
	std::string& toBinString(std::string& str, const int8_t i);

	std::string& toOctString(std::string& str, const uint64_t i);
	std::string& toOctString(std::string& str, const int64_t i);
	std::string& toOctString(std::string& str, const uint32_t i);
	std::string& toOctString(std::string& str, const int32_t i);
	std::string& toOctString(std::string& str, const uint16_t i);
	std::string& toOctString(std::string& str, const int16_t i);
	std::string& toOctString(std::string& str, const uint8_t i);
	std::string& toOctString(std::string& str, const int8_t i);

	std::string& toHexString(std::string& str, const uint64_t i);
	std::string& toHexString(std::string& str, const int64_t i);
	std::string& toHexString(std::string& str, const uint32_t i);
	std::string& toHexString(std::string& str, const int32_t i);
	std::string& toHexString(std::string& str, const uint16_t i);
	std::string& toHexString(std::string& str, const int16_t i);
	std::string& toHexString(std::string& str, const uint8_t i);
	std::string& toHexString(std::string& str, const int8_t i);

	std::string& toDecString(std::string& str, const uint64_t i);
	std::string& toDecString(std::string& str, const int64_t i);
	std::string& toDecString(std::string& str, const uint32_t i);
	std::string& toDecString(std::string& str, const int32_t i);
	std::string& toDecString(std::string& str, const uint16_t i);
	std::string& toDecString(std::string& str, const int16_t i);
	std::string& toDecString(std::string& str, const uint8_t i);
	std::string& toDecString(std::string& str, const int8_t i);
}

#endif // VTSTRUTIL_HPP_INCLUDED
