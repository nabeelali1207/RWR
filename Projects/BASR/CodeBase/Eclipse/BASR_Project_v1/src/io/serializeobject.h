/*
 * serializeobject.h
 *
 *  Created on: Oct 9, 2019
 *      Author: gmd
 */

#include "../io/serialbuffer.h"
#include "../common/common.h"
#include "../utility/utility.h"

#ifndef SERIALIZEOBJECT_H_
#define SERIALIZEOBJECT_H_

enum class SerializationStatus
{
	SUCCESS,
	FAILURE_CRC_MISMATCH,
	FAILURE_MESSAGE_SIZE_LESS,
	FAILURE_MESSAGE_SIZE_MORE,
	FAILURE_HEADER_MISMATCH
};
struct SerialHeader
{
	uint64_t delimeter;
	uint32_t messageSize;
	uint32_t messageCRC;
	uint8_t messageName[8];
	uint64_t systemTime;
	uint64_t systemIteration;
};
class SerializeClass
{
private:
	SerialHeader header;
	SerialBufferClass sb;

	uint32_t calculateCRCwithoutHeader();
	void serializeHeader();
	void setMessageName(const char name[8]);
public:
	template<class T>
	bool serializeClass(T & obj, const char name[8])
	{
		sb.setStreamType(SerialBufferStreamTypeEnum::InputStream);
		setMessageName(name);

		header.delimeter = 0xdeadbeefbeeffeed;
		header.systemTime = 0;
		header.systemIteration = 0;
		header.messageSize = 0;

		sb.seekPosition(sizeof(SerialHeader));
		sb.serialize(obj,0);
		sb.alignToNBytes(64);

		serializeHeader();
		return sb.isConsistent();
	}

	template<class T>
	bool deSerializeClass(T & obj, const char name[8], uint64_t bytesReceived)
	{
		sb.setStreamType(SerialBufferStreamTypeEnum::OutputStream);
		bool headerValid = (bytesReceived >= sizeof(SerialHeader));

		if (headerValid == false)
		{
			return false;
		}
		sb.serialize(header);

//		printf("TP Rec: %d B\n", header.messageSize);
		bool isMessageClassSame = (strcmp((const char *) header.messageName,
				name) == 0);
		bool doesSizeOverflow = header.messageSize > bytesReceived;

		bool isMessageValid = isMessageClassSame && (!doesSizeOverflow);

		if (isMessageValid == false)
		{
			return false;
		}
		bool doesCRCMatch = (header.messageCRC == calculateCRCwithoutHeader());
		isMessageValid = doesCRCMatch;

		if (isMessageValid == false)
		{
			return false;
		}

		sb.seekPosition(sizeof(SerialHeader));
		obj.serialize(sb);
		sb.alignToNBytes(64);

		isMessageValid = sb.isConsistent()
				&& (sb.getPosition() == header.messageSize);
		return isMessageValid;
	}

	uint8_t * getSerializedMessagePointer();
	uint32_t getMaxBufferSize();
	uint32_t getSerializedMessageSize();
};
#endif /* SERIALIZEOBJECT_H_ */
