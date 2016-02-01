# BmsBel+

[![travis](https://travis-ci.org/kuna/bmsbelplus.svg)](https://travis-ci.org/kuna/bmsbelplus)
[![Build status](https://ci.appveyor.com/api/projects/status/ik95v5ne5hssq85j?svg=true)](https://ci.appveyor.com/project/kuna/bmsbelplus)

BMS library for C++

modifed by @lazykuna

original code from http://childs.squares.net/program/bmsbel/index.html by CHILD, all rights reserved.

### additional modification:
- compilable /w vc++ compiler
- support ```#LNTYPE 1```
- support ```#LNTYPE 2```
- parse/process #BGA, #STP definition
- UTF8/auto-encode support (additional bmsext suggestion from (http://cosmic.mearie.org/f/sonorous/bmsexts)[Kang Seonghoon])
- (R-/H-/S-)RANDOM, MIRROR, ALL-LN, NO_LN, ALL-SCR, MORENOTE, LESSNOTE, SPEED_MULTIPLY, MOREMINE, NOMINE, SP-TO-DP, DP-TO-SP, BATTLE, Training(copy and repeat) option available
- Save function implementation
- Shift-JIS code to UTF8
- nested #RANDOM statement
- code coverage (processing)
- documentation (processing)
- compilable with g++/clang++ (c++11)
- (TODO) create bmsbgmmanager / bmsbga object?
- (TODO) git wiki documentation

### just a little documentation ...
  Temporary documentation before wiki is created ..

- Class hierarchery
```
namespace BmsParser
class BmsBms
  - class BmsHeaderTable    // contains #TITLE, #BPM, etc ...
  - class BmsRegistArraySet	// contains #WAVxx, #BMPxx data
  - class BmsChannelManager	// <channel, BmsChannel> data
    - class BmsChannel      // contains each channel
    - class BmsBuffer       // BmsChannel's real note data (single BGM channel can have multiple BmsBuffer)
  - class BmsBarManager		  // contains bar(measure) ratio, and convert bar ~ time/pos.
  - class BmsTimeManager    // converts time ~ bar
  - class BmsSTPManager     // contains #STP information
  -
class BmsParser::Parser     // used for parsing Bms file
```
- Loading/Saving bms file (Automatically finds encoding)
```
// Without parsing RANDOM statement
BmsBms bms;
BmsParser::Parser *parser = new BmsParser::Parser(bms, { false, rand() });
bool r = parser->Load("filepath.bms");
delete parser;

// Setting custom seed for RANDOM statement
BmsBms bms;
BmsParser::Parser *parser = new BmsParser::Parser(bms, { false, 123 });
bool r = parser->Load("filepath.bms");
delete parser;

// Portable method (internally creates BmsParser::Parser object)
BmsBms bms;
bms.LoadBms("filepath.bms");

// Save
bms.SaveBms("outfilepath.bms");
```

- get sound, bitmap or etc tag datas
```
// get Header
std::string title = "(none)";
bms.GetHeader().Query("TITLE", title);
//printf("Title: %s", bms.GetHeader()["TITLE"]);
// get Resource
BmsWord word("A0");
if (bms.GetRegistArraySet()["WAV"].IsExists(word))
	printf("WAV path of #WAVA0: %s", bms.GetRegistArraySet()["WAV"][word]);
```

- get data for game play
```
/*
 * You should change beat/time into internal bar position
 * which library internally uses.
 */
BmsBms bms;
bms.LoadBms("test.bms");
BmsNoteContainer note;
bms.GetNoteData(note);

int tick = GetTick();

int bar = bms.GetTimeManager().GetBarFromTime(tick);
for (auto it = note[lane].Begin(bar); it != note[lane].End(); ++it) {
  int pos = bms.GetBarManager().GetPosFromBar(it->first);
  if (pos > screen_height) break;
  RenderNote(lane, pos);
}
```

### etc
  - Total structure is really different from original library BmsBel so it's not compatible from original one. 
  - Performance: Most of the time, BmsBel+ works more fast and have more compatibility. *a bit slower in a very very large file - over 4MiB?*
  - It has limitation in decomposing note from buffer engine structure.
    You can change resolution in ```bms_define.h```.
  - bmx2ogg / SimpleBmxPlayer were made using this library.