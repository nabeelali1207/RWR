/*
 * threatprocessorselftestdelegate.cpp
 *
 *  Created on: Feb 23, 2021
 *      Author: saad
 */

#include "threatprocessorselftestdelegate.h"

//This function goes through all the EJ emitters and extracts
//parameters of self test emitters, if self test emitters are
//according to VDM specied criterion, self test is passed
void fillPacketInfofindTpSelfTestEmitters(SelfTestHighDelegate &stDelegate,
		DataSensorClass &bandEJ)
{
	UWord32 i = 0;
	UWord32 FST_PULSE_AMP_THRESHOLD = 1300;    //tentative
	UWord32 FST_PULSE_RF_THRESHOLD = 145;     //tentative
	UWord32 maxPower = 0;

//	UWord32 vdmAmpLower = FST_PULSE_AMP_THRESHOLD - 100;
//	UWord32 vdmAmpHigher = FST_PULSE_AMP_THRESHOLD + 800;

	UWord32 vdmAmpLower = 1100;
	UWord32 vdmAmpHigher = 3000;

	UWord32 difmAmpLower = FST_PULSE_RF_THRESHOLD - 100;
//	UWord32 difmAmpHigher = FST_PULSE_RF_THRESHOLD + 800;
	if (ConstHigh::FstInProgress == 0)
	{
		return;
	}
	for (i = 0; i < bandEJ.tracks.size(); i++)
	{
		if (bandEJ.tracks[i].general.flagSelfTest == 0)
		{
			continue;
		}
		int32_t flippedAngle = 360 - (int32_t) bandEJ.tracks[i].angle.curAngle;
		int32_t threatAntenna = 0;
		if (flippedAngle > 40 && flippedAngle < 50)
		{
			threatAntenna = 0;
		}
		else if (flippedAngle > 130 && flippedAngle < 140)
		{
			threatAntenna = 1;
		}
		else if (flippedAngle > 220 && flippedAngle < 230)
		{
			threatAntenna = 2;
		}
		else if (flippedAngle > 310 && flippedAngle < 320)
		{
			threatAntenna = 3;
		}

		TrackDefaultClass &track = bandEJ.tracks[i];
		maxPower = max(bandEJ.tracks[i].power.pxMax,
				bandEJ.tracks[i].power.pyMax);
		printf("STE-- Ant: %d Band: %d RF: %d, PRI: %d, PW: %d, vdmPow: %d, difmPow: %d\n",
				threatAntenna, track.rf.bandCode,
				track.rf.rfCircular[0].rfAvg, track.pri.priCircular[0].priMean,
				track.pw.pwCircular[0].pwAvg, maxPower, track.rf.rfAmplitude);

		if (bandEJ.tracks[i].pri.curPriType == PriTypeEnum::pri_fixed)
		{
			if ((bandEJ.tracks[i].pri.priCircular[0].priMean > 495)
					&& (bandEJ.tracks[i].pri.priCircular[0].priMean < 505))
			{
				stDelegate.tpSelfTestResults.pulseParamResult += 1;
			}
		}

		if (bandEJ.tracks[i].pw.pwCircular.size() > 0)
		{
			if (bandEJ.tracks[i].pw.pwCircular[0].pwAvg > 250
					&& bandEJ.tracks[i].pw.pwCircular[0].pwAvg < 350)
			{
				stDelegate.tpSelfTestResults.pulseParamResult += 1;
			}
		}

		if ((bandEJ.tracks[i].rf.rfCircular.size() == 0)
				|| (bandEJ.tracks[i].rf.curRfType != RfTypeEnum::rf_fixed))
		{
			continue;
		}
		if (bandEJ.tracks[i].rf.bandCode == BandCodeEnum::band_e)
		{

			if (threatAntenna == 0)
			{
				if ((bandEJ.tracks[i].rf.rfCircular[0].rfAvg > 3900)
						&& (bandEJ.tracks[i].rf.rfCircular[0].rfAvg < 4100))
				{
					stDelegate.tpSelfTestResults.tpDetectionResults |= 0x0001;
				}
				if ((maxPower > vdmAmpLower) && (maxPower < vdmAmpHigher))
				{
					stDelegate.tpSelfTestResults.tpPulseAmpResults |= 0x0001;
				}
				if (bandEJ.tracks[i].rf.rfAmplitude > difmAmpLower)
				{
					stDelegate.tpSelfTestResults.tpRfAmpResults |= 0x0001;
				}
			}
			else if (threatAntenna == 1)
			{
				if ((bandEJ.tracks[i].rf.rfCircular[0].rfAvg > 3900)
						&& (bandEJ.tracks[i].rf.rfCircular[0].rfAvg < 4100))
				{
					stDelegate.tpSelfTestResults.tpDetectionResults |= 0x0002;
				}
				if ((maxPower > vdmAmpLower) && (maxPower < vdmAmpHigher))
				{
					stDelegate.tpSelfTestResults.tpPulseAmpResults |= 0x0002;
				}
				if (bandEJ.tracks[i].rf.rfAmplitude > difmAmpLower)
				{
					stDelegate.tpSelfTestResults.tpRfAmpResults |= 0x0002;
				}
			}
			else if (threatAntenna == 2)
			{
				if ((bandEJ.tracks[i].rf.rfCircular[0].rfAvg > 3900)
						&& (bandEJ.tracks[i].rf.rfCircular[0].rfAvg < 4100))
				{
					stDelegate.tpSelfTestResults.tpDetectionResults |= 0x0004;
				}
				if ((maxPower > vdmAmpLower) && (maxPower < vdmAmpHigher))
				{
					stDelegate.tpSelfTestResults.tpPulseAmpResults |= 0x0004;
				}
				if (bandEJ.tracks[i].rf.rfAmplitude > difmAmpLower)
				{
					stDelegate.tpSelfTestResults.tpRfAmpResults |= 0x0004;
				}
			}
			else if (threatAntenna == 3)
			{
				if ((bandEJ.tracks[i].rf.rfCircular[0].rfAvg > 3900)
						&& (bandEJ.tracks[i].rf.rfCircular[0].rfAvg < 4100))
				{
					stDelegate.tpSelfTestResults.tpDetectionResults |= 0x0008;
				}
				if ((maxPower > vdmAmpLower) && (maxPower < vdmAmpHigher))
				{
					stDelegate.tpSelfTestResults.tpPulseAmpResults |= 0x0008;
				}
				if (bandEJ.tracks[i].rf.rfAmplitude > difmAmpLower)
				{
					stDelegate.tpSelfTestResults.tpRfAmpResults |= 0x0008;
				}
			}
		}
		else if (bandEJ.tracks[i].rf.bandCode == BandCodeEnum::band_g)
		{
			if (threatAntenna == 0)
			{
				if ((bandEJ.tracks[i].rf.rfCircular[0].rfAvg > 7900)
						&& (bandEJ.tracks[i].rf.rfCircular[0].rfAvg < 8100))
				{
					stDelegate.tpSelfTestResults.tpDetectionResults |= 0x0010;
				}
				if ((maxPower > vdmAmpLower) && (maxPower < vdmAmpHigher))
				{
					stDelegate.tpSelfTestResults.tpPulseAmpResults |= 0x0010;
				}
				if (bandEJ.tracks[i].rf.rfAmplitude > difmAmpLower)
				{
					stDelegate.tpSelfTestResults.tpRfAmpResults |= 0x0010;
				}
			}
			else if (threatAntenna == 1)
			{
				if ((bandEJ.tracks[i].rf.rfCircular[0].rfAvg > 7900)
						&& (bandEJ.tracks[i].rf.rfCircular[0].rfAvg < 8100))
				{
					stDelegate.tpSelfTestResults.tpDetectionResults |= 0x0020;
				}
				if ((maxPower > vdmAmpLower) && (maxPower < vdmAmpHigher))
				{
					stDelegate.tpSelfTestResults.tpPulseAmpResults |= 0x0020;
				}
				if (bandEJ.tracks[i].rf.rfAmplitude > difmAmpLower)
				{
					stDelegate.tpSelfTestResults.tpRfAmpResults |= 0x0020;
				}
			}
			else if (threatAntenna == 2)
			{
				if ((bandEJ.tracks[i].rf.rfCircular[0].rfAvg > 7900)
						&& (bandEJ.tracks[i].rf.rfCircular[0].rfAvg < 8100))
				{
					stDelegate.tpSelfTestResults.tpDetectionResults |= 0x0040;
				}
				if ((maxPower > vdmAmpLower) && (maxPower < vdmAmpHigher))
				{
					stDelegate.tpSelfTestResults.tpPulseAmpResults |= 0x0040;
				}
				if (bandEJ.tracks[i].rf.rfAmplitude > difmAmpLower)
				{
					stDelegate.tpSelfTestResults.tpRfAmpResults |= 0x00040;
				}
			}
			else if (threatAntenna == 3)
			{
				if ((bandEJ.tracks[i].rf.rfCircular[0].rfAvg > 7900)
						&& (bandEJ.tracks[i].rf.rfCircular[0].rfAvg < 8100))
				{
					stDelegate.tpSelfTestResults.tpDetectionResults |= 0x0080;
				}
				if ((maxPower > vdmAmpLower) && (maxPower < vdmAmpHigher))
				{
					stDelegate.tpSelfTestResults.tpPulseAmpResults |= 0x0080;
				}
				if (bandEJ.tracks[i].rf.rfAmplitude > difmAmpLower)
				{
					stDelegate.tpSelfTestResults.tpRfAmpResults |= 0x0080;
				}
			}
		}
		else if (bandEJ.tracks[i].rf.bandCode == BandCodeEnum::band_i)
		{
			if (threatAntenna == 0)
			{
				if ((bandEJ.tracks[i].rf.rfCircular[0].rfAvg > 11900)
						&& (bandEJ.tracks[i].rf.rfCircular[0].rfAvg < 12100))
				{
					stDelegate.tpSelfTestResults.tpDetectionResults |= 0x0100;
				}
				if ((maxPower > vdmAmpLower) && (maxPower < vdmAmpHigher))
				{
					stDelegate.tpSelfTestResults.tpPulseAmpResults |= 0x0100;
				}
				if (bandEJ.tracks[i].rf.rfAmplitude > difmAmpLower)
				{
					stDelegate.tpSelfTestResults.tpRfAmpResults |= 0x0100;
				}
			}
			else if (threatAntenna == 1)
			{
				if ((bandEJ.tracks[i].rf.rfCircular[0].rfAvg > 11900)
						&& (bandEJ.tracks[i].rf.rfCircular[0].rfAvg < 12100))
				{
					stDelegate.tpSelfTestResults.tpDetectionResults |= 0x0200;
				}
				if ((maxPower > vdmAmpLower) && (maxPower < vdmAmpHigher))
				{
					stDelegate.tpSelfTestResults.tpPulseAmpResults |= 0x0200;
				}
				if (bandEJ.tracks[i].rf.rfAmplitude > difmAmpLower)
				{
					stDelegate.tpSelfTestResults.tpRfAmpResults |= 0x0200;
				}
			}
			else if (threatAntenna == 2)
			{
				if ((bandEJ.tracks[i].rf.rfCircular[0].rfAvg > 11900)
						&& (bandEJ.tracks[i].rf.rfCircular[0].rfAvg < 12100))
				{
					stDelegate.tpSelfTestResults.tpDetectionResults |= 0x0400;
				}
				if ((maxPower > vdmAmpLower) && (maxPower < vdmAmpHigher))
				{
					stDelegate.tpSelfTestResults.tpPulseAmpResults |= 0x0400;
				}
				if (bandEJ.tracks[i].rf.rfAmplitude > difmAmpLower)
				{
					stDelegate.tpSelfTestResults.tpRfAmpResults |= 0x0400;
				}
			}
			else if (threatAntenna == 3)
			{
				if ((bandEJ.tracks[i].rf.rfCircular[0].rfAvg > 11900)
						&& (bandEJ.tracks[i].rf.rfCircular[0].rfAvg < 12100))
				{
					stDelegate.tpSelfTestResults.tpDetectionResults |= 0x0800;
				}
				if ((maxPower > vdmAmpLower) && (maxPower < vdmAmpHigher))
				{
					stDelegate.tpSelfTestResults.tpPulseAmpResults |= 0x0800;
				}
				if (bandEJ.tracks[i].rf.rfAmplitude > difmAmpLower)
				{
					stDelegate.tpSelfTestResults.tpRfAmpResults |= 0x0800;
				}
			}
		}
		else if (bandEJ.tracks[i].rf.bandCode == BandCodeEnum::band_j)
		{
			if (threatAntenna == 0)
			{
				if ((bandEJ.tracks[i].rf.rfCircular[0].rfAvg > 15900)
						&& (bandEJ.tracks[i].rf.rfCircular[0].rfAvg < 16100))
				{
					stDelegate.tpSelfTestResults.tpDetectionResults |= 0x1000;
				}
				if ((maxPower > vdmAmpLower) && (maxPower < vdmAmpHigher))
				{
					stDelegate.tpSelfTestResults.tpPulseAmpResults |= 0x1000;
				}
				if (bandEJ.tracks[i].rf.rfAmplitude > difmAmpLower)
				{
					stDelegate.tpSelfTestResults.tpRfAmpResults |= 0x1000;
				}
			}
			else if (threatAntenna == 1)
			{
				if ((bandEJ.tracks[i].rf.rfCircular[0].rfAvg > 15900)
						&& (bandEJ.tracks[i].rf.rfCircular[0].rfAvg < 16100))
				{
					stDelegate.tpSelfTestResults.tpDetectionResults |= 0x2000;
				}
				if ((maxPower > vdmAmpLower) && (maxPower < vdmAmpHigher))
				{
					stDelegate.tpSelfTestResults.tpPulseAmpResults |= 0x2000;
				}
				if (bandEJ.tracks[i].rf.rfAmplitude > difmAmpLower)
				{
					stDelegate.tpSelfTestResults.tpRfAmpResults |= 0x2000;
				}
			}
			else if (threatAntenna == 2)
			{
				if ((bandEJ.tracks[i].rf.rfCircular[0].rfAvg > 15900)
						&& (bandEJ.tracks[i].rf.rfCircular[0].rfAvg < 16100))
				{
					stDelegate.tpSelfTestResults.tpDetectionResults |= 0x4000;
				}
				if ((maxPower > vdmAmpLower) && (maxPower < vdmAmpHigher))
				{
					stDelegate.tpSelfTestResults.tpPulseAmpResults |= 0x4000;
				}
				if (bandEJ.tracks[i].rf.rfAmplitude > difmAmpLower)
				{
					stDelegate.tpSelfTestResults.tpRfAmpResults |= 0x4000;
				}
			}
			else if (threatAntenna == 3)
			{
				if ((bandEJ.tracks[i].rf.rfCircular[0].rfAvg > 15900)
						&& (bandEJ.tracks[i].rf.rfCircular[0].rfAvg < 16100))
				{
					stDelegate.tpSelfTestResults.tpDetectionResults |= 0x8000;
				}
				if ((maxPower > vdmAmpLower) && (maxPower < vdmAmpHigher))
				{
					stDelegate.tpSelfTestResults.tpPulseAmpResults |= 0x8000;
				}
				if (bandEJ.tracks[i].rf.rfAmplitude > difmAmpLower)
				{
					stDelegate.tpSelfTestResults.tpRfAmpResults |= 0x8000;
				}
			}
		}
//		printf("Rf: 0x%04x, vdmPow:  0x%04x , rfPow: 0x%04x\n",
//				stDelegate.tpSelfTestResults.tpDetectionResults,
//				stDelegate.tpSelfTestResults.tpPulseAmpResults,
//				stDelegate.tpSelfTestResults.tpRfAmpResults);
	}     // end of for

}

