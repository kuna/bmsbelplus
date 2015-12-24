# BmsBel+

BMS library for C++

code from http://childs.squares.net/program/bmsbel/index.html

modifed by @lazykuna

original by CHILD, all rights reserved.

### additional modification:
- compilable /w vc++ compiler
- documentation (processing)
- support ```#LNTYPE 1```
- UTF8/auto-encode support (additional bmsext suggestion from (http://cosmic.mearie.org/f/sonorous/bmsexts)[Kang Seonghoon])
- utilities added: IsUTF8, Alternative File Searcher
- (TODO) dropping specific Channel Available
- (TODO) (R-/H-/S-)RANDOM, MIRROR, ALL-LN, NO_LN, ALL-SCR, MORENOTE, LESSNOTE, SPEED_MULTIPLY, MOREMINE, NOMINE, SP-TO-DP option available
- (TODO) nested #RANDOM statement
- (TODO) support ```#LNTYPE 2``` (we won't support it currently as it's depreciated)
- (TODO) use gradle
- (TODO) change comments into english
- (TODO) code coverage

### just a little documentation ...
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
  - class BmsBarManager		// ?
    - class BmsBar			// ?
  - class BmsTimeManager	// contains definite time, pos, beat, etc data for each row(relative to BmsBar)
```
- Loading bms file
```

```

#### get sound, bitmap or etc tag datas
```
```

#### get note data for game play
```
// call this after bms file loaded
```

### 

### Additional standard suggestion
- Supporting UTF8 file format
  - *.bmx file formats are old, and most of them uses Shift_JIS. and that makes strings displayed wrong in Non_JP(?) language OS.
  - First, attempt to convert first 10 lines as UTF8.
  - If failes, then consider that file as Shift_JIS. (Fallback)
- Extended BMS Word (NOT IMPLEMENTED)
  - Uses 4 base36-character for a word.
  - *.bmx file will have almost x2 size than previous format.
  - That will make BMS composer to add more audio/image(video) files.
  - That will also make merging bms files easiler, like LR2\'s marathon mode.
  - To use this function, write ```#EXTENDWORD 1``` to BMS header.
- LongNote (GAME FEATURE)
  - Inherits O2Jam feature - that is, you have to 
  - We may have 2 judges - LN start(press), LN end(up).
  - If start judge failes, then 2 miss will be generated.
  - If LN end judge failed, then 1 miss and start judge will be generated.
- Scratch LongNote (GAME FEATURE)
  - Have to press one more key after scratching longnote.
  - So, we may have 3 judges - LN start(press), LN end(up), LN end(press)
  - If start judge failes, then 2 miss will be generated.
  - If LN end(up or press) judge failes, then 1 miss and start judge will be generated.
- Hell LongNote (GAME FEATURE)
  - Combo is continually up during pressing longnote.
  - To use Hell LongNote, write ```#LNHELL 1``` to BMS header.
  - 4 Combo per a bar(measure).