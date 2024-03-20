/*
 * memorymap.cpp
 *
 *  Created on: Oct 16, 2019
 *      Author: gmd
 */
#define MEMORYMAP_CPP_

#include "../io/memorymap.h"

bool MemoryMapAccessClass::isMappingValid()
{
	return isValid;
}

MemoryMapAccessClass::MemoryMapAccessClass()
{
	physAddress = NULL;
	virtAddress = NULL;
	virtAddressKernel = NULL;
	mmapAddress = NULL;
	fileDescriptor = NULL;
	mmapSize = 0;
	isValid = false;
}

MemoryMapAccessClass::~MemoryMapAccessClass()
{
	if (isValid == false)
		return;
	munmap((void *) mmapAddress, mmapSize);
}

void MemoryMapAccessClass::initializeFileDescriptor(const char * procName)
{
	fileDescriptor = fopen(procName, "w");
	setvbuf(fileDescriptor,NULL,_IONBF,0);
}
void MemoryMapAccessClass::openThenWriteThenClose(const char * procName,const char* buffer)
{
	initializeFileDescriptor(procName);
	writeToFile(buffer);
	fclose(fileDescriptor);
}
void MemoryMapAccessClass::initializeKernelMemory(UWord8 readWrite, UWord32 memSize, const char * procName)
{
	physAddress = NULL;
	mmapAddress = NULL;
	isValid = false;

	int protStatus = readWrite;
	int rwStatus = 0;
	if(protStatus == 0)
	{
		protStatus = PROT_READ;
		rwStatus = O_RDONLY;
	}
	else if(protStatus == 1)
	{
		protStatus = PROT_WRITE;
		rwStatus = O_WRONLY;
	}
	else if(protStatus == 2)
	{
		protStatus = PROT_READ | PROT_WRITE;
		rwStatus = O_RDWR;
	}

	int file = open(procName, rwStatus);
	if (file < 0)
	{
		cout << "Memory Mapping Failed 1 for " << procName << endl;
		return;
	}
	cout << "Kernel Proc opened: " << procName << "\n";
	virtAddressKernel = (unsigned long long *) malloc(mmapSize);
	mmapSize = memSize;
	virtAddressKernel = (unsigned long long *) mmap(NULL, mmapSize, protStatus, MAP_SHARED, file, 0);
	if (virtAddressKernel == (void*) -1)
	{
		cout << "Memory Mapping Failed 2 for " << procName << endl;
		return;
	}
	isValid = true;
	close(file);
}

void MemoryMapAccessClass::initialize(UWord8 * _physAddress, UWord32 memSize)
{
	physAddress = _physAddress;
	mmapAddress = NULL;
	isValid = false;

	int file = open("/dev/mem", O_RDWR | O_SYNC);
	if (file < 0)
	{
		cout << "Memory Mapping Failed 1" << endl;
		return;
	}
	UWord64 kernelPageSize = sysconf(_SC_PAGESIZE);
	UWord64 devicePageAddress = ((UWord64) _physAddress
			& (~(kernelPageSize - 1)));
	UWord64 devicePageOffset = ((UWord64) _physAddress & (kernelPageSize - 1));
	UWord32 pageOffset = devicePageOffset & 0xFFFFFFFF;

	UWord64 numberOfPages = memSize / kernelPageSize;
	if (numberOfPages * kernelPageSize < memSize)
		numberOfPages++;
	mmapSize = numberOfPages * kernelPageSize;
	mmapAddress = (UWord8 *) mmap(NULL, mmapSize,
	PROT_READ | PROT_WRITE, MAP_SHARED, file, devicePageAddress);
	if (mmapAddress == (void*) -1)
	{
		cout << "Memory Mapping Failed 2" << endl;
		return;
	}
	isValid = true;
	virtAddress = (volatile UWord8 *) (mmapAddress + pageOffset);

	cout << "Mapped Physical: " << std::hex << ((UWord64 *) physAddress)
			<< " To Virtual: " << std::hex << ((UWord64 *) virtAddress)
			<< " Size(hex): " << std::hex << mmapSize << std::dec << endl;
	close(file);
}

void MemoryMapAccessClass::writeDataToReg(UWord32 offset, UWord32 value)
{
	*(volatile UWord32*) (virtAddress + offset) = value;
}

void MemoryMapAccessClass::writeDataToReg(UWord32 offset, UWord32 value,
		UWord32 mask)
{
	UWord32 valueTemp = *(volatile UWord32*) (virtAddress + offset);
	valueTemp = (valueTemp & ~mask) | (value & mask);
	*(volatile UWord32*) (virtAddress + offset) = valueTemp;
}

UWord32 MemoryMapAccessClass::readDataFromReg(UWord32 offset)
{
	return *(volatile UWord32*) (virtAddress + offset);
}

void MemoryMapAccessClass::writeDataToRegKernel(UWord32 offset, UWord32 value)
{
	*(((volatile UWord32*) virtAddressKernel) + offset) = value;
}

void MemoryMapAccessClass::writeDataToRegKernel(UWord32 offset, UWord64 value)
{
	*(((volatile UWord64*) virtAddressKernel) + offset) = value;
}

void MemoryMapAccessClass::readDataFromRegKernel(UWord32 offset, UWord32& result)
{
	result = *(((volatile UWord32*) virtAddressKernel) + offset);
}

void MemoryMapAccessClass::readDataFromRegKernel(UWord32 offset, UWord64& result)
{
	result = *(((volatile UWord64*) virtAddressKernel) + offset);
}

void MemoryMapAccessClass::blockReadWordByWord_v1(UWord32 offset, UWord8 * buffer,
		UWord32 byteCount)
{
	UWord32 * tmpPtr = (UWord32 *) buffer;
	UWord32 i = 0, j = 0;
	while (i < byteCount)
	{
		tmpPtr[j] = readDataFromReg(offset + i);
		j = j + 1;
		i = i + 4;
	}
	if (byteCount != i)
	{
		cout << "alignment Error in read" << endl;
	}
}

void MemoryMapAccessClass::blockReadWordByWord_v2(UWord32 offset, UWord8 * buffer,
		UWord32 byteCount)
{
	UWord64 * tmpPtr = (UWord64 *) buffer;
	volatile UWord64 * address = (volatile UWord64 *) (virtAddress + offset);
	UWord32 i = 0;
	UWord32 wordCount = byteCount / sizeof(UWord64);
	for (; i < wordCount; i++)
	{
		tmpPtr[i] = address[i];
	}
	if (wordCount != i)
	{
		cout << "alignment Error in read" << endl;
	}
}

void MemoryMapAccessClass::blockReadWordByWord_v3(UWord32 offset, UWord8 * buffer,
		UWord32 byteCount)
{
	memcpy((void *) buffer, (void *) (virtAddress + offset),byteCount);
}

void MemoryMapAccessClass::blockWriteWordByWordKernel(UWord32 offset, UWord8 * buffer,
		UWord32 byteCount)
{
	memcpy(((char *) (virtAddressKernel) + offset), (void *) buffer, byteCount);
}
void MemoryMapAccessClass::blockReadWordByWordKernel(UWord32 offset, UWord8 * buffer,
		UWord32 byteCount)
{
	memcpy((void *) buffer, ((char *) (virtAddressKernel) + offset), byteCount);
}
void MemoryMapAccessClass::blockReadChunkByChunk(UWord32 offset,
		UWord8 * buffer, UWord32 byteCount)
{
	//		memcpy((void *) buffer, (void *) (mmapAddress + pageOffset + offset),
	//				readSize);

	UWord32 * tmpPtr = (UWord32 *) buffer;
	UWord32 * tmpMmapedAdd = (UWord32 *) (virtAddress + offset);
	UWord32 i = 0, j = byteCount / sizeof(UWord32);
	while (i < j)
	{
		tmpPtr[i] = *(tmpMmapedAdd + i);
		i++;
	}
	if ((j * sizeof(UWord32)) != byteCount)
	{
		cout << "alignment Error in read" << endl;
	}
}

void MemoryMapAccessClass::blockWriteWordByWord(UWord32 offset, UWord8 * buffer,
		UWord32 byteCount)
{
	UWord32 * tmpPtr = (UWord32 *) buffer;
	UWord32 i = 0, j = 0;
	while (i < byteCount)
	{
		writeDataToReg(offset + i, tmpPtr[j]);
		j = j + 1;
		i = i + 4;
	}
	if (byteCount != i)
	{
		cout << "alignment Error in write" << endl;
	}
}

void MemoryMapAccessClass::blockMemSetWordByWord(UWord32 offset, UWord32 val,
		UWord32 byteCount)
{
	UWord32 i = 0, j = 0;
	while (i < byteCount)
	{
		writeDataToReg(offset + i, val);
		j = j + 1;
		i = i + 4;
	}
	if (byteCount != i)
	{
		cout << "alignment Error in write" << endl;
	}
}

void MemoryMapAccessClass::writeToFile(const char* buffer)
{
	fputs(buffer, fileDescriptor);
	fflush(fileDescriptor);
}

volatile UWord8* MemoryMapAccessClass::getVirtualAddress()
{
	return virtAddress;
}

volatile UWord8* MemoryMapAccessClass::getVirtualAddressKernel()
{
	return (volatile UWord8*) virtAddressKernel;
}
