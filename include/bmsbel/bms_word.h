#ifndef BMSBEL_WORD_H_
#define BMSBEL_WORD_H_

#include <string>
#include <iostream>

// -- BmsWord ------------------------------------------------------------
class BmsWord {
public:
	static const BmsWord MIN;
	static const BmsWord MAX;

	static bool CheckConstruction(const char* str);
	static bool CheckConstruction(const std::string& str);
	static bool CheckConstruction(const wchar_t* str);
	static bool CheckConstruction(const std::wstring& str);

public:
	BmsWord();
	BmsWord(int value);
	BmsWord(char msb, char lsb);
	BmsWord(const char* str);
	BmsWord(const std::string& str);
	BmsWord(const wchar_t* str);
	BmsWord(const std::wstring& str);

	~BmsWord();

	void SetByWord(const BmsWord& word);
	void SetByInteger(int value);
	void SetByChar(char msb, char lsb);

	int ToInteger(void) const;
	char GetMsb(void) const;
	char GetLsb(void) const;
	const char* ToCharPtr(void) const;
	std::string ToString(void) const;
	std::wstring ToWString(void) const;

	void CheckValid(void) const;

	void operator ++(void);
	void operator ++(int);
	void operator --(void);
	void operator --(int);

	bool equal(const BmsWord &obj) const;
	bool operator ==(const BmsWord &obj) const;
	bool operator !=(const BmsWord &obj) const;

	int compare(const BmsWord &obj) const;
	bool operator <=(const BmsWord &obj) const;
	bool operator <(const BmsWord &obj) const;
	bool operator >=(const BmsWord &obj) const;
	bool operator >(const BmsWord &obj) const;

	friend std::ostream& operator <<(std::ostream& os, const BmsWord& word);

private:
	int value_;
};

#endif // BMSBEL_WORD_H_
