# BmsBel+

BMS library for C++

modifed by @lazykuna

original code from http://childs.squares.net/program/bmsbel/index.html by CHILD, all rights reserved.

### additional modification:
- compilable /w vc++ compiler
- documentation (processing)
- support ```#LNTYPE 1```
- support ```#LNTYPE 2
- parse/process #BGA, #STP definition
- UTF8/auto-encode support (additional bmsext suggestion from (http://cosmic.mearie.org/f/sonorous/bmsexts)[Kang Seonghoon])
- (R-/H-/S-)RANDOM, MIRROR, ALL-LN, NO_LN, ALL-SCR, MORENOTE, LESSNOTE, SPEED_MULTIPLY, MOREMINE, NOMINE, SP-TO-DP option available
- Save function implementation
- Shift-JIS code to UTF8
- nested #RANDOM statement
- (TODO) create bmsbgmmanager / bmsbga object
- (TODO) support creating training pattern
- (TODO) compile with MinGW
- (TODO) code coverage
- (TODO) git wiki documentation

### just a little documentation ...
  Temporary documentation before wiki is created ..

- Class hierarchery
```
namespace BmsParser
  - class StartInfo			// staring argument for parsing
  - class ParsingInfo		// status of current parsing
  - class Parser			// a static-like class used during parsing BMS file
class BmsBms
  - class BmsHeaderTable	// contains #TITLE, #BPM, etc ...
  - class BmsRegistArraySet	// contains #WAV, #BMP data
  - class BmsChannelManager	// <channel, BmsChannel> data
    - class BmsChannel		// if there's duplicated channel, then it's stacked to here.
  - class BmsBarManager		// has information about each bar(measure) - such as length
    - class BmsBar			// -
// under these are extracted information from BmsBms
class BmsTimeManager	// contains definite time, pos, beat, etc data for each row(relative to BmsBar). read-only.
class BmsNoteManager	// contains about pure note data - Normal, Longnote, Mine, Invisible. read-only.
```
- Loading bms file
```
// with specific encoding
BmsBms bms;
BmsParser::Parse(L"path-to-bms.bms", "Shift_JIS", bms);

// with auto encoding
BmsBms bms;
BmsParser::Parse(L"path-to-bms.bms", bms);
```

- get sound, bitmap or etc tag datas
```
// get Header
if (bms.GetHeader().IsExists(L"TITLE"))
	wprintf(L"Title: %ls", bms.GetHeader()[L"TITLE"])
// get Resource
BmsWord word("A0");
if (bms.GetRegistArraySet()[L"WAV"].IsExists(word))
	wprintf(L"WAV path of #WAVA0: %ls", bms.GetRegistArraySet()[L"WAV"][word]);
```

- get data for game play
```
BmsTimeManager bms_time;
BmsNoteContainer bms_note;
bms.CalculateTime(bms_time);
bms.GetNotes(bms_note);

for (int i = 0; i <= bms.GetPlayableMaxPosition(); i++) {
	// scan buffer / timetable / and so on~
}
```

- edit note & save for BMSE
```
// edit bms buffer
// save function is not implemented~
``` 

### Problems / etc
  - It has limitation in decomposing note from buffer engine structure.
    Currently, It can divide each measure into 1/19200.
  - bmx2ogg / SimpleBmxPlayer were made using this library.