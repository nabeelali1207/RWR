/*
 * serializeobject.cpp
 *
 *  Created on: Oct 14, 2019
 *      Author: gmd
 */

#include "serializeobject.h"

uint32_t SerializeClass::calculateCRCwithoutHeader()
{
	uint8_t * tempPtr = (uint8_t *) sb.getRawBufferPointer();
	tempPtr = tempPtr + sizeof(SerialHeader);

	return Utility::calculateCRC32(tempPtr,
			header.messageSize - sizeof(SerialHeader));
}
void SerializeClass::serializeHeader()
{
	header.messageSize = sb.getPosition();
	header.messageCRC = calculateCRCwithoutHeader();
	header.systemTime = Utility::getSystemTime();

	sb.seekPosition(0); // write header to start of the message
	sb.serialize(header);
}

void SerializeClass::setMessageName(const char name[8])
{
	memcpy(header.messageName, name, sizeof(header.messageName));
}

uint8_t * SerializeClass::getSerializedMessagePointer()
{
	return (uint8_t *) sb.getRawBufferPointer();
}
uint32_t SerializeClass::getMaxBufferSize()
{
	return sb.getMaxBufferSize();
}
uint32_t SerializeClass::getSerializedMessageSize()
{
	return header.messageSize;
}
