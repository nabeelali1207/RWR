/*
 * inputmatlab.cpp
 *
 *  Created on: Oct 12, 2018
 *      Author: hassan
 */

#define FILEINPUTMATLAB_CPP_

#include "../io/fileinputmatlab.h"

void readPdwAllMatFileOfBasr(const char *matInFileName, const char *csvOutFileName,
		list<PdwDefaultClass> &inputPdwList, uint8_t writeOutputFlag)
{
//	FILE *outputFile;
//	UWord8  *N, *RFV, *Ant,  *BAND;
//	UWord16 *PW, *RF, *AMP, *AOA,*PA, *PB, *PC,*PX, *PY, *PD;
//	UWord64 *TOA;
//	UWord32 size;
//	UWord8 dataPresent;
//
//	Word8 token[] = "0,pd,pc,rfv,ant,PDW,N,pb,pa,px,py,rf";
//	Word8 curr_token[6];
//	Word8 *temp_token;
//	mat_t *matfp;
//	matvar_t *matvar;
//	UWord32 i, j;
//	const F64 *data;
//
//	PA = 0;
//	PB = 0;
//	PC = 0;
//	PD = 0;
//	N = 0;
//	RFV = 0;
//	Ant = 0;
//	PX = 0;
//	PY = 0;
//	BAND = 0;
//	PW = 0;
//	RF = 0;
//	AMP = 0;
//	AOA = 0;
//	TOA = 0;
//
//	matfp = Mat_Open(matInFileName, MAT_ACC_RDONLY);
//	if (matfp == NULL)
//	{
//		printf("\nMAT file not found\n");
//		fcloseall();
//		exit(1);
//	}
//
//	matvar = Mat_VarRead(matfp, "band");
//	if (matvar != NULL)
//	{
//		dataPresent = 1;
//		data = (const F64*) matvar->data;
//		BAND = (UWord8*) malloc(matvar->dims[0] * sizeof(UWord8));
//		for (i = 0, j = 0; i < matvar->dims[0]; i++, j++)
//		{
//			BAND[j] = (UWord8) data[i];
////			BAND[j] = 0;
//		}
//	}
//	else
//	{
//		dataPresent = 0;
//	}
//	if (dataPresent == 0)
//	{
//		cout << "No Data Found in Mat File" << endl;
//		return;
//	}
//	temp_token = strtok(token, ",");
//	while (1)
//	{
//		temp_token = strtok(NULL, ",");
//		if (temp_token == NULL)
//		{
//			break;
//		}
//		strcpy(curr_token, temp_token);
//		matvar = Mat_VarRead(matfp, curr_token);
//		if (matvar == NULL)
//		{
//			printf("\nVariable not present in MAT file\n");
//			Mat_Close(matfp);
//			fcloseall();
//			exit(3);
//		}
//		data = (const F64*) matvar->data;
//
//		if (strcmp(curr_token, "pd") == 0)
//		{
//			PD = (UWord16*) malloc(matvar->dims[0] * sizeof(UWord16));
//			for (i = 0, j = 0; i < matvar->dims[0]; i++, j++)
//			{
//				PD[j] = (UWord16) data[i];
//			}
//		}
//		if (strcmp(curr_token, "pc") == 0)
//		{
//			PC = (UWord16*) malloc(matvar->dims[0] * sizeof(UWord16));
//			for (i = 0, j = 0; i < matvar->dims[0]; i++, j++)
//			{
//				PC[j] = (UWord16) data[i];
//			}
//		}
//		if (strcmp(curr_token, "rfv") == 0)
//		{
//			RFV = (UWord8*) malloc(matvar->dims[0] * sizeof(UWord8));
//			for (i = 0, j = 0; i < matvar->dims[0]; i++, j++)
//			{
//				RFV[j] = (UWord8) data[i];
////				RFV[j] = (RFV[j] == 0);
//			}
//		}
//		if (strcmp(curr_token, "ant") == 0)
//		{
//			Ant = (UWord8*) malloc(matvar->dims[0] * sizeof(UWord8));
//			for (i = 0, j = 0; i < matvar->dims[0]; i++, j++)
//			{
//				Ant[j] = (UWord8) data[i];
//			}
//		}
//		if (strcmp(curr_token, "PDW") == 0)
//		{
//			AMP = (UWord16*) malloc(matvar->dims[0] * sizeof(UWord16));
//			PW = (UWord16*) malloc(matvar->dims[0] * sizeof(UWord16));
////			RF = (UWord16*) malloc(matvar->dims[0] * sizeof(UWord16));
//			AOA = (UWord16*) malloc(matvar->dims[0] * sizeof(UWord16));
//			TOA = (UWord64*) malloc(matvar->dims[0] * sizeof(UWord64));
//			for (i = 0, j = 0; i < (matvar->dims[0] * 1);
//					i++, j++)
//			{
//				AOA[j] = (UWord16) data[i];
//			}
//			for (i = (matvar->dims[0] * 1), j = 0; i < (matvar->dims[0] * 2);
//					i++, j++)
//			{
//				AMP[j] = (UWord16) data[i];
//			}
//			for (i = (matvar->dims[0] * 2), j = 0; i < (matvar->dims[0] * 3);
//					i++, j++)
//			{
//				PW[j] = (UWord16) data[i];
//			}
//			for (i = (matvar->dims[0] * 3), j = 0; i < (matvar->dims[0] * 4);
//					i++, j++)
//			{
//				TOA[j] = (UWord64) data[i];
//			}
//		}
//		if (strcmp(curr_token, "N") == 0)
//		{
//			N = (UWord8*) malloc(matvar->dims[0] * sizeof(UWord8));
//			for (i = 0, j = 0; i < matvar->dims[0]; i++, j++)
//			{
//				N[j] = (UWord8) data[i];
//			}
//		}
//		if (strcmp(curr_token, "pb") == 0)
//		{
//			PB = (UWord16*) malloc(matvar->dims[0] * sizeof(UWord16));
//			for (i = 0, j = 0; i < matvar->dims[0]; i++, j++)
//			{
//				PB[j] = (UWord16) data[i];
//			}
//		}
//		if (strcmp(curr_token, "pa") == 0)
//		{
//			PA = (UWord16*) malloc(matvar->dims[0] * sizeof(UWord16));
//			for (i = 0, j = 0; i < matvar->dims[0]; i++, j++)
//			{
//				PA[j] = (UWord16) data[i];
//			}
//		}
//		if (strcmp(curr_token, "px") == 0)
//		{
//			PX = (UWord16*) malloc(matvar->dims[0] * sizeof(UWord16));
//			for (i = 0, j = 0; i < matvar->dims[0]; i++, j++)
//			{
//				PX[j] = (UWord16) data[i];
//			}
//		}
//		if (strcmp(curr_token, "py") == 0)
//		{
//			PY = (UWord16*) malloc(matvar->dims[0] * sizeof(UWord16));
//			for (i = 0, j = 0; i < matvar->dims[0]; i++, j++)
//			{
//				PY[j] = (UWord16) data[i];
//			}
//		}
//		if (strcmp(curr_token, "rf") == 0)
//		{
//			RF = (UWord16*) malloc(matvar->dims[0] * sizeof(UWord16));
//			for (i = 0, j = 0; i < matvar->dims[0]; i++, j++)
//			{
//				RF[j] = (UWord16) data[i];
////				RF[j] = (RF[j] -2000)/0.244;
//			}
//		}
//	}
//	size = matvar->dims[0];
//
//	if (writeOutputFlag == 1)
//	{
//		outputFile = fopen(csvOutFileName, "w");
//		for (i = 0; i < size; i++)
//		{
//			fprintf(outputFile,
//					"%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%lu,%u,%u,%u\r\n", Ant[i],
//					BAND[i], N[i], PA[i], PB[i], PC[i], PD[i], RF[i], AOA[i],
//					AMP[i], PW[i], TOA[i], PX[i], PY[i], RFV[i]);
//		}
//		fclose(outputFile);
//	}
//
//	Mat_VarFree(matvar);
//	Mat_Close(matfp);
//	//copy all PDW from custom arrays into a list
//	for (i = 0; i < size; i++)
//	{
////		if (TOA[i] * 5 < 256148472965)
////		{
////			continue;
////		}
////		if (PA[i] < 16 && PB[i] < 16 && PC[i] < 16 && PD[i] < 16)
////		{
////			continue;
////		}
////		if((2.5f*PW[i]) >= 10 && (2.5f*PW[i]) <= 70)
////		if(RFV[i]==1)
//		{
//			PdwDefaultClass pdwObject(BAND[i], RFV[i], 0, N[i], Ant[i], PW[i],
//					AOA[i], PA[i], PB[i], PC[i], PD[i], PX[i], PY[i], AMP[i],
//					RF[i], TOA[i], 0);
//			inputPdwList.push_back(pdwObject);
//		}
//	}
//	//free memory allocated during this function
//	free(PA);
//	free(PB);
//	free(PC);
//	free(PD);
//	free(N);
//	free(RFV);
//	free(Ant);
//	free(PX);
//	free(PY);
//	free(BAND);
//	free(PW);
//	free(RF);
//	free(AMP);
//	free(AOA);
//	free(TOA);
}
void readPdwAllMatFile(const char *matInFileName, const char *csvOutFileName,
		list<PdwDefaultClass> &inputPdwList, uint8_t writeOutputFlag)
{
//	FILE *outputFile;
//	UWord8 *PA, *PB, *PC, *PD, *N, *RFV, *Ant, *PX, *PY, *BAND;
//	UWord16 *PW, *RF, *AMP, *AOA;
//	UWord64 *TOA;
//	UWord32 size;
//	UWord8 dataPresent;
//
//	Word8 token[] = "0,pd,pc,rfv,ant,PDW,N,pb,pa,px,py";
//	Word8 curr_token[6];
//	Word8 *temp_token;
//	mat_t *matfp;
//	matvar_t *matvar;
//	UWord32 i, j;
//	const F64 *data;
//
//	PA = 0;
//	PB = 0;
//	PC = 0;
//	PD = 0;
//	N = 0;
//	RFV = 0;
//	Ant = 0;
//	PX = 0;
//	PY = 0;
//	BAND = 0;
//	PW = 0;
//	RF = 0;
//	AMP = 0;
//	AOA = 0;
//	TOA = 0;
//
//	matfp = Mat_Open(matInFileName, MAT_ACC_RDONLY);
//	if (matfp == NULL)
//	{
//		printf("\nMAT file not found\n");
//		fcloseall();
//		exit(1);
//	}
//
//	matvar = Mat_VarRead(matfp, "band");
//	if (matvar != NULL)
//	{
//		dataPresent = 1;
//		data = (const F64*) matvar->data;
//		BAND = (UWord8*) malloc(matvar->dims[0] * sizeof(UWord8));
//		for (i = 0, j = 0; i < matvar->dims[0]; i++, j++)
//		{
//			BAND[j] = (UWord8) data[i];
////			BAND[j] = 0;
//		}
//	}
//	else
//	{
//		dataPresent = 0;
//	}
//	if (dataPresent == 0)
//	{
//		cout << "No Data Found in Mat File" << endl;
//		return;
//	}
//	temp_token = strtok(token, ",");
//	while (1)
//	{
//		temp_token = strtok(NULL, ",");
//		if (temp_token == NULL)
//		{
//			break;
//		}
//		strcpy(curr_token, temp_token);
//		matvar = Mat_VarRead(matfp, curr_token);
//		if (matvar == NULL)
//		{
//			printf("\nVariable not present in MAT file\n");
//			Mat_Close(matfp);
//			fcloseall();
//			exit(3);
//		}
//		data = (const F64*) matvar->data;
//
//		if (strcmp(curr_token, "pd") == 0)
//		{
//			PD = (UWord8*) malloc(matvar->dims[0] * sizeof(UWord8));
//			for (i = 0, j = 0; i < matvar->dims[0]; i++, j++)
//			{
//				PD[j] = (UWord8) data[i];
//			}
//		}
//		if (strcmp(curr_token, "pc") == 0)
//		{
//			PC = (UWord8*) malloc(matvar->dims[0] * sizeof(UWord8));
//			for (i = 0, j = 0; i < matvar->dims[0]; i++, j++)
//			{
//				PC[j] = (UWord8) data[i];
//			}
//		}
//		if (strcmp(curr_token, "rfv") == 0)
//		{
//			RFV = (UWord8*) malloc(matvar->dims[0] * sizeof(UWord8));
//			for (i = 0, j = 0; i < matvar->dims[0]; i++, j++)
//			{
//				RFV[j] = (UWord8) data[i];
//				RFV[j] = (RFV[j] == 0);
//			}
//		}
//		if (strcmp(curr_token, "ant") == 0)
//		{
//			Ant = (UWord8*) malloc(matvar->dims[0] * sizeof(UWord8));
//			for (i = 0, j = 0; i < matvar->dims[0]; i++, j++)
//			{
//				Ant[j] = (UWord8) data[i];
//			}
//		}
//		if (strcmp(curr_token, "PDW") == 0)
//		{
//			AMP = (UWord16*) malloc(matvar->dims[0] * sizeof(UWord16));
//			PW = (UWord16*) malloc(matvar->dims[0] * sizeof(UWord16));
//			RF = (UWord16*) malloc(matvar->dims[0] * sizeof(UWord16));
//			AOA = (UWord16*) malloc(matvar->dims[0] * sizeof(UWord16));
//			TOA = (UWord64*) malloc(matvar->dims[0] * sizeof(UWord64));
//			for (i = 0, j = 0; i < matvar->dims[0]; i++, j++)
//			{
//				RF[j] = (UWord16) data[i];
//			}
//			for (i = (matvar->dims[0] * 1), j = 0; i < (matvar->dims[0] * 2);
//					i++, j++)
//			{
//				AOA[j] = (UWord16) data[i];
//			}
//			for (i = (matvar->dims[0] * 2), j = 0; i < (matvar->dims[0] * 3);
//					i++, j++)
//			{
//				PW[j] = (UWord16) data[i];
//			}
//			for (i = (matvar->dims[0] * 3), j = 0; i < (matvar->dims[0] * 4);
//					i++, j++)
//			{
//				TOA[j] = (UWord16) data[i];
//			}
////			for (i = (matvar->dims[0] * 4), j = 0; i < (matvar->dims[0] * 5);
////					i++, j++)
////			{
////				TOA[j] = (UWord64) data[i];
////			}
//		}
//		if (strcmp(curr_token, "N") == 0)
//		{
//			N = (UWord8*) malloc(matvar->dims[0] * sizeof(UWord8));
//			for (i = 0, j = 0; i < matvar->dims[0]; i++, j++)
//			{
//				N[j] = (UWord8) data[i];
//			}
//		}
//		if (strcmp(curr_token, "pb") == 0)
//		{
//			PB = (UWord8*) malloc(matvar->dims[0] * sizeof(UWord8));
//			for (i = 0, j = 0; i < matvar->dims[0]; i++, j++)
//			{
//				PB[j] = (UWord8) data[i];
//			}
//		}
//		if (strcmp(curr_token, "pa") == 0)
//		{
//			PA = (UWord8*) malloc(matvar->dims[0] * sizeof(UWord8));
//			for (i = 0, j = 0; i < matvar->dims[0]; i++, j++)
//			{
//				PA[j] = (UWord8) data[i];
//			}
//		}
//		if (strcmp(curr_token, "px") == 0)
//		{
//			PX = (UWord8*) malloc(matvar->dims[0] * sizeof(UWord8));
//			for (i = 0, j = 0; i < matvar->dims[0]; i++, j++)
//			{
//				PX[j] = (UWord8) data[i];
//			}
//		}
//		if (strcmp(curr_token, "py") == 0)
//		{
//			PY = (UWord8*) malloc(matvar->dims[0] * sizeof(UWord8));
//			for (i = 0, j = 0; i < matvar->dims[0]; i++, j++)
//			{
//				PY[j] = (UWord8) data[i];
//			}
//		}
//	}
//	size = matvar->dims[0];
//
//	if (writeOutputFlag == 1)
//	{
//		outputFile = fopen(csvOutFileName, "w");
//		for (i = 0; i < size; i++)
//		{
//			fprintf(outputFile,
//					"%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%u,%lu,%u,%u,%u\r\n", Ant[i],
//					BAND[i], N[i], PA[i], PB[i], PC[i], PD[i], RF[i], AOA[i],
//					AMP[i], PW[i], TOA[i], PX[i], PY[i], RFV[i]);
//		}
//		fclose(outputFile);
//	}
//
//	Mat_VarFree(matvar);
//	Mat_Close(matfp);
//	//copy all PDW from custom arrays into a list
//	for (i = 0; i < size; i++)
//	{
////		if (TOA[i] * 5 < 256148472965)
////		{
////			continue;
////		}
////		if (PA[i] < 16 && PB[i] < 16 && PC[i] < 16 && PD[i] < 16)
////		{
////			continue;
////		}
////		if((2.5f*PW[i]) >= 10 && (2.5f*PW[i]) <= 70)
//		if(RFV[i]==1)
//		{
//			PdwDefaultClass pdwObject(BAND[i], RFV[i], 0, N[i], Ant[i], PW[i],
//					AOA[i], PA[i], PB[i], PC[i], PD[i], PX[i], PY[i], AMP[i],
//					RF[i], TOA[i], 0);
//			inputPdwList.push_back(pdwObject);
//		}
//	}
//	//free memory allocated during this function
//	free(PA);
//	free(PB);
//	free(PC);
//	free(PD);
//	free(N);
//	free(RFV);
//	free(Ant);
//	free(PX);
//	free(PY);
//	free(BAND);
//	free(PW);
//	free(RF);
//	free(AMP);
//	free(AOA);
//	free(TOA);
}
