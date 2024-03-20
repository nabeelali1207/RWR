/*
 * selftestmfldelegate.cpp
 *
 *  Created on: Jun 25, 2021
 *      Author: zumair
 */

#define SELFTESTMFLDELEGATE_CPP_

#include "selftestmflhighdelegate.h"

//This function checks whether /tmp/emmc/mfl/ directory exists, if it
//doesn't, it makes the directory and makes all the essential files of mfl
void SelfTestMflHigh::initMflDirectory()
{
	DIR *directory;
	if (ConstHigh::simOnActualHwFlag == 1)
	{
		directory = opendir("/tmp/emmc/mfl/");
		if (!directory)
		{
			strcpy(command, "mkdir /tmp/emmc/mfl/");
			system(command);

			strcpy(command, "touch /tmp/emmc/mfl/current.txt");
			system(command);

			initMflFiles();
		}
	}
}

//This function initializes all the mfl files used to record faults in
///mfl
void SelfTestMflHigh::initMflFiles()
{
	Word8 filename[30];
	UWord8 i = 0;
	initMflStructure();
	FILE *pf;
	pf = fopen("/tmp/emmc/mfl/current.txt", "wb+");
	if (pf != NULL)
	{
		fseek(pf, 0, SEEK_SET);
		for (UWord32 i = 0; i < TOTAL_ERROR_COUNT; i++)
		{
			fprintf(pf, "0\n");
		}
		fclose(pf);
	}
	for (i = 1; i <= CPF_ERROR_COUNT; i++)
	{
		sprintf(filename, "/tmp/emmc/mfl/1-1-1-%d.mfl", i);

		mflHandler.writeMflFile(filename, sizeof(MflStructure), (UWord8*) &mfl);
	}

	for (i = 1; i <= PPF_ERROR_COUNT; i++)
	{
		sprintf(filename, "/tmp/emmc/mfl/1-1-2-%d.mfl", i);
		mflHandler.writeMflFile(filename, sizeof(MflStructure), (UWord8*) &mfl);
	}

	for (i = 1; i <= CCP_ERROR_COUNT; i++)
	{
		sprintf(filename, "/tmp/emmc/mfl/1-1-3-%d.mfl", i);
		mflHandler.writeMflFile(filename, sizeof(MflStructure), (UWord8*) &mfl);
	}

	for (i = 1; i <= TP_ERROR_COUNT; i++)
	{
		sprintf(filename, "/tmp/emmc/mfl/1-2-0-%d.mfl", i);
		mflHandler.writeMflFile(filename, sizeof(MflStructure), (UWord8*) &mfl);
	}

	for (i = 1; i <= DIFM_ERROR_COUNT; i++)
	{
		sprintf(filename, "/tmp/emmc/mfl/1-3-1-%d.mfl", i);
		mflHandler.writeMflFile(filename, sizeof(MflStructure), (UWord8*) &mfl);
	}

	for (i = 1; i <= CWP_ERROR_COUNT; i++)
	{
		sprintf(filename, "/tmp/emmc/mfl/1-3-2-%d.mfl", i);
		mflHandler.writeMflFile(filename, sizeof(MflStructure), (UWord8*) &mfl);
	}

	for (i = 1; i <= DCM_ERROR_COUNT; i++)
	{
		sprintf(filename, "/tmp/emmc/mfl/1-4-0-%d.mfl", i);
		mflHandler.writeMflFile(filename, sizeof(MflStructure), (UWord8*) &mfl);
	}

	for (i = 1; i <= DRU_ERROR_COUNT; i++)
	{
		sprintf(filename, "/tmp/emmc/mfl/2-0-0-%d.mfl", i);
		mflHandler.writeMflFile(filename, sizeof(MflStructure), (UWord8*) &mfl);
	}
}

void SelfTestMflHigh::initMflStructure()
{
	mfl.exists = 0;
	mfl.sys1 = 0;
	mfl.sys2 = 0;
	mfl.subsys = ' ';
	mfl.lru[0] = ' ';
	mfl.lru[1] = ' ';
	mfl.lru[2] = ' ';
	mfl.fctn1 = 0;
	mfl.fctn2 = 0;

	mfl.amount = 0;
	mfl.occurTime = 0;
	mfl.durationTime = 0;
	for (auto in = 0; in < 40; in++)
	{
		mfl.description[in] = ' ';
	}
}

//This function updates the mfl files corresponding to fault
void SelfTestMflHigh::updateMflEntry(SingleFaultData fault, uint32_t rtcTime,
		FlightRecordHeader &flightRecordMaintainanceHeader)
{
	Word8 filename[30];
	UWord16 i = 0;
	UWord8 lru = 0, sru = 0, subModule = 0, module = 0;

	if (fault.module == 0 || fault.module == 1 || fault.module == 3)
	{
		lru = 1;
		sru = 1;
		if (fault.module == 0)
		{
			subModule = 1;
			module = 0;
		}
		else if (fault.module == 1)
		{
			subModule = 2;
			module = 1;
		}
		else if (fault.module == 3)
		{
			subModule = 3;
			module = 2;
		}
	}
	else if (fault.module == 4 || fault.module == 5)
	{
		lru = 1;
		sru = 3;
		if (fault.module == 4)
		{
			subModule = 1;
			module = 4;
		}
		else if (fault.module == 5)
		{
			subModule = 2;
			module = 5;
		}
	}
	else if (fault.module == 2)
	{
		lru = 1;
		sru = 2;
		subModule = 0;
		module = 3;
	}
	else if (fault.module == 6)
	{
		lru = 1;
		sru = 4;
		subModule = 0;
		module = 6;
	}
	else if (fault.module == 7)
	{
		lru = 2;
		sru = 0;
		subModule = 0;
		module = 7;
		fault.status = 0;
	}

	sprintf(filename, "/tmp/emmc/mfl/%d-%d-%d-%d.mfl", lru, sru, subModule,
			fault.faultId);

	// read mfl values
	mflHandler.readMflFile(filename, sizeof(MflStructure), (UWord8*) &mfl);

	if ((mfl.exists ^ fault.status) == 1)
	{
		if (fault.status == 0)
		{
			mfl.durationTime = rtcTime - mfl.occurTime;
		}
		else
		{
			mfl.amount += 1;
			if (mfl.occurTime == 0)
			{
				mfl.occurTime = rtcTime;
				mfl.durationTime = 100000;
			}
			else
			{
				mfl.durationTime = rtcTime - mfl.occurTime;
			}

		}
	}
	if (mfl.exists != fault.status)
	{
		mflData[5 * flightRecordMaintainanceHeader.structureCount] = lru;
		mflData[5 * flightRecordMaintainanceHeader.structureCount + 1] = sru;
		mflData[5 * flightRecordMaintainanceHeader.structureCount + 2] =
				subModule;
		mflData[5 * flightRecordMaintainanceHeader.structureCount + 3] =
				fault.faultId;
		mflData[5 * flightRecordMaintainanceHeader.structureCount + 4] =
				fault.status;
		flightRecordMaintainanceHeader.structureCount++;
	}
	mfl.exists = fault.status;

	i = fault.faultId;
	if (module >= 1)
	{
		i += CPF_ERROR_COUNT;
	}
	if (module >= 2)
	{
		i += PPF_ERROR_COUNT;
	}
	if (module >= 3)
	{
		i += CCP_ERROR_COUNT;
	}
	if (module >= 4)
	{
		i += TP_ERROR_COUNT;
	}
	if (module >= 5)
	{
		i += DIFM_ERROR_COUNT;
	}
	if (module >= 6)
	{
		i += CWP_ERROR_COUNT;
	}
	if (module >= 7)
	{
		i += DCM_ERROR_COUNT;
	}
	if(i>=TOTAL_ERROR_COUNT)
	{
		printf("Unknown ERRROR in self test error Module:%d Eid: %d\n",module,fault.faultId);
		return;
	}
	mfl.sys1 = 7;
	mfl.sys2 = 6;
	mfl.subsys = 'A';
	strcpy((char*) mfl.lru, ConstHigh::faultLru[i]);

	// Fctn1 is non-zero for temperature faults only, for the rest is 0
	if (i == 11 || i == 37)
	{
		mfl.fctn1 = 1;
	}
	else if (i == 44 || i == 56 || i == 57 || i == 58)
	{
		mfl.fctn1 = 2;
	}
	else if (i == 112 || i == 133)
	{
		mfl.fctn1 = 3;
	}
	else
	{
		mfl.fctn1 = 0;
	}
	mfl.fctn2 = ConstHigh::faultFctn[i];

	strcpy((char*) mfl.description, ConstHigh::faultDescriptor[i]);

	if (mfl.exists)
	{
		fillPflStructure(i);
	}

	// write mfl updated values
	mflHandler.writeMflFile(filename, sizeof(MflStructure), (UWord8*) &mfl);
	mflHandler.writeCurrentMflFile(filename, i, mfl.amount, mfl.exists);
}

void SelfTestMflHigh::fillSpecialPfls(UWord32 channelStatus)
{
	UWord32 antWiseResults = 0;
//	antWiseResults |= ((channelStatus & 0x0001) >> 0);
//	antWiseResults |= ((channelStatus & 0x0010) >> 3);
//	antWiseResults |= ((channelStatus & 0x0100) >> 6);
//	antWiseResults |= ((channelStatus & 0x1000) >> 9);
//
//	antWiseResults |= ((channelStatus & 0x0002) << 3);
//	antWiseResults |= ((channelStatus & 0x0020) >> 0);
//	antWiseResults |= ((channelStatus & 0x0200) >> 5);
//	antWiseResults |= ((channelStatus & 0x2000) >> 10);
//
//	antWiseResults |= ((channelStatus & 0x0004) >> 2);
//	antWiseResults |= ((channelStatus & 0x0040) >> 5);
//	antWiseResults |= ((channelStatus & 0x0400) >> 8);
//	antWiseResults |= ((channelStatus & 0x4000) >> 11);
//
//	antWiseResults |= ((channelStatus & 0x0008) >> 3);
//	antWiseResults |= ((channelStatus & 0x0080) >> 6);
//	antWiseResults |= ((channelStatus & 0x0800) >> 9);
//	antWiseResults |= ((channelStatus & 0x8000) >> 12);

	if ((channelStatus & 0x1111) == 0)
	{
		pfl.pflLevel4.frontLeftRxFailure = 1;
	}
	if ((channelStatus & 0x2222) == 0)
	{
		pfl.pflLevel4.backLeftRxFailure = 1;
	}

	if ((channelStatus & 0x4444) == 0)
	{
		pfl.pflLevel4.backRightRxFailure = 1;
	}

	if ((channelStatus & 0x8888) == 0)
	{
		pfl.pflLevel4.frontRightRxFailure = 1;
	}
}
//This function fills PFL structure with respect to index given to it
void SelfTestMflHigh::fillPflStructure(UWord8 index)
{
	switch (index)
	{
	case 1:
		// N/A
		break;
	case 2:
		pfl.pflLevel3.sysCntlrFailure = 1;
		break;
	case 3:
		pfl.pflLevel4.audioFailure = 1;
		break;
	case 4:
		pfl.pflLevel4.audioFailure = 1;
		break;
	case 5:
		pfl.pflLevel4.audioFailure = 1;
		break;
	case 6:
		// N/A
		break;
	case 7:
		// N/A
		break;
	case 8:
		// N/A
		break;
	case 9:
		pfl.pflLevel3.sysCntlrFailure = 1;
		break;
	case 10:
		// N/A
		break;
	case 11:
		pfl.pflLevel3.sysCntlrFailure = 1;
		break;
	case 12:
		pfl.pflLevel3.sysCntlrFailure = 1;
		break;
	case 13:
		pfl.pflLevel3.sysCntlrFailure = 1;
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 14:
		pfl.pflLevel3.sysCntlrFailure = 1;
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 15:
		pfl.pflLevel3.sysCntlrFailure = 1;
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 16:
		pfl.pflLevel3.sysCntlrFailure = 1;
		break;
	case 17:
		pfl.pflLevel3.sysCntlrFailure = 1;
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 18:
		pfl.pflLevel3.sysCntlrFailure = 1;
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 19:
		pfl.pflLevel4.cwFailure = 1;
		break;
	case 20:
		pfl.pflLevel4.cwFailure = 1;
		break;
	case 21:
		// N/A
		break;
	case 22:
		// N/A
		break;
	case 23:
		// N/A
		break;
	case 24:
		// N/A
		break;
	case 25:
		// N/A
		break;
	case 26:
		pfl.pflLevel3.sysCntlrFailure = 1;
		break;
	case 27:
		pfl.pflLevel4.recordingFailure = 1;
		break;
	case 28:
		pfl.pflLevel4.opsTlFailure = 1;
		break;
	case 29:
		pfl.pflLevel4.recordingFailure = 1;
		break;
	case 30:
		//alag; FOR TESTING SYNTHETIC FAILURE
		pfl.pflLevel4.audioFailure = 1;
		break;
	case 31:
		pfl.pflLevel4.recordingFailure = 1;
		break;
	case 32:
		pfl.pflLevel4.opsTlFailure = 1;
		break;
	case 33:
		pfl.pflLevel4.opsTlFailure = 1;
		break;
	case 34:
		pfl.pflLevel4.opsTlFailure = 1;
		break;
	case 35:
		pfl.pflLevel4.opsTlFailure = 1;
		break;
	case 36:
		// N/A
		break;
	case 37:
		// N/A
		break;
	case 38:
		pfl.pflLevel3.sysCntlrFailure = 1;
		break;
	case 39:
		pfl.pflLevel3.sysCntlrFailure = 1;
		break;
	case 40:
		// N/A
		break;
	case 41:
		// N/A
		break;
	case 42:
		// N/A
		break;
	case 43:
		pfl.pflLevel3.sysCntlrFailure = 1;
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 44:
		pfl.pflLevel3.sysCntlrFailure = 1;
		break;
	case 45:
		pfl.pflLevel3.sysCntlrFailure = 1;
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 46:
		// N/A
		break;
	case 47:
		// N/A
		break;
	case 48:
		pfl.pflLevel3.sysCntlrFailure = 1;
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 49:
		pfl.pflLevel3.sysCntlrFailure = 1;
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 50:
		pfl.pflLevel3.sysCntlrFailure = 1;
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 51:
		pfl.pflLevel3.sysCntlrFailure = 1;
		break;
	case 52:
		pfl.pflLevel3.sysCntlrFailure = 1;
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 53:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 54:
		// N/A
		break;
	case 55:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 56:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 57:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 58:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 59:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 60:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 61:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 62:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 63:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 64:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 65:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 66:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 67:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 68:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 69:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 70:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 71:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 72:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 73:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 74:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 75:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 76:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 77:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 78:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 79:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 80:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 81:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 82:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 83:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 84:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 85:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 86:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 87:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 88:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 89:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 90:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 91:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 92:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 93:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 94:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 95:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 96:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 97:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 98:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 99:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 100:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 101:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 102:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 103:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 104:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 105:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 106:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 107:
		// N/A
		break;
	case 108:
		// N/A
		break;
	case 109:
		pfl.pflLevel4.cwFailure = 1;
		break;
	case 110:
		pfl.pflLevel3.sysCntlrFailure = 1;
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 111:
		pfl.pflLevel4.cwFailure = 1;
		break;
	case 112:
		pfl.pflLevel4.cwFailure = 1;
		break;
	case 113:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 114:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 115:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 116:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 117:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 118:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 119:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 120:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 121:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 122:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 123:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 124:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 125:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 126:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 127:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 128:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 129:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 130:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 131:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 132:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 133:
		pfl.pflLevel3.detectionFailure = 1;
		break;
	case 134:
		pfl.pflLevel3.sysCntlrFailure = 1;
		break;
	case 135:
		pfl.pflLevel4.cwFailure = 1;
		break;
	case 136:
		pfl.pflLevel4.frontRightRxFailure = 1;
		break;
	case 137:
		pfl.pflLevel4.frontLeftRxFailure = 1;
		break;
	case 138:
		pfl.pflLevel4.backRightRxFailure = 1;
		break;
	case 139:
		pfl.pflLevel4.backLeftRxFailure = 1;
		break;
	}
}

//This function extracts results out of readData
void SelfTestMflHigh::fillFaultPacketInfo(UWord32 readData)
{
	UWord8 type = readData >> 30;

	fault.packetId = type;
	fault.module = (readData >> 25) & 0x07;
	fault.faultId = (readData >> 17) & 0xFF;
	fault.count = (readData >> 1) & 0xFFFF;
	fault.status = readData & 0x00000001;
}


UWord8 SelfTestMflHigh::checkPfl3Existence(Level3Pfl &pflStruct)
{
	char* a=(char*)&pflStruct;
	UWord8 pflCount=0;
	for(UWord8 i=0;i<sizeof(Level3Pfl);i++)
	{
		if(a[i]==1)
		{
			pflCount++;
		}
	}
	return (pflCount>0);
}
UWord8 SelfTestMflHigh::checkPfl4Existence(Level4Pfl &pflStruct)
{
	char* a=(char*)&pflStruct;
	UWord8 pflCount=0;
	for(UWord8 i=0;i<sizeof(Level4Pfl);i++)
	{
		if(a[i]==1)
		{
			pflCount++;
		}
	}
	return (pflCount>0);
}

UWord8 SelfTestMflHigh::checkNewPfl3Existence()
{
	char* a=(char*)&oldPfls.pflLevel3;
	char* b=(char*)&pfl.pflLevel3;
	UWord8 newPfl=0;
	for(UWord8 i=0;i<sizeof(Level3Pfl);i++)
	{
		if((a[i]!=b[i]) &(b[i]==1))
		{
			newPfl++;
		}
	}

	return (newPfl>0);
}
UWord8 SelfTestMflHigh::checkNewPfl4Existence()
{
	char*a=(char*)&oldPfls.pflLevel4;
	char*b=(char*)&pfl.pflLevel4;
	UWord8 newPfl=0;
	for(UWord8 i=0;i<sizeof(Level4Pfl);i++)
	{
		if((a[i]!=b[i]) &(b[i]==1))
		{
			newPfl++;
		}
	}

	return (newPfl>0);
}

UWord8 SelfTestMflHigh::checkPflFlagChanged()
{
	char*a=(char*)&oldPfls.pflLevel3;
	char*b=(char*)&pfl.pflLevel3;
	UWord8 flagChangedCount=0;
	for(UWord8 i=0;i<sizeof(Level3Pfl);i++)
	{
		if((a[i]!=b[i]))
		{
			flagChangedCount++;
		}
	}

	a=(char*)&oldPfls.pflLevel4;
	b=(char*)&pfl.pflLevel4;
	for(UWord8 i=0;i<sizeof(Level4Pfl);i++)
	{
		if((a[i]!=b[i]))
		{
			flagChangedCount++;
		}
	}

	return (flagChangedCount>0);
}

void SelfTestMflHigh::clearPflStruct()
{
	// newPfl3 = 0;
	// newPfl4 = 0;
	oldPfls = pfl;
	memset(&pfl, 0, sizeof(PflStructure));

	// as cfd update mechanism is diffrent
	pfl.pflLevel3.cfdCommFailure 		= oldPfls.pflLevel3.cfdCommFailure;
}

void SelfTestMflHigh::updatePflFlags(UWord8 selfTestType)
{

	if(selfTestType == 0)
	{
		// pflRelatedAsyncSopNeededFlag = 1;
	}
	else if(selfTestType == 1)
	{
		pfl.pflLevel4.frontRightRxFailure	= oldPfls.pflLevel4.frontRightRxFailure;
		pfl.pflLevel4.frontLeftRxFailure	= oldPfls.pflLevel4.frontLeftRxFailure;
		pfl.pflLevel4.backLeftRxFailure		= oldPfls.pflLevel4.backLeftRxFailure;
		pfl.pflLevel4.backRightRxFailure	= oldPfls.pflLevel4.backRightRxFailure;
		if(pfl.pflLevel4.frontRightRxFailure || pfl.pflLevel4.frontLeftRxFailure || pfl.pflLevel4.backLeftRxFailure || pfl.pflLevel4.backRightRxFailure)
		{
			pfl.pflLevel3.detectionFailure		= 1;
		}
	}
	else if(selfTestType ==2)
	{

	}


	newPfl3 = newPfl3 || checkNewPfl3Existence();
	newPfl4 = newPfl4 || checkNewPfl4Existence();
	existencePfl3 = checkPfl3Existence(pfl.pflLevel3);
	existencePfl4 = checkPfl4Existence(pfl.pflLevel4);

	if((newPfl3 || newPfl4) && checkPflFlagChanged())
	{
		pflRelatedAsyncSopNeededFlag = 1;
	}


	oldPfls = pfl;
}

void SelfTestMflHigh::updateCfdCommFlag(UWord8 cfdUpStatus)
{
	// UWord8 statusChangeFlag=0;
	// if(((pfl.pflLevel3.cfdCommFailure==1) ^ (cfdUpStatus==0))==1)
	// {
	// 	statusChangeFlag = 1;
	// }
	if(cfdUpStatus==0)
	{
		pfl.pflLevel3.cfdCommFailure=1;
	}
	else
	{
		pfl.pflLevel3.cfdCommFailure=0;
	}
	// if(statusChangeFlag==1)
	{
		updatePflFlags(0);
	}
}


void SelfTestMflHigh::onPflStatusMessageSent()
{
	oldPfls = pfl;
	pflRelatedAsyncSopNeededFlag = 0;
	newPfl3 = 0;
	newPfl4 = 0;
}
