/*
 * cfduartlink.h
 *
 *  Created on: Mar 18, 2021
 *      Author: zumair
 */

#ifndef IO_CFDUARTLINK_H_
#define IO_CFDUARTLINK_H_

#include "../common/common.h"
#include "../io/memorymap.h"
#include "../utility/consthigh.h"

class CfdUartLink {

	static const unsigned int CFD_UART_CONTROL = 0x00000104;
	static const unsigned int CFD_UART_STATUS = 0x00000100;
	static const unsigned int CFD_UART_TX_OFFSET = 0x00000000;
	static const unsigned int CFD_UART_RX_OFFSET = 0x00000080;

	static const unsigned int CFD_TX_ENABLE = 0x00000001;
	static const unsigned int CFD_RX_DONE = 0x00000002;

	static const unsigned int CFD_RX_FIFO_EMPTY = 0x00000001;
	static const unsigned int CFD_RX_FIFO_FULL = 0x00000002;
	static const unsigned int CFD_TX_FIFO_EMPTY = 0x00000004;
	static const unsigned int CFD_TX_FIFO_FULL = 0x00000008;
	static const unsigned int CFD_VALID_DATA_PRESENT = 0x00000010;
	static const unsigned int CFD_UART_RX_BUSY = 0x00000020;

	static const unsigned int CFD_UART_BUFFER_SIZE = 32;

	static const unsigned int CRC_TSIZE = 256;
	static const unsigned int CRC_BUFSIZE = 32768U;

	static const unsigned int CRC16_POLYNOMIAL = 0xA001U;
	static const unsigned int CRC16_INIT = 0x0000U;
	static const unsigned int CRC16_FINAL = 0x0000U;

	static const unsigned int CRC32_POLYNOMIAL = 0xEDB88320UL;
	static const unsigned int CRC32_INIT = 0xFFFFFFFFUL;
	static const unsigned int CRC32_FINAL = 0xFFFFFFFFUL;

private:
	unsigned int msgCrc;
	unsigned int wCrcTable[CRC_TSIZE];

	void sendWordUart(UWord32 address, UWord32 word);
	UWord32 recWordUart(UWord32 address);

public:
	uint32_t cfdDataBuffer[CFD_UART_BUFFER_SIZE];
	uint8_t cfdDataRxBuffer[CFD_UART_BUFFER_SIZE];

	void initializeCfdUart();
	void calculateParity(UWord32 data, UWord8 iterator);
	void clearRxRegisters();
	void sendCfdData(UWord32 nBytes);
	void readCfdData();
	void initCrc();
	void preCrc();
	void updateCrc(unsigned char *buf, unsigned int nCount);
	uint16_t getCrc();
};

#endif /* IO_CFDUARTLINK_H_ */
