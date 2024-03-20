/*
 * cfdstructuresclass.h
 *
 *  Created on: Mar 18, 2021
 *      Author: zumair
 */

#ifndef VO_CFDSTRUCTURESCLASS_H_
#define VO_CFDSTRUCTURESCLASS_H_

#include "../common/common.h"
#include "../model/model.h"

// OUTPUT MESSAGES
static const unsigned int CFD_STATUS_REQ_CODE = 0x02;
static const unsigned int CFD_THREAT_CODE = 0x04;
static const unsigned int CFD_BIT_REQ_CODE = 0x0A;
static const unsigned int CFD_REF_REQ_CODE = 0x0E;
static const unsigned int CFD_PART_NUM_REQ_CODE = 0x16;
static const unsigned int CFD_LOAD_REQ_CODE = 0x06;
static const unsigned int CFD_UPDATE_REQ_CODE = 0x26;
static const unsigned int CFD_PFM_REQ_CODE = 0x36;
static const unsigned int CFD_RESET_REQ_CODE = 0x46;

static const unsigned int CFD_STATUS_OUT_LENGTH = 6;
static const unsigned int CFD_THREAT_OUT_LENGTH = 7;
static const unsigned int CFD_BIT_OUT_LENGTH = 10;
static const unsigned int CFD_REF_OUT_LENGTH = 2;
static const unsigned int CFD_PART_NUM_OUT_LENGTH = 2;
static const unsigned int CFD_LOAD_OUT_LENGTH = 7;
static const unsigned int CFD_UPDATE_OUT_LENGTH = 3;
static const unsigned int CFD_PFM_REQ_OUT_LENGTH = 3;
static const unsigned int CFD_RESET_OUT_LENGTH = 2;

// INPUT MESSAGES
static const unsigned int CFD_STATUS_RESPONSE_CODE = 0x03;
static const unsigned int CFD_BIT_RESPONSE_CODE = 0x0B;
static const unsigned int CFD_REF_RESPONSE_CODE = 0x0F;
static const unsigned int CFD_PART_NUM_RESPONSE_CODE = 0x17;
static const unsigned int CFD_LOAD_ACK_CODE = 0x07;
static const unsigned int CFD_LOAD_NACK_CODE = 0x09;
static const unsigned int CFD_UPDATE_ACK_CODE = 0x27;
static const unsigned int CFD_UPDATE_NACK_CODE = 0x29;
static const unsigned int CFD_PFM_RESPONSE_CODE = 0x37;

static const unsigned int CFD_STATUS_IN_LENGTH = 12;
static const unsigned int CFD_BIT_IN_LENGTH = 12;
static const unsigned int CFD_REF_IN_LENGTH = 18;
static const unsigned int CFD_PART_NUM_IN_LENGTH = 11;
static const unsigned int CFD_LOAD_IN_LENGTH = 3;
static const unsigned int CFD_UPDATE_IN_LENGTH = 3;
static const unsigned int CFD_PFM_RESPONSE_IN_LENGTH = 7;

enum class FlagValidEnum : uint8_t
{
	invalid, valid
};

enum class AngleCfdEnum : uint8_t
{
	Aoa_0_90, Aoa_90_180, Aoa_180_270, Aoa_270_360
};

enum class StatusCfdEnum : uint8_t
{
	None, New, Active, Delete
};

enum class StatusEnum : uint8_t
{
	Flase, True
};

enum class LoadTypeEnum : uint8_t
{
	Chaff, Flare
};

struct cfdStatusRequest
{
	uint8_t modeValidity;
	uint8_t manualProgValidity;
	uint8_t presetChaffValidity;
	uint8_t presetFlareValidity;
	uint8_t mode;
	uint8_t manualProgram;
	uint8_t presetChaff;
	uint8_t presetFlare;
};

struct cfdStatusResponse
{
	uint8_t dispensing;
	uint8_t chaffType;
	uint8_t flareType;
	uint8_t mode;
	uint8_t programType;
	uint8_t programNum;
	uint8_t currentProgram;
	uint8_t threatId;
	uint8_t presetChaff;
	uint8_t presetFlare;
	uint8_t rightTop;
	uint8_t rightTopInventory;
	uint8_t rightBottom;
	uint8_t rightBottomInventory;
	uint8_t leftTop;
	uint8_t leftTopInventory;
	uint8_t leftBottom;
	uint8_t leftBottomInventory;
};

struct cfdThreatData
{
	uint8_t threatId;
	uint8_t cmId;
	uint8_t altitudeValidity;
	uint8_t velocityValidity;
	uint8_t aoa;
	uint8_t status;
	int8_t altitude;
	uint8_t velocity;
};

struct cfdBitRequest
{
	uint8_t rtc1;
	uint8_t rtc2;
	uint8_t rtc3;
	uint8_t rtc4;
	uint8_t year1;
	uint8_t year2;
	uint8_t month;
	uint8_t date;
};

struct cfdBitResponse
{
	uint8_t errorCode1;
	uint8_t ec1CfdU1Fail;
	uint8_t ec1CfdU2Fail;
	uint8_t ec1CfdU3Fail;
	uint8_t ec1CfdU4Fail;
	uint8_t errorCode2;
	uint8_t ec2CfdU1Fail;
	uint8_t ec2CfdU2Fail;
	uint8_t ec2CfdU3Fail;
	uint8_t ec2CfdU4Fail;
	uint8_t errorCode3;
	uint8_t ec3CfdU1Fail;
	uint8_t ec3CfdU2Fail;
	uint8_t ec3CfdU3Fail;
	uint8_t ec3CfdU4Fail;
	uint8_t errorCode4;
	uint8_t ec4CfdU1Fail;
	uint8_t ec4CfdU2Fail;
	uint8_t ec4CfdU3Fail;
	uint8_t ec4CfdU4Fail;
	uint8_t errorCode5;
	uint8_t ec5CfdU1Fail;
	uint8_t ec5CfdU2Fail;
	uint8_t ec5CfdU3Fail;
	uint8_t ec5CfdU4Fail;
};

struct cfdPartNumResponse
{
	uint8_t pnRefChar1;
	uint8_t pnRefChar2;
	uint8_t pnRefChar3;
	uint8_t pnRefChar4;
	uint8_t pnRefChar5;
	uint8_t pnRefChar6;
	uint8_t pnRefChar7;
	uint8_t pnRefChar8;
	uint8_t pnRefChar9;
	uint8_t pnRefChar10;
	uint8_t pnRefChar11;
	uint8_t mc1SwMajor;
	uint8_t mc1SwMinor;
	uint8_t mc2SwMajor;
	uint8_t mc2SwMinor;
	uint8_t fpgaSwMajor;
	uint8_t fpgaSwMinor;
};

struct cfdPfmRefResponse
{
	uint8_t pfmIdMajor;
	uint8_t pfmIdMinor;
	uint8_t pfmYearThousand;
	uint8_t pfmYearHundred;
	uint8_t pfmYearDecade;
	uint8_t pfmYearUnit;
	uint8_t pfmMonthDecade;
	uint8_t pfmMonthUnit;
	uint8_t pfmDayDecade;
	uint8_t pfmDayUnit;
	uint8_t pfmHourDecade;
	uint8_t pfmHourUnit;
	uint8_t pfmMinDecade;
	uint8_t pfmMinUnit;
	uint8_t pfmSecDecade;
	uint8_t pfmSecUnit;
};

struct cfdPfmLoad
{
	uint8_t programId;
	uint8_t numChaffGroup;
	uint8_t intervalChaffGroup;
	uint8_t numChaffPerGroup;
	uint8_t chaffSides;
	uint8_t intervalChaff;
	uint8_t numFlare;
	uint8_t intervalFlare;
	uint8_t flareSides;
};

struct cfdPfmUpdate
{
	uint8_t type;
	uint8_t programId;
};

struct cfdPfmResponse
{
	uint8_t programId;
	uint8_t Nack;
};

struct cfdPfmRequest
{
	uint8_t programId;
};

struct cfdPfmReqResponse
{
	uint8_t type;
	uint8_t programId;
	uint8_t numChaffGroup;
	uint8_t intervalChaffGroup;
	uint8_t chaffPerGroup;
	uint8_t sglDblChaff;
	uint8_t intervalChaff;
	uint8_t numFlare;
	uint8_t intervalFlare;
	uint8_t sglDblFlare;
};

#ifdef VO_CFDSTRUCTURESCLASS_CPP_
cfdStatusResponse statusResponse;
cfdBitResponse bitResponse;
cfdPfmRefResponse referenceResponse;
cfdPartNumResponse partNumResponse;
cfdPfmResponse loadResponse;
cfdPfmResponse updateResponse;
cfdPfmReqResponse pfmReqResponse;
#else
extern cfdStatusResponse statusResponse;
extern cfdBitResponse bitResponse;
extern cfdPfmRefResponse referenceResponse;
extern cfdPartNumResponse partNumResponse;
extern cfdPfmResponse loadResponse;
extern cfdPfmResponse updateResponse;
extern cfdPfmReqResponse pfmReqResponse;
#endif

int handleStatusRequest(cfdStatusRequest cfdStatus);
void handleThreatData(cfdThreatData cfdThreat);
int handleBitRequest(cfdBitRequest cfdBit);
int handleReferenceRequest();
int handlePartNumRequest();
int handlePfmLoad(cfdPfmLoad cfdLoad);
int handlePfmUpdate(cfdPfmUpdate cfdUpdate);
int handlePfmRequest(cfdPfmRequest cfdRequest);
int handleReset();

#endif /* VO_CFDSTRUCTURESCLASS_H_ */
