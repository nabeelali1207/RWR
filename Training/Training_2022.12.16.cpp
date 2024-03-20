//1
//#include <iostream>
//#include <fstream>
//#include <bitset>
//
//using namespace std;
//
//int main()
//{
//	ofstream outf("sample.bin", ios::binary | std::ios::trunc); //binary and trunc flags
//	if (!outf)
//	{
//		cerr << "invalid\n";
//		return 1;
//	}
//	const int size = 32;
//	uint64_t arr[size];
//	for (int i = 0; i < size; i++)
//	{
//		arr[i] = uint64_t(i);
//	}
//	outf.write((char *)arr, size * sizeof(uint64_t));
//	//for (int i = 0; i < 128; i++)
//	//{
//	//	outf << arr[i];
//	//}
//	outf.close();
//
//	//const int size = 32;
//	int buffer[size];
//	ifstream inFILE("sample.bin", ios::binary | ios::in | ios::beg);
//	inFILE.seekg(0, ios::end);
//	int temp = inFILE.tellg();
//	cout << temp << endl;
//	inFILE.seekg(0, ios::beg);
//	//inFILE.read(reinterpret_cast<char *>(buffer), sizeof(buffer));
//	if (inFILE.is_open())
//	{
//		inFILE.read((char*)buffer, sizeof(buffer));
//	}
//	else
//		return 0;
//
//	for (int i = 0; i < size; i++)
//	{
//		//cout << bitset<32> (buffer[i]) << endl;
//		cout << buffer[i] << endl;
//	}
//
//	system("pause");
//	return 0;
//}










////2
//#include <iostream>
//#include <fstream>
//
//using namespace std;
//
//struct Data
//{
//	char c1;
//	int n1, n2;
//};
//
//int main()
//{
//	const int size = 10;
//	Data outTest1, inTest1, outTest2[size], inTest2[size];
//	Data *T;
//
//	outTest1.c1 = 3; outTest1.n1 = 10; outTest1.n2 = 20;
//	for (int i = 0; i < size; i++)
//	{
//		outTest2[i].c1 = i;
//		outTest2[i].n1 = i + 1;
//		outTest2[i].n2 = i + 2;
//	}
//
//	//Writing Struct
//	ofstream outFile1("test1.bin", ios::binary);
//	T = &outTest1;
//	if (outFile1)
//		outFile1.write((char *)T, sizeof(Data));
//	outFile1.close();
//	//Reading Struct
//	ifstream inFile1("test1.bin", ios::binary);
//	T = &inTest1;
//	if (inFile1)
//		inFile1.read((char *)T, sizeof(Data));
//	inFile1.close();
//
//	//Writing an array of Struct
//	ofstream outFile2("test2.bin", ios::binary);
//	T = outTest2;
//	if (outFile2)
//		outFile2.write((char *)T, size * sizeof(Data));
//	outFile2.close();
//	//Reading an array of Struct
//	ifstream inFile2("test2.bin", ios::binary);
//	T = inTest2;
//	if (inFile2)
//		inFile2.read((char *)T, size * sizeof(Data));
//	inFile2.close();
//
//	//Display Data
//	cout << "Test1:\nc1 = " << int(inTest1.c1) << ", n1 = " << inTest1.n1 << ", n2 = " << inTest1.n2;
//	cout << "\n\nTest2:\n";
//	for (int i = 0; i < size; i++)
//	{
//		cout << "For i = " << i << ": c1 = " << int(inTest2[i].c1);
//		cout << ", n1 = " << inTest2[i].n1 << ", n2 = " << inTest2[i].n2 << endl;
//	}
//	cout << endl;
//
//	system("pause");
//	return 0;
//}










//3
#include <iostream>
#include <fstream>
#include <bitset>

using namespace std;

struct Data
{
	unsigned long long int TOA = 0;
	unsigned short int PW;
	unsigned short int FMR_Frequency;
	unsigned char FMR_Antenna_a;
	bool FMR_Valid;
	unsigned char FMR_Antenna_A;
	unsigned char FMR_Antenna_B;
	unsigned char FMR_Antenna_C;
	unsigned char FMR_Antenna_D;
	unsigned short int Freq_Band;
	unsigned short int FEED;
};

int main()
{
	const int size = 20;
	unsigned char dataPacket[size];
	unsigned char *seek;
	Data *test = new Data;

	ifstream inFile("filtered.bin", ios::binary);
	if (inFile)
		inFile.read((char *)dataPacket, size * sizeof(char));
	inFile.close();

	//test->TOA = dataPacket[4];
	//test->TOA |= dataPacket[5] << 8;
	//test->TOA |= dataPacket[6] << 16;
	//test->TOA |= (unsigned long long int)(dataPacket[7]) << 24;
	//test->TOA |= (unsigned long long int)(dataPacket[8]) << 32;
	//test->TOA |= (unsigned long long int)(0x1f & dataPacket[9]) << 43;
	test->TOA = (unsigned long long int)(0x1f & dataPacket[9]) << 43 | (unsigned long long int)(dataPacket[8]) << 32 | (unsigned long long int)(dataPacket[7]) << 24 | dataPacket[6] << 16 | dataPacket[5] << 8 | dataPacket[4];

	cout << "TOA = " << hex << test->TOA << endl;
	cout << "Expected Output: 00 12 af 5f 3f 7c\n";
	for (int i = 0; i < size; i++)
	{
		cout << hex << (int)dataPacket[i] << " ";
	}

	cout << endl;
	system("pause");
	return 0;
}

