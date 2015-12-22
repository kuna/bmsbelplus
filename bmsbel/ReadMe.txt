# bmsbel

BMS library for C++

code from http://childs.squares.net/program/bmsbel/index.html

modifed by @lazykuna

original by CHILD, all rights reserved.

### additional modification:
- compilable /w vc++ compiler
- documentation (processing)
- support ```#LNTYPE1```
- (TODO) UTF8/auto-encode support (additional bmsext suggestion from (http://cosmic.mearie.org/f/sonorous/bmsexts)[Kang Seonghoon])
- (TODO) proper supporting of #000 measure note
- (TODO) change comments into english
- (TODO) code coverage
- (TODO) support #LNTYPE2 (we won't support it currently as it's depreciated)

### little documentation
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