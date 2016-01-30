#ifndef BMSBEL_BUFFER_H_
#define BMSBEL_BUFFER_H_

#include <string>
#include <map>
#include <set>

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
	BmsBuffer();
	BmsBuffer(int length, const std::string& input);

	virtual ~BmsBuffer();

	unsigned int GetObjectCount() const;
	unsigned int GetObjectCount(barindex start, barindex length = -1) const;
	void GetObjectExistBar(std::set<barindex> &barmap) const;

	/** @brief safely get value from array. */
	BmsWord Get(barindex pos) const;
	/** @brief this method only for Get() method. use Set() method to set value. */
	//const BmsWord& operator [](unsigned int pos);
	/** @brief set value at pos. */
	void Set(barindex pos, BmsWord value);

	void DeleteAt(barindex pos);

	void Clear(void);

	/** @brief appends BmsBuffer to the start of this buffer */
	virtual void Merge(const BmsBuffer& buffer);
	/** @brief merge buffer data from start position (it doesn't make buffer longer) */
	virtual void Merge(barindex start, const BmsBuffer& buffer);
	/** @brief cut note data at from ~ to position */
	BmsBuffer SubBuffer(barindex from, barindex length) const;

	bool Contains(const BmsWord &word) const;

	void MagnifyBy(double multiplier);

	std::string ToString(void) const;

	// -- Iterator ---------------------------------------------------------
	typedef std::map<barindex, BmsWord>::iterator Iterator;
	typedef std::map<barindex, BmsWord>::const_iterator ConstIterator;
	Iterator Begin();
	Iterator End();
	ConstIterator Begin() const;
	ConstIterator End() const;
	/** @brief get iterator of current note. if not exists, then returns next note. */
	Iterator Begin(barindex startbar);
	ConstIterator Begin(barindex startbar) const;
private:
	/** @brief contains (bar index, keyvalue) */
	std::map<barindex, BmsWord> array_;
};

#endif // BMSBEL_BUFFER_H_
