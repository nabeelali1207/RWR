/*
 * cfdstructuresclass.cpp
 *
 *  Created on: Mar 18, 2021
 *      Author: zumair
 */
#define VO_CFDSTRUCTURESCLASS_CPP_

#include "../vo/cfdstructuresclass.h"

int handleStatusRequest(cfdStatusRequest cfdStatus)
{
	uint16_t crc = 0, rxCrc = 0;
	uint8_t *buff = (UWord8*) &Model::cfdUartLink.cfdDataBuffer[0];

	Model::cfdUartLink.cfdDataBuffer[0] = CFD_STATUS_REQ_CODE;
	Model::cfdUartLink.cfdDataBuffer[1] = CFD_STATUS_OUT_LENGTH;

	Model::cfdUartLink.cfdDataBuffer[2] = (cfdStatus.modeValidity & 0x01) << 7;
	Model::cfdUartLink.cfdDataBuffer[2] |= (cfdStatus.manualProgValidity & 0x01)
			<< 6;
	Model::cfdUartLink.cfdDataBuffer[2] |=
			(cfdStatus.presetChaffValidity & 0x01) << 5;
	Model::cfdUartLink.cfdDataBuffer[2] |=
			(cfdStatus.presetFlareValidity & 0x01) << 4;
	Model::cfdUartLink.cfdDataBuffer[2] |= (cfdStatus.mode & 0x0F);

	Model::cfdUartLink.cfdDataBuffer[3] = cfdStatus.manualProgram;

	Model::cfdUartLink.cfdDataBuffer[4] = cfdStatus.presetChaff;

	Model::cfdUartLink.cfdDataBuffer[5] = cfdStatus.presetFlare;

	Model::cfdUartLink.preCrc();
	Model::cfdUartLink.updateCrc(buff, CFD_STATUS_OUT_LENGTH);
	crc = Model::cfdUartLink.getCrc();

	Model::cfdUartLink.cfdDataBuffer[CFD_STATUS_OUT_LENGTH] = crc >> 8;
	Model::cfdUartLink.cfdDataBuffer[CFD_STATUS_OUT_LENGTH + 1] = crc & 0x00FF;

	Model::cfdUartLink.sendCfdData(CFD_STATUS_OUT_LENGTH + 2);

	usleep(100);

	Model::cfdUartLink.readCfdData();

	if (Model::cfdUartLink.cfdDataRxBuffer[0] == CFD_STATUS_RESPONSE_CODE)
	{
		statusResponse.dispensing = (Model::cfdUartLink.cfdDataRxBuffer[2] >> 7)
				& 0x01;
		statusResponse.chaffType = (Model::cfdUartLink.cfdDataRxBuffer[2] >> 6)
				& 0x01;
		statusResponse.flareType = (Model::cfdUartLink.cfdDataRxBuffer[2] >> 5)
				& 0x01;
		statusResponse.mode = (Model::cfdUartLink.cfdDataRxBuffer[2] >> 1) & 0x1F;

		statusResponse.programType = (Model::cfdUartLink.cfdDataRxBuffer[3] >> 6) & 0x03;
		statusResponse.programNum = (Model::cfdUartLink.cfdDataRxBuffer[3]) & 0x0F;

		statusResponse.currentProgram = (Model::cfdUartLink.cfdDataRxBuffer[4]);

		statusResponse.threatId = (Model::cfdUartLink.cfdDataRxBuffer[5]);

		statusResponse.presetChaff = (Model::cfdUartLink.cfdDataRxBuffer[6]);

		statusResponse.presetFlare = (Model::cfdUartLink.cfdDataRxBuffer[7]);

		statusResponse.rightTop = (Model::cfdUartLink.cfdDataRxBuffer[8] >> 7)
				& 0x01;
		statusResponse.rightTopInventory = (Model::cfdUartLink.cfdDataRxBuffer[8])
				& 0x7F;

		statusResponse.rightBottom = (Model::cfdUartLink.cfdDataRxBuffer[9] >> 7)
				& 0x01;
		statusResponse.rightBottomInventory =
				(Model::cfdUartLink.cfdDataRxBuffer[9]) & 0x7F;

		statusResponse.leftTop = (Model::cfdUartLink.cfdDataRxBuffer[10] >> 7)
				& 0x01;
		statusResponse.leftTopInventory = (Model::cfdUartLink.cfdDataRxBuffer[10])
				& 0x7F;

		statusResponse.leftBottom = (Model::cfdUartLink.cfdDataRxBuffer[11] >> 7)
				& 0x01;
		statusResponse.leftBottomInventory =
				(Model::cfdUartLink.cfdDataRxBuffer[11]) & 0x7F;

		buff = (UWord8*) &Model::cfdUartLink.cfdDataRxBuffer[0];
		Model::cfdUartLink.preCrc();
		Model::cfdUartLink.updateCrc(buff, CFD_STATUS_IN_LENGTH);
		rxCrc = Model::cfdUartLink.getCrc();

		crc = Model::cfdUartLink.cfdDataRxBuffer[CFD_STATUS_IN_LENGTH] << 8
				| Model::cfdUartLink.cfdDataRxBuffer[CFD_STATUS_IN_LENGTH + 1];

		if (rxCrc != crc)
		{
			return 3;
		}
	}
	else
	{
		return 2;
	}

	return 1;
}

void handleThreatData(cfdThreatData cfdThreat)
{
	uint16_t crc = 0;
	uint8_t *buff = (UWord8*) &Model::cfdUartLink.cfdDataBuffer[0];

	Model::cfdUartLink.cfdDataBuffer[0] = CFD_THREAT_CODE;
	Model::cfdUartLink.cfdDataBuffer[1] = CFD_THREAT_OUT_LENGTH;

	Model::cfdUartLink.cfdDataBuffer[2] = (cfdThreat.threatId & 0xFF);

	Model::cfdUartLink.cfdDataBuffer[3] = (cfdThreat.cmId & 0xFF);

	Model::cfdUartLink.cfdDataBuffer[4] = (cfdThreat.altitudeValidity & 0x01)
			<< 7;
	Model::cfdUartLink.cfdDataBuffer[4] |= (cfdThreat.velocityValidity & 0x01)
			<< 6;
	Model::cfdUartLink.cfdDataBuffer[4] |= (cfdThreat.aoa & 0x03) << 2;
	Model::cfdUartLink.cfdDataBuffer[4] |= (cfdThreat.status & 0x03);

	Model::cfdUartLink.cfdDataBuffer[5] = (cfdThreat.altitude & 0xFF);

	Model::cfdUartLink.cfdDataBuffer[6] = (cfdThreat.velocity & 0xFF);

	Model::cfdUartLink.preCrc();
	Model::cfdUartLink.updateCrc(buff, CFD_THREAT_OUT_LENGTH);
	crc = Model::cfdUartLink.getCrc();

	Model::cfdUartLink.cfdDataBuffer[CFD_THREAT_OUT_LENGTH] = crc >> 8;
	Model::cfdUartLink.cfdDataBuffer[CFD_THREAT_OUT_LENGTH + 1] = crc & 0x00FF;

	Model::cfdUartLink.sendCfdData(CFD_THREAT_OUT_LENGTH + 2);
}

int handleBitRequest(cfdBitRequest cfdBit)
{
	uint16_t crc = 0, rxCrc = 0;
	uint8_t *buff = (UWord8*) &Model::cfdUartLink.cfdDataBuffer[0];

	Model::cfdUartLink.cfdDataBuffer[0] = CFD_BIT_REQ_CODE;
	Model::cfdUartLink.cfdDataBuffer[1] = CFD_BIT_OUT_LENGTH;

	Model::cfdUartLink.cfdDataBuffer[2] = cfdBit.rtc1;
	Model::cfdUartLink.cfdDataBuffer[3] = cfdBit.rtc2;
	Model::cfdUartLink.cfdDataBuffer[4] = cfdBit.rtc3;
	Model::cfdUartLink.cfdDataBuffer[5] = cfdBit.rtc4;
	Model::cfdUartLink.cfdDataBuffer[6] = cfdBit.year1;
	Model::cfdUartLink.cfdDataBuffer[7] = cfdBit.year2;
	Model::cfdUartLink.cfdDataBuffer[8] = cfdBit.month;
	Model::cfdUartLink.cfdDataBuffer[9] = cfdBit.date;

	Model::cfdUartLink.preCrc();
	Model::cfdUartLink.updateCrc(buff, CFD_BIT_OUT_LENGTH);
	crc = Model::cfdUartLink.getCrc();

	Model::cfdUartLink.cfdDataBuffer[CFD_BIT_OUT_LENGTH] = crc >> 8;
	Model::cfdUartLink.cfdDataBuffer[CFD_BIT_OUT_LENGTH + 1] = crc & 0x00FF;

	Model::cfdUartLink.sendCfdData(CFD_BIT_OUT_LENGTH + 2);

	usleep(100);

	Model::cfdUartLink.readCfdData();

	if (Model::cfdUartLink.cfdDataRxBuffer[0] == CFD_BIT_RESPONSE_CODE)
	{
		bitResponse.errorCode1 = Model::cfdUartLink.cfdDataRxBuffer[2];

		bitResponse.ec1CfdU1Fail = (Model::cfdUartLink.cfdDataRxBuffer[3] >> 3)
				& 0x01;
		bitResponse.ec1CfdU2Fail = (Model::cfdUartLink.cfdDataRxBuffer[3] >> 2)
				& 0x01;
		bitResponse.ec1CfdU3Fail = (Model::cfdUartLink.cfdDataRxBuffer[3] >> 1)
				& 0x01;
		bitResponse.ec1CfdU4Fail = (Model::cfdUartLink.cfdDataRxBuffer[3]) & 0x01;

		bitResponse.errorCode2 = Model::cfdUartLink.cfdDataRxBuffer[4];

		bitResponse.ec2CfdU1Fail = (Model::cfdUartLink.cfdDataRxBuffer[5] >> 3)
				& 0x01;
		bitResponse.ec2CfdU2Fail = (Model::cfdUartLink.cfdDataRxBuffer[5] >> 2)
				& 0x01;
		bitResponse.ec2CfdU3Fail = (Model::cfdUartLink.cfdDataRxBuffer[5] >> 1)
				& 0x01;
		bitResponse.ec2CfdU4Fail = (Model::cfdUartLink.cfdDataRxBuffer[5]) & 0x01;

		bitResponse.errorCode3 = Model::cfdUartLink.cfdDataRxBuffer[6];

		bitResponse.ec3CfdU1Fail = (Model::cfdUartLink.cfdDataRxBuffer[7] >> 3)
				& 0x01;
		bitResponse.ec3CfdU2Fail = (Model::cfdUartLink.cfdDataRxBuffer[7] >> 2)
				& 0x01;
		bitResponse.ec3CfdU3Fail = (Model::cfdUartLink.cfdDataRxBuffer[7] >> 1)
				& 0x01;
		bitResponse.ec3CfdU4Fail = (Model::cfdUartLink.cfdDataRxBuffer[7]) & 0x01;

		bitResponse.errorCode4 = Model::cfdUartLink.cfdDataRxBuffer[8];

		bitResponse.ec4CfdU1Fail = (Model::cfdUartLink.cfdDataRxBuffer[9] >> 3)
				& 0x01;
		bitResponse.ec4CfdU2Fail = (Model::cfdUartLink.cfdDataRxBuffer[9] >> 2)
				& 0x01;
		bitResponse.ec4CfdU3Fail = (Model::cfdUartLink.cfdDataRxBuffer[9] >> 1)
				& 0x01;
		bitResponse.ec4CfdU4Fail = (Model::cfdUartLink.cfdDataRxBuffer[9]) & 0x01;

		bitResponse.errorCode5 = Model::cfdUartLink.cfdDataRxBuffer[10];

		bitResponse.ec5CfdU1Fail = (Model::cfdUartLink.cfdDataRxBuffer[11] >> 3)
				& 0x01;
		bitResponse.ec5CfdU2Fail = (Model::cfdUartLink.cfdDataRxBuffer[11] >> 2)
				& 0x01;
		bitResponse.ec5CfdU3Fail = (Model::cfdUartLink.cfdDataRxBuffer[11] >> 1)
				& 0x01;
		bitResponse.ec5CfdU4Fail = (Model::cfdUartLink.cfdDataRxBuffer[11])
				& 0x01;

		buff = (UWord8*) &Model::cfdUartLink.cfdDataRxBuffer[0];
		Model::cfdUartLink.preCrc();
		Model::cfdUartLink.updateCrc(buff, CFD_BIT_IN_LENGTH);
		rxCrc = Model::cfdUartLink.getCrc();

		crc = Model::cfdUartLink.cfdDataRxBuffer[CFD_BIT_IN_LENGTH] << 8
				| Model::cfdUartLink.cfdDataRxBuffer[CFD_BIT_IN_LENGTH + 1];

		if (rxCrc != crc)
		{
			return 3;
		}
	}
	else
	{
		return 2;
	}

	return 1;
}

int handleReferenceRequest()
{
	uint16_t crc = 0, rxCrc = 0;
	uint8_t *buff = (UWord8*) &Model::cfdUartLink.cfdDataBuffer[0];

	Model::cfdUartLink.cfdDataBuffer[0] = CFD_REF_REQ_CODE;
	Model::cfdUartLink.cfdDataBuffer[1] = CFD_REF_OUT_LENGTH;

	Model::cfdUartLink.preCrc();
	Model::cfdUartLink.updateCrc(buff, CFD_REF_OUT_LENGTH);
	crc = Model::cfdUartLink.getCrc();

	Model::cfdUartLink.cfdDataBuffer[CFD_REF_OUT_LENGTH] = crc >> 8;
	Model::cfdUartLink.cfdDataBuffer[CFD_REF_OUT_LENGTH + 1] = crc & 0x00FF;

	Model::cfdUartLink.sendCfdData(CFD_REF_OUT_LENGTH + 2);

	usleep(100);

	Model::cfdUartLink.readCfdData();

	if (Model::cfdUartLink.cfdDataRxBuffer[0] == CFD_REF_RESPONSE_CODE)
	{
		referenceResponse.pfmIdMajor = Model::cfdUartLink.cfdDataRxBuffer[2];

		referenceResponse.pfmIdMinor = Model::cfdUartLink.cfdDataRxBuffer[3];

		referenceResponse.pfmYearThousand = (Model::cfdUartLink.cfdDataRxBuffer[4]
				>> 4) & 0x0F;
		referenceResponse.pfmYearHundred = (Model::cfdUartLink.cfdDataRxBuffer[4])
				& 0x0F;

		referenceResponse.pfmYearDecade = (Model::cfdUartLink.cfdDataRxBuffer[5]
				>> 4) & 0x0F;
		referenceResponse.pfmYearUnit = (Model::cfdUartLink.cfdDataRxBuffer[5])
				& 0x0F;

		referenceResponse.pfmMonthDecade = (Model::cfdUartLink.cfdDataRxBuffer[6]
				>> 4) & 0x0F;
		referenceResponse.pfmMonthUnit = (Model::cfdUartLink.cfdDataRxBuffer[6])
				& 0x0F;

		referenceResponse.pfmDayDecade = (Model::cfdUartLink.cfdDataRxBuffer[7]
				>> 4) & 0x0F;
		referenceResponse.pfmDayUnit = (Model::cfdUartLink.cfdDataRxBuffer[7])
				& 0x0F;

		referenceResponse.pfmHourDecade = (Model::cfdUartLink.cfdDataRxBuffer[8]
				>> 4) & 0x0F;
		referenceResponse.pfmHourUnit = (Model::cfdUartLink.cfdDataRxBuffer[8])
				& 0x0F;

		referenceResponse.pfmMinDecade = (Model::cfdUartLink.cfdDataRxBuffer[9]
				>> 4) & 0x0F;
		referenceResponse.pfmMinUnit = (Model::cfdUartLink.cfdDataRxBuffer[9])
				& 0x0F;

		referenceResponse.pfmSecDecade = (Model::cfdUartLink.cfdDataRxBuffer[10]
				>> 4) & 0x0F;
		referenceResponse.pfmSecUnit = (Model::cfdUartLink.cfdDataRxBuffer[10])
				& 0x0F;

		buff = (UWord8*) &Model::cfdUartLink.cfdDataRxBuffer[0];
		Model::cfdUartLink.preCrc();
		Model::cfdUartLink.updateCrc(buff, CFD_REF_IN_LENGTH);
		rxCrc = Model::cfdUartLink.getCrc();

		crc = Model::cfdUartLink.cfdDataRxBuffer[CFD_REF_IN_LENGTH] << 8
				| Model::cfdUartLink.cfdDataRxBuffer[CFD_REF_IN_LENGTH + 1];

		if (rxCrc != crc)
		{
			return 3;
		}
	}
	else
	{
		return 2;
	}

	return 1;
}

int handlePartNumRequest()
{
	uint16_t crc = 0, rxCrc = 0;
	uint8_t *buff = (UWord8*) &Model::cfdUartLink.cfdDataBuffer[0];

	Model::cfdUartLink.cfdDataBuffer[0] = CFD_PART_NUM_REQ_CODE;
	Model::cfdUartLink.cfdDataBuffer[1] = CFD_PART_NUM_OUT_LENGTH;

	Model::cfdUartLink.preCrc();
	Model::cfdUartLink.updateCrc(buff, CFD_PART_NUM_OUT_LENGTH);
	crc = Model::cfdUartLink.getCrc();

	Model::cfdUartLink.cfdDataBuffer[CFD_PART_NUM_OUT_LENGTH] = crc >> 8;
	Model::cfdUartLink.cfdDataBuffer[CFD_PART_NUM_OUT_LENGTH + 1] = crc
			& 0x00FF;

	Model::cfdUartLink.sendCfdData(CFD_PART_NUM_OUT_LENGTH + 2);

	usleep(100);

	Model::cfdUartLink.readCfdData();

	if (Model::cfdUartLink.cfdDataRxBuffer[0] == CFD_PART_NUM_RESPONSE_CODE)
	{
		partNumResponse.pnRefChar1 = Model::cfdUartLink.cfdDataRxBuffer[2];

		partNumResponse.pnRefChar2 = Model::cfdUartLink.cfdDataRxBuffer[3];

		partNumResponse.pnRefChar3 = Model::cfdUartLink.cfdDataRxBuffer[4];

		partNumResponse.pnRefChar4 = Model::cfdUartLink.cfdDataRxBuffer[5];

		partNumResponse.pnRefChar5 = Model::cfdUartLink.cfdDataRxBuffer[6];

		partNumResponse.pnRefChar6 = Model::cfdUartLink.cfdDataRxBuffer[7];

		partNumResponse.pnRefChar7 = Model::cfdUartLink.cfdDataRxBuffer[8];

		partNumResponse.pnRefChar8 = Model::cfdUartLink.cfdDataRxBuffer[9];

		partNumResponse.pnRefChar9 = Model::cfdUartLink.cfdDataRxBuffer[10];

		partNumResponse.pnRefChar10 = Model::cfdUartLink.cfdDataRxBuffer[11];

		partNumResponse.pnRefChar11 = Model::cfdUartLink.cfdDataRxBuffer[12];

		partNumResponse.mc1SwMajor = (Model::cfdUartLink.cfdDataRxBuffer[13] >> 4)
				& 0x0F;
		partNumResponse.mc1SwMinor = (Model::cfdUartLink.cfdDataRxBuffer[13])
				& 0x0F;

		partNumResponse.mc2SwMajor = (Model::cfdUartLink.cfdDataRxBuffer[14] >> 4)
				& 0x0F;
		partNumResponse.mc2SwMinor = (Model::cfdUartLink.cfdDataRxBuffer[14])
				& 0x0F;

		partNumResponse.fpgaSwMajor =
				(Model::cfdUartLink.cfdDataRxBuffer[15] >> 4) & 0x0F;
		partNumResponse.fpgaSwMinor = (Model::cfdUartLink.cfdDataRxBuffer[15])
				& 0x0F;

		buff = (UWord8*) &Model::cfdUartLink.cfdDataRxBuffer[0];
		Model::cfdUartLink.preCrc();
		Model::cfdUartLink.updateCrc(buff, CFD_PART_NUM_IN_LENGTH);
		rxCrc = Model::cfdUartLink.getCrc();

		crc = Model::cfdUartLink.cfdDataRxBuffer[CFD_PART_NUM_IN_LENGTH] << 8
				| Model::cfdUartLink.cfdDataRxBuffer[CFD_PART_NUM_IN_LENGTH + 1];

		if (rxCrc != crc)
		{
			return 3;
		}
	}
	else
	{
		return 2;
	}

	return 1;
}

int handlePfmLoad(cfdPfmLoad cfdLoad)
{
	uint16_t crc = 0, rxCrc = 0;
	uint8_t *buff = (UWord8*) &Model::cfdUartLink.cfdDataBuffer[0];

	Model::cfdUartLink.cfdDataBuffer[0] = CFD_LOAD_REQ_CODE;
	Model::cfdUartLink.cfdDataBuffer[1] = CFD_LOAD_OUT_LENGTH;

	Model::cfdUartLink.cfdDataBuffer[2] = cfdLoad.programId;

	Model::cfdUartLink.cfdDataBuffer[3] = (cfdLoad.numChaffGroup & 0x3F) << 2;
	Model::cfdUartLink.cfdDataBuffer[3] |= ((cfdLoad.intervalChaffGroup & 0x03)
			>> 2);

	Model::cfdUartLink.cfdDataBuffer[4] = (cfdLoad.intervalChaffGroup & 0x0F)
			<< 6;
	Model::cfdUartLink.cfdDataBuffer[4] |= (cfdLoad.numChaffPerGroup & 0x1F)
			<< 1;
	Model::cfdUartLink.cfdDataBuffer[4] |= (cfdLoad.chaffSides & 0x01);

	Model::cfdUartLink.cfdDataBuffer[5] = (cfdLoad.intervalChaff & 0x0F) << 4;
	Model::cfdUartLink.cfdDataBuffer[5] |= ((cfdLoad.numFlare &0x0F) >> 3);

	Model::cfdUartLink.cfdDataBuffer[6] = (cfdLoad.numFlare & 0x07) << 5;
	Model::cfdUartLink.cfdDataBuffer[6] |= (cfdLoad.intervalFlare & 0x0F) << 1;
	Model::cfdUartLink.cfdDataBuffer[6] |= (cfdLoad.flareSides & 0x01);

	Model::cfdUartLink.preCrc();
	Model::cfdUartLink.updateCrc(buff, CFD_LOAD_OUT_LENGTH);
	crc = Model::cfdUartLink.getCrc();

	Model::cfdUartLink.cfdDataBuffer[CFD_LOAD_OUT_LENGTH] = crc >> 8;
	Model::cfdUartLink.cfdDataBuffer[CFD_LOAD_OUT_LENGTH + 1] = crc & 0x00FF;

	Model::cfdUartLink.sendCfdData(CFD_LOAD_OUT_LENGTH + 2);

	usleep(100);

	Model::cfdUartLink.readCfdData();

	if ((Model::cfdUartLink.cfdDataRxBuffer[0] == CFD_LOAD_ACK_CODE)
			|| (Model::cfdUartLink.cfdDataRxBuffer[0] == CFD_LOAD_NACK_CODE))
	{
		loadResponse.programId = Model::cfdUartLink.cfdDataRxBuffer[2];
		if ((Model::cfdUartLink.cfdDataRxBuffer[0] == CFD_LOAD_NACK_CODE))
		{
			loadResponse.Nack = 1;
		}
		else
		{
			loadResponse.Nack = 0;
		}

		buff = (UWord8*) &Model::cfdUartLink.cfdDataRxBuffer[0];
		Model::cfdUartLink.preCrc();
		Model::cfdUartLink.updateCrc(buff, CFD_LOAD_IN_LENGTH);
		rxCrc = Model::cfdUartLink.getCrc();

		crc = Model::cfdUartLink.cfdDataRxBuffer[CFD_LOAD_IN_LENGTH] << 8
				| Model::cfdUartLink.cfdDataRxBuffer[CFD_LOAD_IN_LENGTH + 1];

		if (rxCrc != crc)
		{
			return 3;
		}
	}
	else
	{
		return 2;
	}

	return 1;
}

int handlePfmUpdate(cfdPfmUpdate cfdUpdate)
{
	uint16_t crc = 0, rxCrc = 0;
	uint8_t *buff = (UWord8*) &Model::cfdUartLink.cfdDataBuffer[0];

	Model::cfdUartLink.cfdDataBuffer[0] = CFD_UPDATE_REQ_CODE;
	Model::cfdUartLink.cfdDataBuffer[1] = CFD_UPDATE_OUT_LENGTH;

	Model::cfdUartLink.cfdDataBuffer[2] = (cfdUpdate.type & 0x03) << 5;
	Model::cfdUartLink.cfdDataBuffer[2] |= (cfdUpdate.programId & 0x1F);

	Model::cfdUartLink.preCrc();
	Model::cfdUartLink.updateCrc(buff, CFD_UPDATE_OUT_LENGTH);
	crc = Model::cfdUartLink.getCrc();

	Model::cfdUartLink.cfdDataBuffer[CFD_UPDATE_OUT_LENGTH] = crc >> 8;
	Model::cfdUartLink.cfdDataBuffer[CFD_UPDATE_OUT_LENGTH + 1] = crc & 0x00FF;

	Model::cfdUartLink.sendCfdData(CFD_UPDATE_OUT_LENGTH + 2);

	usleep(100);

	Model::cfdUartLink.readCfdData();

	if ((Model::cfdUartLink.cfdDataRxBuffer[0] == CFD_UPDATE_ACK_CODE)
			|| (Model::cfdUartLink.cfdDataRxBuffer[0] == CFD_UPDATE_NACK_CODE))
	{
		updateResponse.programId = Model::cfdUartLink.cfdDataRxBuffer[2];
		if ((Model::cfdUartLink.cfdDataRxBuffer[0] == CFD_UPDATE_NACK_CODE))
		{
			updateResponse.Nack = 1;
		}
		else
		{
			updateResponse.Nack = 0;
		}

		buff = (UWord8*) &Model::cfdUartLink.cfdDataRxBuffer[0];
		Model::cfdUartLink.preCrc();
		Model::cfdUartLink.updateCrc(buff, CFD_UPDATE_IN_LENGTH);
		rxCrc = Model::cfdUartLink.getCrc();

		crc = Model::cfdUartLink.cfdDataRxBuffer[CFD_UPDATE_IN_LENGTH] << 8
				| Model::cfdUartLink.cfdDataRxBuffer[CFD_UPDATE_IN_LENGTH + 1];

		if (rxCrc != crc)
		{
			return 3;
		}
	}
	else
	{
		return 2;
	}

	return 1;
}

int handlePfmRequest(cfdPfmRequest cfdRequest)
{
	uint16_t crc = 0, rxCrc = 0;
	uint8_t *buff = (UWord8*) &Model::cfdUartLink.cfdDataBuffer[0];

	Model::cfdUartLink.cfdDataBuffer[0] = CFD_PFM_REQ_CODE;
	Model::cfdUartLink.cfdDataBuffer[1] = CFD_PFM_REQ_OUT_LENGTH;

	Model::cfdUartLink.cfdDataBuffer[2] = (cfdRequest.programId & 0x1F);

	Model::cfdUartLink.preCrc();
	Model::cfdUartLink.updateCrc(buff, CFD_PFM_REQ_OUT_LENGTH);
	crc = Model::cfdUartLink.getCrc();

	Model::cfdUartLink.cfdDataBuffer[CFD_PFM_REQ_OUT_LENGTH] = crc >> 8;
	Model::cfdUartLink.cfdDataBuffer[CFD_PFM_REQ_OUT_LENGTH + 1] = crc & 0x00FF;

	Model::cfdUartLink.sendCfdData(CFD_PFM_REQ_OUT_LENGTH + 2);

	usleep(100);

	Model::cfdUartLink.readCfdData();

	if (Model::cfdUartLink.cfdDataRxBuffer[0] == CFD_PFM_RESPONSE_CODE)
	{
		pfmReqResponse.type = (Model::cfdUartLink.cfdDataRxBuffer[2] >> 5) & 0x03;

		pfmReqResponse.programId = Model::cfdUartLink.cfdDataRxBuffer[2] & 0x1F;

		pfmReqResponse.numChaffGroup = Model::cfdUartLink.cfdDataRxBuffer[3] >> 2;

		pfmReqResponse.intervalChaffGroup = (Model::cfdUartLink.cfdDataRxBuffer[3]
				& 0x03) << 2;
		pfmReqResponse.intervalChaffGroup |=
				(Model::cfdUartLink.cfdDataRxBuffer[4] >> 6);

		pfmReqResponse.chaffPerGroup =
				(Model::cfdUartLink.cfdDataRxBuffer[4] >> 1) & 0x1F;

		pfmReqResponse.sglDblChaff = Model::cfdUartLink.cfdDataRxBuffer[4] & 0x01;

		pfmReqResponse.intervalChaff = Model::cfdUartLink.cfdDataRxBuffer[5] >> 4;

		pfmReqResponse.numFlare = (Model::cfdUartLink.cfdDataRxBuffer[5] & 0x0F)
				<< 3;
		pfmReqResponse.numFlare |= (Model::cfdUartLink.cfdDataRxBuffer[6] >> 5);

		pfmReqResponse.intervalFlare =
				(Model::cfdUartLink.cfdDataRxBuffer[6] >> 1) & 0x0F;

		pfmReqResponse.sglDblFlare = Model::cfdUartLink.cfdDataRxBuffer[6] & 0x01;

		buff = (UWord8*) &Model::cfdUartLink.cfdDataRxBuffer[0];
		Model::cfdUartLink.preCrc();
		Model::cfdUartLink.updateCrc(buff, CFD_PFM_RESPONSE_IN_LENGTH);
		rxCrc = Model::cfdUartLink.getCrc();

		crc = Model::cfdUartLink.cfdDataRxBuffer[CFD_PFM_RESPONSE_IN_LENGTH] << 8
				| Model::cfdUartLink.cfdDataRxBuffer[CFD_PFM_RESPONSE_IN_LENGTH
						+ 1];

		if (rxCrc != crc)
		{
			return 3;
		}
	}
	else
	{
		return 2;
	}

	return 1;
}

int handleReset()
{
	uint16_t crc = 0;
	uint8_t *buff = (UWord8*) &Model::cfdUartLink.cfdDataBuffer[0];

	Model::cfdUartLink.cfdDataBuffer[0] = CFD_RESET_REQ_CODE;
	Model::cfdUartLink.cfdDataBuffer[1] = CFD_RESET_OUT_LENGTH;

	Model::cfdUartLink.preCrc();
	Model::cfdUartLink.updateCrc(buff, CFD_RESET_OUT_LENGTH);
	crc = Model::cfdUartLink.getCrc();

	Model::cfdUartLink.cfdDataBuffer[CFD_RESET_OUT_LENGTH] = crc >> 8;
	Model::cfdUartLink.cfdDataBuffer[CFD_RESET_OUT_LENGTH + 1] = crc & 0x00FF;

	Model::cfdUartLink.sendCfdData(CFD_RESET_OUT_LENGTH + 2);

	return 1;
}
