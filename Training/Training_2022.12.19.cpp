////1
//#include <iostream>
//#include <fstream>
//#include <bitset>
//#include "Training_2022.12.19_myVector.h"
//
//using namespace std;
//
//struct Data
//{
//	uint64_t TOA = 0;
//	uint16_t PW = 0;
//	uint16_t FMR_Frequency = 0;
//	unsigned char FMR_Antenna_a = 0;
//	bool FMR_Valid = 0;
//	unsigned char Power_Antenna_A = 0;
//	unsigned char Power_Antenna_B = 0;
//	unsigned char Power_Antenna_C = 0;
//	unsigned char Power_Antenna_D = 0;
//	unsigned short int Freq_Band = 0;
//	uint16_t FEED = 0;
//};
//
//int main()
//{
//	myVector<Data> v;
//	const int size = 20;
//	unsigned char dataPacket[size];
//	unsigned char *seek;
//	Data test;// = new Data;
//
//	ifstream inFile("filtered.bin", ios::binary);
//	if (inFile)
//		inFile.read((char *)dataPacket, size * sizeof(char));
//	inFile.close();
//	
//	test.TOA = ((unsigned long long int)(0x1f & dataPacket[9]) << 43) | ((unsigned long long int)(dataPacket[8]) << 32) | ((unsigned long long int)(dataPacket[7]) << 24) | (dataPacket[6] << 16) | (dataPacket[5] << 8) | dataPacket[4];
//	test.PW = ((0x07 & dataPacket[11]) << 11) | (dataPacket[10] << 3) | ((0xe0 & dataPacket[9]) >> 5);
//	test.FMR_Frequency = (0x07 & dataPacket[13]) << 13 | dataPacket[12] << 5 | ((0xf8 & dataPacket[11]) >> 3);
//	test.FMR_Antenna_a = ((0x18 & dataPacket[13]) >> 3);
//	test.FMR_Valid = ((0x20 & dataPacket[13]) >> 5);
//	test.Power_Antenna_A = ((0x3f & dataPacket[14]) << 2) | ((0xc0 & dataPacket[13]) >> 6);
//	test.Power_Antenna_B = ((0x3f & dataPacket[15]) << 2) | ((0xc0 & dataPacket[14]) >> 6);
//	test.Power_Antenna_C = ((0x3f & dataPacket[16]) << 2) | ((0xc0 & dataPacket[15]) >> 6);
//	test.Power_Antenna_D = ((0x3f & dataPacket[17]) << 2) | ((0xc0 & dataPacket[16]) >> 6);
//	test.Freq_Band = ((0xc0 & dataPacket[17]) >> 6);
//	test.FEED = (dataPacket[19] << 8) | dataPacket[18];
//
//	v.push_back(test);
//
//	for (int i = 0; i < size; i++)
//	{
//		cout << hex << (int)dataPacket[i] << " ";
//	}
//	cout << endl << endl;
//	cout << "Expected test->TOA output: 00 12 af 5f 3f 7c\n";
//	cout << "TOA = 0x" << dec << test.TOA << endl << endl;
//	cout << "Expected test->PW output: 01 74\n";
//	cout << "PW = 0x" << dec << test.PW << endl << endl;
//	cout << "Expected test->FMR_Frequency output: 7d 2d\n";
//	cout << "FMR_Frequency = 0x" << dec << test.FMR_Frequency << endl << endl;
//	cout << "Expected test->FMR_Antenna_a output: 0\n";
//	cout << "FMR_Antenna_a = 0x" << dec << int(test.FMR_Antenna_a) << endl << endl;
//	cout << "Expected test->FMR_Valid output: false\n";
//	cout << "FMR_Valid = " << test.FMR_Valid << endl << endl;
//	cout << "Expected test->Power_Antenna_A output: 59\n";
//	cout << "Power_Antenna_A = " << dec << 16 * int(test.Power_Antenna_A) << endl << endl;
//	cout << "Expected test->Power_Antenna_B output: 17\n";
//	cout << "Power_Antenna_B = " << dec << 16 * int(test.Power_Antenna_B) << endl << endl;
//	cout << "Expected test->Power_Antenna_C output: 14\n";
//	cout << "Power_Antenna_C = " << dec << 16 * int(test.Power_Antenna_C) << endl << endl;
//	cout << "Expected test->Power_Antenna_D output: 13\n";
//	cout << "Power_Antenna_D = " << dec << 16 * int(test.Power_Antenna_D) << endl << endl;
//	cout << "Expected test->Freq_Band output: 2\n";
//	cout << "Freq_Band = " << dec << int(test.Freq_Band) << endl << endl;
//	cout << "Expected test->FEED output: feed\n";
//	cout << "FEED = " << dec << int(test.FEED) << endl << endl;
//
//	cout << endl;
//	system("pause");
//	return 0;
//}










////2
//#include <iostream>
//#include <fstream>
//#include <bitset>
//#include "Training_2022.12.19_myVector.h"
//
//using namespace std;
//
//struct Data
//{
//	unsigned long long int TOA = 0;
//	unsigned short int PW = 0;
//	unsigned short int FMR_Frequency = 0;
//	unsigned char FMR_Antenna_a = 0;
//	bool FMR_Valid = 0;
//	unsigned char Power_Antenna_A = 0;
//	unsigned char Power_Antenna_B = 0;
//	unsigned char Power_Antenna_C = 0;
//	unsigned char Power_Antenna_D = 0;
//	unsigned short int Freq_Band = 0;
//	unsigned short int FEED = 0;
//
//	void printData()
//	{
//		cout << endl;
//		cout << "TOA = 0x" << hex << TOA << endl;
//		cout << "PW = 0x" << hex << PW << endl;
//		cout << "FMR_Frequency = 0x" << hex << FMR_Frequency << endl;
//		cout << "FMR_Antenna_a = 0x" << hex << int(FMR_Antenna_a) << endl;
//		cout << "FMR_Valid = " << FMR_Valid << endl;
//		cout << "Power_Antenna_A = " << hex << int(Power_Antenna_A) << endl;
//		cout << "Power_Antenna_B = " << hex << int(Power_Antenna_B) << endl;
//		cout << "Power_Antenna_C = " << hex << int(Power_Antenna_C) << endl;
//		cout << "Power_Antenna_D = " << hex << int(Power_Antenna_D) << endl;
//		cout << "Freq_Band = " << hex << int(Freq_Band) << endl;
//		cout << "FEED = " << hex << int(FEED) << endl << endl;
//
//	}
//};
//
//int main()
//{
//	myVector<Data> v;
//	myVector<Data>::myIterator<Data> it;
//	Data temp;
//	const int size = 20;
//	unsigned char dataPacket[size];
//	unsigned char *seek;
//
//	ifstream inFile("filtered.bin", ios::binary);
//	while (inFile)
//	{
//		inFile.read((char *)dataPacket, size * sizeof(char));
//		temp.TOA = ((unsigned long long int)(0x1f & dataPacket[9]) << 43) | ((unsigned long long int)(dataPacket[8]) << 32) | ((unsigned long long int)(dataPacket[7]) << 24) | (dataPacket[6] << 16) | (dataPacket[5] << 8) | dataPacket[4];
//		temp.PW = ((0x07 & dataPacket[11]) << 11) | (dataPacket[10] << 3) | ((0xe0 & dataPacket[9]) >> 5);
//		temp.FMR_Frequency = (0x07 & dataPacket[13]) << 13 | dataPacket[12] << 5 | ((0xf8 & dataPacket[11]) >> 3);
//		temp.FMR_Antenna_a = ((0x18 & dataPacket[13]) >> 3);
//		temp.FMR_Valid = ((0x20 & dataPacket[13]) >> 5);
//		temp.Power_Antenna_A = ((0x3f & dataPacket[14]) << 2) | ((0xc0 & dataPacket[13]) >> 6);
//		temp.Power_Antenna_B = ((0x3f & dataPacket[15]) << 2) | ((0xc0 & dataPacket[14]) >> 6);
//		temp.Power_Antenna_C = ((0x3f & dataPacket[16]) << 2) | ((0xc0 & dataPacket[15]) >> 6);
//		temp.Power_Antenna_D = ((0x3f & dataPacket[17]) << 2) | ((0xc0 & dataPacket[16]) >> 6);
//		temp.Freq_Band = ((0xc0 & dataPacket[17]) >> 6);
//		temp.FEED = (dataPacket[19] << 8) | dataPacket[18];
//		v.push_back(temp);
//	}
//	inFile.close();
//
//	cout << v.size();
//	for (int i = 0; i < 10; i++)
//	{
//		v[i].printData();
//	}
//
//	
//	for (int i = 0; i < size; i++)
//	{
//		cout << hex << (int)dataPacket[i] << " ";
//	}
//	
//	cout << endl;
//	system("pause");
//	return 0;
//}










////3
//#include <iostream>
//#include <vector>
//#include <fstream>
//#include <bitset>
//#include "Training_2022.12.19_myVector.h"
//
//using namespace std;
//
//struct Data
//{
//	unsigned long long int TOA = 0;
//	unsigned short int PW = 0;
//	unsigned short int FMR_Frequency = 0;
//	unsigned char FMR_Antenna_a = 0;
//	bool FMR_Valid = 0;
//	unsigned char Power_Antenna_A = 0;
//	unsigned char Power_Antenna_B = 0;
//	unsigned char Power_Antenna_C = 0;
//	unsigned char Power_Antenna_D = 0;
//	unsigned short int Freq_Band = 0;
//	unsigned short int FEED = 0;
//
//	void readFromBuffer(unsigned char *buffer)
//	{
//		TOA = ((unsigned long long int)(0x1f & *(buffer + 5)) << 43) | ((unsigned long long int)(*(buffer + 4)) << 32) | ((unsigned long long int)(*(buffer + 3)) << 24) | (*(buffer + 2) << 16) | (*(buffer + 1) << 8) | *buffer;
//		PW = ((0x07 & *(buffer + 7)) << 11) | (*(buffer + 6) << 3) | ((0xe0 & *(buffer + 5)) >> 5);
//		FMR_Frequency = (0x07 & *(buffer + 9)) << 13 | *(buffer + 8) << 5 | ((0xf8 & *(buffer + 7)) >> 3);
//		FMR_Antenna_a = ((0x18 & *(buffer + 9)) >> 3);
//		FMR_Valid = ((0x20 & *(buffer + 9)) >> 5);
//		Power_Antenna_A = ((0x3f & *(buffer + 10)) << 2) | ((0xc0 & *(buffer + 9)) >> 6);
//		Power_Antenna_B = ((0x3f & *(buffer + 11)) << 2) | ((0xc0 & *(buffer + 10)) >> 6);
//		Power_Antenna_C = ((0x3f & *(buffer + 12)) << 2) | ((0xc0 & *(buffer + 11)) >> 6);
//		Power_Antenna_D = ((0x3f & *(buffer + 13)) << 2) | ((0xc0 & *(buffer + 12)) >> 6);
//		Freq_Band = ((0xc0 & *(buffer + 13)) >> 6);
//		FEED = (*(buffer + 15) << 8) | *(buffer + 14);
//	}
//	void printTabularData()
//	{
//		cout << " " << dec << TOA;
//		cout << "\t" << dec << PW;
//		cout << "\t" << dec << FMR_Frequency;
//		cout << "\t   " << dec << int(FMR_Antenna_a);
//		cout << "\t    " << FMR_Valid;
//		cout << "\t  " << dec << 16 * int(Power_Antenna_A);
//		cout << "\t" << dec << 16 * int(Power_Antenna_B);
//		cout << "\t" << dec << 16 * int(Power_Antenna_C);
//		cout << "\t" << dec << 16 * int(Power_Antenna_D);
//		cout << "\t" << dec << int(Freq_Band);
//		cout << "\t" << hex << int(FEED) << endl;
//	}
//	void printData()
//	{
//		cout << "TOA = " << dec << TOA << endl;
//		cout << "PW = " << dec << PW << endl;
//		cout << "FMR_Frequency = " << dec << FMR_Frequency << endl;
//		cout << "FMR_Antenna_a = " << dec << int(FMR_Antenna_a) << endl;
//		cout << "FMR_Valid = " << FMR_Valid << endl;
//		cout << "Power_Antenna_A = " << dec << 16 * int(Power_Antenna_A) << endl;
//		cout << "Power_Antenna_B = " << dec << 16 * int(Power_Antenna_B) << endl;
//		cout << "Power_Antenna_C = " << dec << 16 * int(Power_Antenna_C) << endl;
//		cout << "Power_Antenna_D = " << dec << 16 * int(Power_Antenna_D) << endl;
//		cout << "Freq_Band = " << dec << int(Freq_Band) << endl;
//		cout << "FEED = " << hex << int(FEED) << endl << endl;
//	}
//};
//
//int main()
//{
//	myVector<Data> v;
//	myVector<Data>::myIterator<Data> it;
//	vector<Data> V;
//	Data tempBuffer;
//	int fileSize, count = 0;
//	unsigned char *buffer, *temp;
//
//	ifstream inFile("filtered.bin", ios::binary);
//	inFile.seekg(0, ios::end);
//	fileSize = inFile.tellg();
//	inFile.seekg(0, ios::beg);
//	buffer = (unsigned char *)malloc(fileSize);
//	if (inFile)
//		inFile.read((char *)buffer, fileSize);
//	inFile.close();
//
//	temp = buffer;
//	for (int i = 1; i <= fileSize; i++)
//	{
//		if (*temp == 0x55 && *(temp + 1) == 0xAA && *(temp + 2) == 0xFF && *(temp + 3) == 0xFF)
//		{
//			count++;
//			tempBuffer.readFromBuffer(temp + 4);
//			v.push_back(tempBuffer);
//			V.push_back(tempBuffer);
//		}
//		temp++;
//	}
//
//	cout << "     TOA\tPW\tFreq   Antenna_a  Valid    A\t B\t C\t D    Band\tFEED\n";
//	for (int i = 1; i <= count; i++)
//	{
//		v[i].printTabularData();
//		if (i >= 10)
//			break;
//	}
//	cout << endl;
//
//	cout << "     TOA\tPW\tFreq   Antenna_a  Valid    A\t B\t C\t D    Band\tFEED\n";
//	for (int i = 1; i <= count; i++)
//	{
//		V[i].printTabularData();
//		if (i >= 10)
//			break;
//	}
//	cout << endl;
//
//	cout << "fileSize = " << dec << fileSize << endl;
//	cout << "count = " << dec << count << endl;
//	cout << endl;
//
//	system("pause");
//	return 0;
//}
//
//
