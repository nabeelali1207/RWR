/*
 * cfduartdelegate.cpp
 *
 *  Created on: Mar 29, 2021
 *      Author: zumair
 */
#define CFDUARTDELEGATE_CPP_

#include "../delegate/cfduartdelegate.h"

int cfdResult = 0;
int cfdLoadInit = 0;
bool nack = false;

cfdStatusRequest cfdStatus;
cfdBitRequest cfdBit;
cfdThreatData cfdThreat;
cfdPfmLoad cfdLoad[16];
cfdPfmRequest cfdRequest;
cfdPfmUpdate update;

uint64_t cfdStatusLastTime, cfdBitLastTime, cfdPfmLoadLastTime,
		cfdNowTime;

void cfdSendAutoPrograms(CfdLibrary &cfdLib)
{
	int count = 0;
	for (auto i=0; i<cfdLib.cfdHeader.totalSequences; i++)
	{
		memset(&cfdLoad, 0, sizeof(cfdPfmLoad) * 16);

		cfdLoad[i].programId = cfdLib.cfdProgram[i].programId;
		cfdLoad[0].numChaffGroup = cfdLib.cfdProgram[i].numChaffGroup;
		cfdLoad[0].intervalChaffGroup =
				cfdLib.cfdProgram[i].intervalChaffGroup;
		cfdLoad[0].numChaffPerGroup =
				cfdLib.cfdProgram[i].chaffPerGroup;
		cfdLoad[0].chaffSides =
				cfdLib.cfdProgram[i].sglDblChaff;
		cfdLoad[0].intervalChaff =
				cfdLib.cfdProgram[i].intervalChaff;
		cfdLoad[0].numFlare =
				cfdLib.cfdProgram[i].numFlare;
		cfdLoad[0].intervalFlare =
				cfdLib.cfdProgram[i].intervalFlare;
		cfdLoad[0].flareSides =
				cfdLib.cfdProgram[i].sglDblFlare;

		printf("AUTO CFD: %d, C: %d F: %d\n", cfdLoad[0].programId,
				cfdLoad[0].numChaffPerGroup,
				cfdLoad[0].numFlare);


		cfdResult = handlePfmLoad(cfdLoad[0]);

		while (cfdResult == 2)
		{
			if (count >= 2)
			{
				Model::selfTestMflHigh.updateCfdCommFlag(0);

				// send NACK SOP for new PFL
				sendAsyncSop(2, 4, 1);
				ConstHigh::cfdPause = 0;
				return;
			}

			usleep(110000);
			cfdResult = handlePfmLoad(cfdLoad[0]);
			count++;
		}
		count = 0;

		if (cfdResult != 2)
		{
			if (loadResponse.Nack)
			{
				nack = true;
			}
			usleep(110000);
		}
	}
	memset(&cfdLoad, 0, sizeof(cfdPfmLoad) * 16);

	usleep(110000);
	// Send Empty
	cfdResult = handlePfmLoad(cfdLoad[0]);

	while (cfdResult == 2)
	{
		if (count >= 2)
		{
			
			Model::selfTestMflHigh.updateCfdCommFlag(0);

			// send NACK SOP for new PFL
			sendAsyncSop(2, 4, 1);
			ConstHigh::cfdPause = 0;
			return;
		}

		usleep(110000);
		cfdResult = handlePfmLoad(cfdLoad[0]);
		count++;
	}
	count = 0;

	if (cfdResult != 2)
	{
		if (loadResponse.Nack)
		{
			nack = true;
		}
		usleep(110000);
	}
}

void handleCfdCommunication(M1ewCfdProgram &cfdIn)
{
	int count = 0, progNum = 0;
	UWord8 i = 0, lb = cfdIn.blockInfo.blockId;

	usleep(15000);

	if (lb == 1)
	{
		if (cfdLoadInit == 0)
		{
			cfdLoadInit = 1;
			memset(&cfdLoad, 0, sizeof(cfdPfmLoad) * 16);
		}

		ConstHigh::cfdPause = 1;

		for (i = 0; i < 15; i++)
		{
			cfdLoad[i].programId = i + 1;
		}

		for (i = 0; i < 8; i++)
		{
			progNum = cfdIn.dispenseProg[i].dispenseProgNum.dispenseProgNum;

			if (progNum != 0)
			{
				cfdLoad[progNum - 1].numChaffGroup =
						cfdIn.dispenseProg[i].dispenseProgParams.numChaffGroup;
				cfdLoad[progNum - 1].intervalChaffGroup =
						cfdIn.dispenseProg[i].dispenseProgParams.intervalChaffGroup;
				cfdLoad[progNum - 1].numChaffPerGroup =
						cfdIn.dispenseProg[i].dispenseProgParams.chaffPerGroup;
				cfdLoad[progNum - 1].chaffSides =
						cfdIn.dispenseProg[i].dispenseProgParams.sglDblChaff;
				cfdLoad[progNum - 1].intervalChaff =
						cfdIn.dispenseProg[i].dispenseProgParams.intervalChaff;
				cfdLoad[progNum - 1].numFlare =
						cfdIn.dispenseProg[i].dispenseProgParams.numFlare;
				cfdLoad[progNum - 1].intervalFlare =
						cfdIn.dispenseProg[i].dispenseProgParams.intervalFlare;
				cfdLoad[progNum - 1].flareSides =
						cfdIn.dispenseProg[i].dispenseProgParams.sglDblFlare;
			}

			printf("DTC CFD: %d, C: %d F: %d\n", cfdLoad[progNum - 1].programId,
					cfdLoad[progNum - 1].numChaffPerGroup,
					cfdLoad[progNum - 1].numFlare);
		}

		if (cfdIn.blockInfo.lastBlock == 1)
		{
			for (i = 0; i < 16; i++)
			{
				cfdResult = handlePfmLoad(cfdLoad[i]);

				while (cfdResult == 2)
				{
					if (count >= 2)
					{
						
						Model::selfTestMflHigh.updateCfdCommFlag(0);

						// send NACK SOP for new PFL
						sendAsyncSop(2, 4, lb);
						ConstHigh::cfdPause = 0;
						return;
					}

					usleep(110000);
					cfdResult = handlePfmLoad(cfdLoad[i]);
					count++;
				}
				count = 0;

				if (cfdResult != 2)
				{
					if (loadResponse.Nack)
					{
						nack = true;
					}
					usleep(110000);
				}
			}

			printf("/n/t dtc E\n");
			usleep(60000);

			// Send 0x0E message
			cfdResult = handleReferenceRequest();
			while (cfdResult == 2)
			{
				if (count >= 2)
				{
					// NACK
					Model::selfTestMflHigh.updateCfdCommFlag(0);

					// send ASYNC SOP for new PFL
					sendAsyncSop(2, 4, lb);
					ConstHigh::cfdPause = 0;
					return;
				}

				usleep(20000);
				cfdResult = handleReferenceRequest();
				count++;
			}
			count = 0;

			if ((nack) || cfdResult == 2)
			{
				nack = false;
				// send NACK SOP
				sendAsyncSop(2, 4, lb);
			}
			else
			{
				// send ACK SOP
				sendAsyncSop(1, 4, lb);
			}
			ConstHigh::cfdPause = 0;
		}
	}
	else if (lb == 2)
	{
		printf("/n/t Preset - R\n");
		ConstHigh::presetChaff = cfdIn.presetCF.presetChaff;
		ConstHigh::presetFlare = cfdIn.presetCF.presetFlare;

		cfdResult = handleStatusRequest(cfdStatus);

		if (cfdResult != 2)
		{
			// send ACK SOP
			sendAsyncSop(1, 4, lb);
			cfdStatusLastTime = Utility::getSystemTime();
		}
		else
		{
			// send NACK SOP
			sendAsyncSop(2, 4, lb);
		}
		cfdStatusLastTime = Utility::getSystemTime();
		cfdBitLastTime = Utility::getSystemTime();
	}
}

void handleCfdCommunication(M1ewCfdRequest &cfdIn, Ewm1CfdRequest &cfdOut)
{
	int count = 0, code = 0;
	memset(&cfdOut, 0, sizeof(Ewm1CfdRequest));

	usleep(15000);
	if ((cfdIn.cfdDataRequest.cfRequest == 1)
			|| (cfdIn.cfdDataRequest.progTypeRequest == 1))
	{
		printf("\tCFD-R B1 REQ\n");
		code = 1;
		cfdResult = 1;
	}

	if ((cfdIn.cfdParams.dispenseProgNum > 0)
			|| (cfdIn.cfdParams.dispenseType > 0))
	{
		printf("\tCFD-R Update REQ\n");
		code = 2;
		if (cfdIn.cfdParams.dispenseProgNum != 0)
		{
			ConstHigh::programNum = cfdIn.cfdParams.dispenseProgNum;
			handleCfdStatusRequest();
		}

		if (cfdIn.cfdParams.dispenseType != 0)
		{
			update.programId = cfdIn.cfdParams.dispenseProgNum;
			update.type = cfdIn.cfdParams.dispenseType;

			cfdResult = handlePfmUpdate(update);
			while (cfdResult == 2)
			{
				if (count >= 2)
				{
					Model::selfTestMflHigh.updateCfdCommFlag(0);

					// send NACK SOP for new PFL
					sendAsyncSop(2, 3, 0);
					break;
				}

				usleep(60000);
				cfdResult = handlePfmUpdate(update);
				count++;
			}
			count = 0;

			if (cfdResult != 2)
			{
				if (updateResponse.Nack)
				{
					// send NACK SOP here
					sendAsyncSop(2, 3, 0);
				}
			}
		}

		usleep(60000);
		cfdRequest.programId = cfdIn.cfdParams.dispenseProgNum;
		cfdResult = handlePfmRequest(cfdRequest);
		while (cfdResult == 2)
		{
			if (count >= 2)
			{
				Model::selfTestMflHigh.updateCfdCommFlag(0);

				// send NACK SOP for new PFL
				sendAsyncSop(2, 3, 0);
				break;
			}

			usleep(60000);
			cfdResult = handlePfmRequest(cfdRequest);
			count++;
		}
		count = 0;
	}

	if (cfdResult != 2)
	{
		sendRequestOutToTrenz(0);

		if (code == 1)
		{
			printf("\tCFD-R B1 Response\n");
		}
		else if (code == 2)
		{
			cfdRequestOut.cfdProgData.dispenseProgNum =
					pfmReqResponse.programId;
			cfdRequestOut.cfdProgData.dispenseType = pfmReqResponse.type;
			printf("\tCFD-R Update Response\n");
			printf("\tPFM Pid: %d, T: %d\n", pfmReqResponse.programId,
					pfmReqResponse.type);
		}
		printf("\tSR Pid: %d, T: %d\n", statusResponse.programNum,
				statusResponse.programType);
	}
}

void handleCfdListRequest(M1ewCfdRequest &cfdIn, Ewm1CfdProgram &cfdOut)
{
	int i = 0, count = 0;
	memset(&cfdOut, 0, sizeof(Ewm1CfdProgram));

	if (cfdIn.cfdDataRequest.progListRequest == 1)
	{
		usleep(15000);
		for (i = 0; i < 15; i++)
		{
			cfdRequest.programId = i + 1;
			cfdResult = handlePfmRequest(cfdRequest);
			while (cfdResult == 2)
			{
				if (count >= 2)
				{
					Model::selfTestMflHigh.updateCfdCommFlag(0);

					// send NACK SOP for new PFL
					sendAsyncSop(2, 3, 0);
					return;
				}

				usleep(50000);
				cfdResult = handlePfmRequest(cfdRequest);
				count++;
			}
			count = 0;

			if (cfdResult != 2)
			{
				cfdOut.dispenseProg[i].numChaffGroup =
						pfmReqResponse.numChaffGroup;
				cfdOut.dispenseProg[i].intervalChaffGroup =
						pfmReqResponse.intervalChaffGroup;
				cfdOut.dispenseProg[i].chaffPerGroup =
						pfmReqResponse.chaffPerGroup;
				cfdOut.dispenseProg[i].sglDblChaff = pfmReqResponse.sglDblChaff;
				cfdOut.dispenseProg[i].intervalChaff =
						pfmReqResponse.intervalChaff;
				cfdOut.dispenseProg[i].numFlare = pfmReqResponse.numFlare;
				cfdOut.dispenseProg[i].intervalFlare =
						pfmReqResponse.intervalFlare;
				cfdOut.dispenseProg[i].sglDblFlare = pfmReqResponse.sglDblFlare;

				printf("LR CFD: %d, C: %d F: %d\n", pfmReqResponse.programId,
						pfmReqResponse.chaffPerGroup, pfmReqResponse.numFlare);
			}
			else
			{
				nack = true;
			}
		}

		if (nack)
		{
			nack = false;
			// send NACK SOP
			sendAsyncSop(2, 3, 0);
		}
	}
}

void handleCfdStatusRequest()
{
	cfdStatus.modeValidity = 1;

	if (cfdStatus.manualProgram != ConstHigh::programNum)
	{
		cfdStatus.manualProgValidity = 1;
	}
	if (cfdStatus.presetChaff != ConstHigh::presetChaff)
	{
		cfdStatus.presetChaffValidity = 1;
	}
	if (cfdStatus.presetFlare != ConstHigh::presetFlare)
	{
		cfdStatus.presetFlareValidity = 1;
	}

	cfdStatus.mode = ConstHigh::ecmMode;
	cfdStatus.manualProgram = ConstHigh::programNum;
	cfdStatus.presetChaff = ConstHigh::presetChaff;
	cfdStatus.presetFlare = ConstHigh::presetFlare;
}

void cfdUnitStatusCheck(UWord8 code)
{
//	int count = 0;

	if (code == 1)
	{
		memset(&cfdThreat, 0, sizeof(cfdThreatData));

		cfdStatus.modeValidity = 1;
		cfdStatus.manualProgValidity = 0;
		cfdStatus.presetChaffValidity = 0;
		cfdStatus.presetFlareValidity = 0;
		cfdStatus.mode = ConstHigh::ecmMode;
		cfdStatus.manualProgram = 15;
		cfdStatus.presetChaff = 1;
		cfdStatus.presetFlare = 1;
	}
//	cfdResult = handleStatusRequest(cfdStatus);
//
//	while (cfdResult == 2)
//	{
//		if (count >= 2)
//		{
//			break;
//		}
//
//		usleep(10000);
//		cfdResult = handleStatusRequest(cfdStatus);
//		count++;
//	}
//	count = 0;
//
//	if (cfdResult != 2)
//	{
//		printf("\n\t A - ecmMode: %d\n", statusResponse.mode);
//		ConstHigh::cfdInit = 1;
//		ConstHigh::cfdUp = 1;
//		ConstHigh::cfdFailCount = 0;
//
//		ConstHigh::cfdType = statusResponse.programType;
//		ConstHigh::programNum = statusResponse.programNum;
//		ConstHigh::presetChaff = statusResponse.presetChaff;
//		ConstHigh::presetFlare = statusResponse.presetFlare;
//
//		cfdRequestOut.cfdProgData.dispenseStatus = 2;
//		ConstHigh::cfdType = cfdRequestOut.cfdProgData.dispenseType;
//
//		sendRequestOutToTrenz(1);
//
//		if (code == 1)
//		{
//			usleep(12000);
//
//			// Send 0x0E message
//			cfdResult = handleReferenceRequest();
//			while (cfdResult == 2)
//			{
//				if (count >= 2)
//				{
//					// NACK
//					Model::selfTestMflHigh.pfl.pflLevel3.cfdCommFailure =
//							1;
//					ConstHigh::cfdUp = 0;
//					return;
//				}
//
//				usleep(10000);
//				cfdResult = handleReferenceRequest();
//				count++;
//			}
//			count = 0;
//
//			if (cfdResult != 2)
//			{
//				usleep(12000);
//
//				// Send 0x16 message
//				cfdResult = handlePartNumRequest();
//				while (cfdResult == 2)
//				{
//					if (count >= 2)
//					{
//						// NACK
//						Model::selfTestMflHigh.pfl.pflLevel3.cfdCommFailure =
//								1;
//						ConstHigh::cfdUp = 0;
//						return;
//					}
//
//					usleep(10000);
//					cfdResult = handlePartNumRequest();
//					count++;
//				}
//				count = 0;
//			}
//		}
//	}
//	else
//	{
//		printf("\t\n CFD-FAIL \n");
//	}


	cfdStatusLastTime = Utility::getSystemTime();
	cfdBitLastTime = Utility::getSystemTime();
}

void cfdFillThreatData(vector<TrackDefaultClass> &ejTracks,
		vector<TrackDefaultClass> &cdTracks,
		vector<TrackDefaultClass> &kBandTracks,
		vector<TrackDefaultClass> &cwTracks)
{
	UWord8 i = 0, id = 0, threatFound = 0, maxPriority = 0, currentPriority = 0;
	UWord8 trackType = 0;
	cfdThreatData cfdThreatLocal;
	memset(&cfdThreatLocal, 0, sizeof(cfdThreatData));

	if (ConstHigh::FstInProgress == 1)
	{
		return;
	}

	for (i = 0; i < cdTracks.size(); i++)
	{
		currentPriority = cdTracks[i].general.priorityDynamic;
		if ((currentPriority > maxPriority) && (cdTracks[i].cfd.validCfdThreat == 1))
		{
			id = i;
			maxPriority = currentPriority;
			trackType = 0;
		}
	}

	for (i = 0; i < ejTracks.size(); i++)
	{
		currentPriority = ejTracks[i].general.priorityDynamic;
		if ((currentPriority > maxPriority) && (ejTracks[i].cfd.validCfdThreat == 1))
		{
			id = i;
			maxPriority = currentPriority;
			trackType = 1;
		}
	}

	for (i = 0; i < cwTracks.size(); i++)
	{
		currentPriority = cwTracks[i].general.priorityDynamic;
		if ((currentPriority > maxPriority) && (cwTracks[i].cfd.validCfdThreat == 1))
		{
			id = i;
			maxPriority = currentPriority;
			trackType = 2;
		}
	}

	for (i = 0; i < kBandTracks.size(); i++)
	{
		currentPriority = kBandTracks[i].general.priorityDynamic;
		if ((currentPriority > maxPriority) && (kBandTracks[i].cfd.validCfdThreat == 1))
		{
			id = i;
			maxPriority = currentPriority;
			trackType = 3;
		}
	}

	if (trackType == 0)
	{
		if (cdTracks.size() > 0)
		{
			threatFound = 1;

			cfdThreatLocal.threatId = cdTracks[id].cfd.threatIndex;
			cfdThreatLocal.cmId = cdTracks[id].cfd.cmProgramId;
			cfdThreatLocal.altitudeValidity = cdTracks[id].cfd.altitudeValidity;
			cfdThreatLocal.velocityValidity = cdTracks[id].cfd.velocityValidity;
			cfdThreatLocal.aoa = cdTracks[id].cfd.aoa;
			cfdThreatLocal.status = 1;
			cfdThreatLocal.altitude = cdTracks[id].cfd.altitude;
			cfdThreatLocal.velocity = cdTracks[id].cfd.velocity;
		}
	}
	else if (trackType == 1)
	{
		if (ejTracks.size() > 0)
		{
			threatFound = 1;

			cfdThreatLocal.threatId = ejTracks[id].cfd.threatIndex;
			cfdThreatLocal.cmId = ejTracks[id].cfd.cmProgramId;
			cfdThreatLocal.altitudeValidity = ejTracks[id].cfd.altitudeValidity;
			cfdThreatLocal.velocityValidity = ejTracks[id].cfd.velocityValidity;
			cfdThreatLocal.aoa = ejTracks[id].cfd.aoa;
			cfdThreatLocal.status = 1;
			cfdThreatLocal.altitude = ejTracks[id].cfd.altitude;
			cfdThreatLocal.velocity = ejTracks[id].cfd.velocity;
		}
	}
	else if (trackType == 2)
	{
		if (cwTracks.size() > 0)
		{
			threatFound = 1;

			cfdThreatLocal.threatId = cwTracks[id].cfd.threatIndex;
			cfdThreatLocal.cmId = cwTracks[id].cfd.cmProgramId;
			cfdThreatLocal.altitudeValidity = cwTracks[id].cfd.altitudeValidity;
			cfdThreatLocal.velocityValidity = cwTracks[id].cfd.velocityValidity;
			cfdThreatLocal.aoa = cwTracks[id].cfd.aoa;
			cfdThreatLocal.status = 1;
			cfdThreatLocal.altitude = cwTracks[id].cfd.altitude;
			cfdThreatLocal.velocity = cwTracks[id].cfd.velocity;
		}
	}
	else if (trackType == 3)
	{
		if (kBandTracks.size() > 0)
		{
			threatFound = 1;

			cfdThreatLocal.threatId = kBandTracks[id].cfd.threatIndex;
			cfdThreatLocal.cmId = kBandTracks[id].cfd.cmProgramId;
			cfdThreatLocal.altitudeValidity =
					kBandTracks[id].cfd.altitudeValidity;
			cfdThreatLocal.velocityValidity =
					kBandTracks[id].cfd.velocityValidity;
			cfdThreatLocal.aoa = kBandTracks[id].cfd.aoa;
			cfdThreatLocal.status = 1;
			cfdThreatLocal.altitude = kBandTracks[id].cfd.altitude;
			cfdThreatLocal.velocity = kBandTracks[id].cfd.velocity;
		}
	}

	if (cfdThreatLocal.threatId != cfdThreat.threatId)
	{
		if ((cfdThreat.status == 0) || (cfdThreat.status == 3))
		{
			cfdThreat.threatId = cfdThreatLocal.threatId;
			cfdThreat.cmId = cfdThreatLocal.cmId;
			cfdThreat.altitudeValidity = cfdThreatLocal.altitudeValidity;
			cfdThreat.velocityValidity = cfdThreatLocal.velocityValidity;
			cfdThreat.aoa = cfdThreatLocal.aoa;
			cfdThreat.status = cfdThreatLocal.status;
			cfdThreat.altitude = cfdThreatLocal.altitude;
			cfdThreat.velocity = cfdThreatLocal.velocity;
		}
		else if ((cfdThreat.status == 1) || (cfdThreat.status == 2))
		{
			cfdThreat.status = 3;
		}
	}
	else
	{
		cfdThreat.threatId = cfdThreatLocal.threatId;
		cfdThreat.cmId = cfdThreatLocal.cmId;
		cfdThreat.altitudeValidity = cfdThreatLocal.altitudeValidity;
		cfdThreat.velocityValidity = cfdThreatLocal.velocityValidity;
		cfdThreat.aoa = cfdThreatLocal.aoa;
		cfdThreat.status = 2;
		cfdThreat.altitude = cfdThreatLocal.altitude;
		cfdThreat.velocity = cfdThreatLocal.velocity;
	}

	if (threatFound == 1)
	{
		handleThreatData(cfdThreat);
		usleep(50000);
	}
}

void cfdPeriodicStatusCheck()
{
	Word32 timeDifference = 0;

	if (ConstHigh::cfdPause == 0)
	{
		cfdNowTime = Utility::getSystemTime();

		timeDifference = (cfdNowTime -cfdStatusLastTime)/1000;

		if (timeDifference >= 200)
		{
			usleep(15000);
//			printf("\n TD: %d\n", timeDifference);

//			printf("\n\t ecmMode: %d\n", cfdStatus.mode);
			cfdResult = handleStatusRequest(cfdStatus);

			if (cfdResult != 2)
			{
				ConstHigh::cfdUp = 1;
				ConstHigh::cfdFailCount = 0;

				sendRequestOutToTrenz(1);
				cfdStatus.manualProgram = statusResponse.programNum;
				if((timeDifference%100)==50)
				{
					printf("CFD: (P %d, T: %d)\n", statusResponse.programNum,
							statusResponse.programType);
				}
				if (Model::selfTestMflHigh.pfl.pflLevel3.cfdCommFailure == 1)
				{
					Model::selfTestMflHigh.updateCfdCommFlag(1);

					// send ASYNC SOP for PFL change
					sendAsyncSop(3, 0, 0);
				}
			}
			else
			{
				ConstHigh::cfdFailCount++;
				if (ConstHigh::cfdFailCount >= 3)
				{
					if (ConstHigh::cfdFailCount%20==0)
					{
						printf("CFD-FAIL \n");
					}
					if (ConstHigh::cfdUp != 0)
					{
						ConstHigh::cfdUp = 0;
					}
					if (Model::selfTestMflHigh.pfl.pflLevel3.cfdCommFailure
							!= 1)
					{
						Model::selfTestMflHigh.updateCfdCommFlag(0);

						// send ASYNC SOP for new PFL
						sendAsyncSop(3, 0, 0);
					}
				}
			}

			if (cfdStatus.manualProgram != ConstHigh::programNum)
			{
				cfdStatus.manualProgValidity = 1;
			}
			else
			{
				cfdStatus.manualProgValidity = 0;
			}
			if (cfdStatus.presetChaff != ConstHigh::presetChaff)
			{
				cfdStatus.presetChaffValidity = 1;
			}
			else
			{
				cfdStatus.presetChaffValidity = 0;
			}
			if (cfdStatus.presetFlare != ConstHigh::presetFlare)
			{
				cfdStatus.presetFlareValidity = 1;
			}
			else
			{
				cfdStatus.presetFlareValidity = 0;
			}
			cfdStatusLastTime = Utility::getSystemTime();
		}
	}
}

void cfdPeriodicBitRequest()
{
	Word32 timeDifference = 0;

	if ((ConstHigh::cfdPause == 0) && (ConstHigh::cfdUp == 1))
	{
		cfdNowTime = Utility::getSystemTime();

		timeDifference = (cfdNowTime-cfdBitLastTime)/1000;

		if ((ConstHigh::cfdUp) && (timeDifference > 6000))
		{
			usleep(15000);

			cfdBit.rtc1 = Model::fileoutputflightrecord.RTC_TIME >> 24;
			cfdBit.rtc2 = Model::fileoutputflightrecord.RTC_TIME >> 16;
			cfdBit.rtc3 = Model::fileoutputflightrecord.RTC_TIME >> 8;
			cfdBit.rtc4 = Model::fileoutputflightrecord.RTC_TIME;

			cfdBit.year1 = sopIn.year.Year >> 8;
			cfdBit.year2 = sopIn.year.Year;
			cfdBit.month = sopIn.mmdd.Month;
			cfdBit.date = sopIn.mmdd.Day;

			cfdResult = handleBitRequest(cfdBit);

			if (cfdResult == 1)
			{
				if ((bitResponse.errorCode1 != 0)
						|| (bitResponse.errorCode2 != 0)
						|| (bitResponse.errorCode3 != 0)
						|| (bitResponse.errorCode4 != 0)
						|| (bitResponse.errorCode5 != 0))
				{
					if (Model::selfTestMflHigh.pfl.pflLevel3.cfdCommFailure
							!= 1)
					{
						
						Model::selfTestMflHigh.updateCfdCommFlag(0);

						// send ASYNC SOP for new PFL
						sendAsyncSop(3, 0, 0);
					}
				}
			}
			usleep(15000);
			cfdBitLastTime = Utility::getSystemTime();
		}
	}
}

void sendRequestOutToTrenz(UWord8 code)
{
	cfdRequestOut.cfdCount.numChaff = 0;
	cfdRequestOut.cfdCount.numFlare = 0;
	if (statusResponse.rightTop == 0)
	{
		cfdRequestOut.cfdCount.numChaff += statusResponse.rightTopInventory;
	}
	else if (statusResponse.rightTop == 1)
	{
		cfdRequestOut.cfdCount.numFlare += statusResponse.rightTopInventory;
	}

	if (statusResponse.rightBottom == 0)
	{
		cfdRequestOut.cfdCount.numChaff += statusResponse.rightBottomInventory;
	}
	else if (statusResponse.rightBottom == 1)
	{
		cfdRequestOut.cfdCount.numFlare += statusResponse.rightBottomInventory;
	}

	if (statusResponse.leftTop == 0)
	{
		cfdRequestOut.cfdCount.numChaff += statusResponse.leftTopInventory;
	}
	else if (statusResponse.leftTop == 1)
	{
		cfdRequestOut.cfdCount.numFlare += statusResponse.leftTopInventory;
	}

	if (statusResponse.leftBottom == 0)
	{
		cfdRequestOut.cfdCount.numChaff += statusResponse.leftBottomInventory;
	}
	else if (statusResponse.leftBottom == 1)
	{
		cfdRequestOut.cfdCount.numFlare += statusResponse.leftBottomInventory;
	}

	cfdRequestOut.cfdProgData.dispenseProgNum = statusResponse.programNum;
	cfdRequestOut.cfdProgData.dispenseType = statusResponse.programType;

	cfdRequestOut.cfdProgData.dispenseMode = statusResponse.mode;

	cfdRequestOut.cfdPreset.presetChaff = statusResponse.presetChaff;
	cfdRequestOut.cfdPreset.presetFlare = statusResponse.presetFlare;

	if (code == 1)
	{
		if (statusResponse.dispensing == 1)
		{
			cfdRequestOut.cfdProgData.dispenseStatus =
					statusResponse.dispensing;
			cfdRequestOut.cfdProgData.dispenseProgNum =
					statusResponse.currentProgram;
			if ((statusResponse.chaffType == 1)
					&& (statusResponse.flareType == 1))
			{
				cfdRequestOut.cfdProgData.dispenseType = 3;
			}
			else if (statusResponse.chaffType == 1)
			{
				cfdRequestOut.cfdProgData.dispenseType = 2;
			}
			else if (statusResponse.flareType == 1)
			{
				cfdRequestOut.cfdProgData.dispenseType = 1;
			}

			fillEwm1CfdRequestBuffer(&cfdRequestOut);

			Model::trenzSpiLink.writeReplyToTrenz(13, 3, 0);
		}
		else if ((statusResponse.dispensing == 0)
				&& (cfdRequestOut.cfdProgData.dispenseStatus == 1))
		{
			cfdRequestOut.cfdProgData.dispenseStatus = 2;
			cfdRequestOut.cfdProgData.dispenseProgNum =
					statusResponse.programNum;
			cfdRequestOut.cfdProgData.dispenseType = statusResponse.programType;

			fillEwm1CfdRequestBuffer(&cfdRequestOut);

			Model::trenzSpiLink.writeReplyToTrenz(13, 3, 0);
		}
//		else if ((statusResponse.dispensing == 0)
//				&& (cfdRequestOut.cfdProgData.dispenseStatus == 2))
//		{
//			cfdRequestOut.cfdProgData.dispenseStatus = 0;
//
//			fillEwm1CfdRequestBuffer(&cfdRequestOut);
//
//			Model::trenzSpiLink.writeReplyToTrenz(13, 3, 0);
//		}
	}
}
