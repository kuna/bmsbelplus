#ifndef BMSBEL_BUFFER_H_
#define BMSBEL_BUFFER_H_

#include <string>
#include <map>

#include "bms_word.h"
#include "bms_define.h"

/*
 * @description
 * stores note data(a single line of lane), indexed as bar number.
 * this class is different from bms_channel(which stores a lane).
 * ex: BmsChannel can store a channel (like #01),
 *     but BmsChannel is consisted with multiple BmsBuffer.
 *     that makes BmsBel support multiple same channel.
 */

/*
 * @comment
 * We can make more precision by making each buffer consisted with measure objects, includes note objects.
 * (current model directly includes note objects)
 * but that requires quite much cost when iterating.
 * What about using beat(double) as key instead of channelbar?
 * - That's also a problem when we gives note modifying option like RANDOM or something else ...
 * and over 19600? precision gives almost no difference in real play.
 * so, we limit our resolution. that's the most better solution I think.
 */

class BmsBuffer {
public:
	BmsBuffer(int length = BmsConst::BAR_DIVISION_COUNT_MAX);
	BmsBuffer(int length, const std::string& input);

	virtual ~BmsBuffer();

	unsigned int GetLength(void) const;
	unsigned int GetCount(void) const;

	/** @brief safely get value from array. */
	BmsWord Get(unsigned int pos) const;
	/** @brief this method only for Get() method. use Set() method to set value. */
	//const BmsWord& operator [](unsigned int pos);
	/** @brief set value at pos. */
	void Set(unsigned int pos, BmsWord value);

	void DeleteAt(unsigned int pos);

	void Clear(void);

	/** @brief appends BmsBuffer to the start of this buffer */
	virtual void Merge(const BmsBuffer& buffer);
	/** @brief merge buffer data from start position (it doesn't make buffer longer) */
	virtual void Merge(unsigned int start, const BmsBuffer& buffer);
	/** @brief insert buffer data (it WILL make buffer longer) */
	virtual void Append(const BmsBuffer& buffer);
	/** @brief cut note data at from ~ to position */
	BmsBuffer SubBuffer(unsigned int from, unsigned int length) const;

	bool Contains(const BmsWord &word) const;

	void MagnifyBy(unsigned int multiplier);

	std::string ToString(void) const;

	// -- Iterator ---------------------------------------------------------
	typedef std::map<int, BmsWord>::iterator Iterator;
	typedef std::map<int, BmsWord>::const_iterator ConstIterator;
	Iterator Begin(void);
	Iterator End(void);
	ConstIterator Begin(void) const;
	ConstIterator End(void) const;
	/** @brief reset iterator position to startbar. return End() if no proper element exists. */
	Iterator Reset(int startbar = 0);
	/** @brief get next iterator. */
	Iterator Next();
	/** @brief get current iterator. */
	Iterator Current();
private:
	/** @brief contains (bar index, keyvalue) */
	std::map<int, BmsWord> array_;
	/** @brief indicates bar's total size. used for Append/Insert. */
	int length_;
	/** @brief current bar's position. used for finding / iterating. */
	Iterator iter_;
};

#endif // BMSBEL_BUFFER_H_
