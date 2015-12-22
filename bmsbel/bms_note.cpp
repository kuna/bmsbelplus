#include "bmsbel\bms_note.h"

int BmsNote::NOTE_NORMAL = 0;
int BmsNote::NOTE_LN = 1;
int BmsNote::NOTE_BOMB = 2;

BmsNote::BmsNote(int type, int channel, BmsWord value, double beat, double time, double absbeat) :
type(type),
channel(channel),
beat(beat),
time(time),
value(value),
absbeat(absbeat) {}