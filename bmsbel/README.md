# BmsBel+

BMS library for C++

modifed by @lazykuna

original code from http://childs.squares.net/program/bmsbel/index.html by CHILD, all rights reserved.

### additional modification:
- compilable /w vc++ compiler
- documentation (processing)
- support ```#LNTYPE 1```
- UTF8/auto-encode support (additional bmsext suggestion from (http://cosmic.mearie.org/f/sonorous/bmsexts)[Kang Seonghoon])
- (TODO) (R-/H-/S-)RANDOM, MIRROR, ALL-LN, NO_LN, ALL-SCR, MORENOTE, LESSNOTE, SPEED_MULTIPLY, MOREMINE, NOMINE, SP-TO-DP option available
- (TODO) nested #RANDOM statement
- (TODO) tidier code ...
- (TODO) support ```#LNTYPE 2``` (we won't support it currently as it's depreciated)
- (TODO) compile with MinGW
- (TODO) save function implementation
- (TODO) change comments into english
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

### Additional standard suggestion
- Supporting UTF8 file format
  - *.bmx file formats are old, and most of them uses Shift_JIS. and that makes strings displayed wrong in Non_JP(?) language OS.
  - First, attempt to convert first 10 lines as UTF8.
  - If failes, then consider that file as Shift_JIS. (Fallback)
- Extended BMS Word *(NOT IMPLEMENTED)*
  - Uses 4 base36-character for a word.
  - *.bmx file will have almost x2 size than previous format.
  - That will make BMS composer to add more audio/image(video) files.
  - That will also make merging bms files easiler, like LR2\'s marathon mode.
  - To use this function, write ```#EXTENDWORD 1``` to BMS header.
- LongNote *(GAME FEATURE)*
  - Inherits O2Jam feature - that is, you have to key up at the end of the LongNote. If not, Combo break and Guage lose.
  - We may have 2 judges - LN start(press), LN end(up).
  - If start judge failes, then 2 miss will be generated.
  - If LN end judge failed, then 1 miss and start judge will be generated.
- Scratch LongNote *(GAME FEATURE)*
  - Have to press one more key after scratching longnote.
  - So, we may have 3 judges - LN start(press), LN end(up), LN end(press)
  - If start judge failes, then 2 miss will be generated.
  - If LN end(up or press) judge failes, then 1 miss and start judge will be generated.
- Hell LongNote *(GAME FEATURE)*
  - Combo is continually up during longnote pressing.
  - To use Hell LongNote, write ```#LNHELL 1``` to BMS header.
  - 4 Combo per a bar(measure).

### Problems / etc
  - It has limitation in decomposing note from buffer engine structure.
    Currently, It can divide each measure into 1/19200.