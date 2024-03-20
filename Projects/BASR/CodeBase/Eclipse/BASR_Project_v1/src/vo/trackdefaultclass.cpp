/*
 * trackdefaultclass.cpp
 *
 *  Created on: Nov 20, 2018
 *      Author: hassan
 */

#define TRACKDEFAULTCLASS_CPP_

#include "../vo/trackdefaultclass.h"

//default constructor
TrackDefaultClass::TrackDefaultClass()
{
	this->last.set_capacity(0);
}

TrackDefaultClass::~TrackDefaultClass()
{
	// TODO Auto-generated destructor stub
}
void TrackDefaultClass::serialize(SerialBufferClass &buffer)
{
	buffer.serialize(angle, 0);
	buffer.alignToNBytes(16);
	buffer.serialize(general, 0);
	buffer.alignToNBytes(16);
	buffer.serialize(intrapulse, 0);
	buffer.alignToNBytes(16);
	buffer.serialize(power, 0);
	buffer.alignToNBytes(16);
	buffer.serialize(pri, 0);
	buffer.alignToNBytes(16);
	buffer.serialize(pw, 0);
	buffer.alignToNBytes(16);
	buffer.serialize(rf, 0);
	buffer.alignToNBytes(16);

}
bool TrackDefaultClass::compareTrackToas(const TrackDefaultClass &first,
		const TrackDefaultClass &second)
{
	return first.general.toaLast < second.general.toaLast;
}
