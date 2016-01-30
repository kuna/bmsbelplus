#ifndef BMSBEL_UTIL_H_
#define BMSBEL_UTIL_H_

#include <string>
#include <vector>

#include "bms_word.h"

// -- BmsUtil ------------------------------------------------------------
namespace BmsUtil {
	char CharacterToNumber(char c);

	bool IsDigit(char c);
	bool IsNotDigit(char c);
	bool IsAlphabet(char c);
	bool IsNotAlphabet(char c);
	bool IsHex36(char c);
	bool IsNotHex36(char c);

	bool StringToInteger(const std::string& str, int* ret, unsigned int base);
	bool StringToFloat(const std::string& str, double* ret);
	bool StringToInteger(const std::wstring& str, int* ret, unsigned int base);
	bool StringToFloat(const std::wstring& str, double* ret);

	char IntegerToHex36(int x);

	barindex GCD(barindex x, barindex y);
	barindex LCM(barindex x, barindex y);
	unsigned int GetDenominator(double x, unsigned int max);

	void StringToUpper(std::string& str);
	void StringToUpper(std::wstring& str);
	
	bool wchar_to_utf8(const wchar_t *org, char *out, int maxlen);
	bool utf8_to_wchar(const char *org, wchar_t *out, int maxlen);
	bool convert_to_utf8(const char *org, char *out, const char *encoding, int maxlen);
	int IsFileUTF8(const std::string& filename);
	int IsUTF8(const char* text);
	bool OpenFile(FILE **f, const char* filename, const char* mode);
};

#endif // BMSBEL_UTIL_H_
