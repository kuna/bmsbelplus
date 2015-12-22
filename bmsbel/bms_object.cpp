#include <bmsbel\bms_object.h>

int BmsObject::NOTE_NORMAL	= 0;
int BmsObject::NOTE_LN		= 1;
int BmsObject::NOTE_BOMB	= 2;
int BmsObject::MEASURE		= 3;

BmsObject::BmsObject(int type, int channel, BmsWord value, double beat, double time, double absbeat) :
type(type),
channel(channel),
beat(beat),
time(time),
value(value),
absbeat(absbeat) {}

int BmsObject::GetKey() {
	return channel % 36;
}