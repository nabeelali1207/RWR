/*
 * cfduartlink.cpp
 *
 *  Created on: Mar 18, 2021
 *      Author: zumair
 */

#include "../io/cfduartlink.h"

void CfdUartLink::initializeCfdUart()
{
	MemoryMapAddressClass::cfdUartReg.initialize(
			(UWord8*) MemoryMapAddressClass::CFD_UART_BASE_REGISTER);

	initCrc();
}

void CfdUartLink::sendWordUart(UWord32 address, UWord32 word)
{
	MemoryMapAddressClass::cfdUartReg.writeDataToReg(address, word);
}

UWord32 CfdUartLink::recWordUart(UWord32 address)
{
	UWord32 val = MemoryMapAddressClass::cfdUartReg.readDataFromReg(address);
	return val;
}

void CfdUartLink::calculateParity(UWord32 data, UWord8 iterator)
{
	bool parity = false;
	UWord32 val = 1;
	UWord32 mask = 0x01;

	for (int i = 0; i < 8; i++)
	{
		if ((data & (mask << i)))
		{
			parity = !parity;
		}
	}

	if (parity)
	{
		data |= (val << 8);
	}

	CfdUartLink::sendWordUart(CFD_UART_TX_OFFSET + (iterator * 4), data);
}

void CfdUartLink::clearRxRegisters()
{
	for (int i = 0; i < 32; i++)
	{
		CfdUartLink::sendWordUart(CFD_UART_RX_OFFSET + (i * 4), 0x00000000);
	}
}

void CfdUartLink::sendCfdData(UWord32 nBytes)
{
	UWord32 controlReg = 0;
	UWord16 i = 0;

	if (nBytes > 31)
	{
		return;
	}

	for (i = 0; i < CFD_UART_BUFFER_SIZE; i++)
	{
		if (i >= nBytes)
		{
			break;
		}

		CfdUartLink::calculateParity(cfdDataBuffer[i], i);
	}
	memset(cfdDataBuffer, 0, sizeof(UWord8) * CFD_UART_BUFFER_SIZE);

	CfdUartLink::sendWordUart(CFD_UART_CONTROL, controlReg);
	controlReg = CfdUartLink::recWordUart(CFD_UART_CONTROL);
	controlReg |= nBytes << 8;
	controlReg |= CFD_TX_ENABLE;
	CfdUartLink::sendWordUart(CFD_UART_CONTROL, controlReg);
}

void CfdUartLink::readCfdData()
{
	UWord32 statusReg = 0, controlReg = 0, counter = 0;
	UWord16 i = 0;
	memset(cfdDataRxBuffer, 0, sizeof(UWord8) * CFD_UART_BUFFER_SIZE);

	statusReg = CfdUartLink::recWordUart(CFD_UART_STATUS);
//	if (!(statusReg & CFD_RX_FIFO_EMPTY))
	{
		while (!(statusReg & CFD_VALID_DATA_PRESENT))
		{
			statusReg = CfdUartLink::recWordUart(CFD_UART_STATUS);
			counter++;

			if (counter >= 100000)
			{
				return;
			}
		}

//		printf("\n");
		for (i = 0; i < CFD_UART_BUFFER_SIZE; i++)
		{
			cfdDataRxBuffer[i] = CfdUartLink::recWordUart(
					CFD_UART_RX_OFFSET + (i * 4));

//			printf(" %.2x ", cfdDataRxBuffer[i]);
		}
//		printf("\n");

		CfdUartLink::sendWordUart(CFD_UART_CONTROL, controlReg);
		controlReg = CfdUartLink::recWordUart(CFD_UART_CONTROL);
		controlReg |= CFD_RX_DONE;
		CfdUartLink::sendWordUart(CFD_UART_CONTROL, controlReg);
	}
}

void CfdUartLink::initCrc()
{
	uint16_t wIndex, wSize, wCrc;

	for (wIndex = 0; wIndex < CRC_TSIZE; wIndex++)
	{
		wCrc = wIndex;
		for (wSize = 0; wSize < 8; wSize++)
		{
			if (wCrc & 1)
			{
				wCrc >>= 1;
				wCrc ^= CRC16_POLYNOMIAL;
			}
			else
			{
				wCrc >>= 1;
			}

			wCrcTable[wIndex] = wCrc;
		}
	}
}

void CfdUartLink::preCrc()
{
	msgCrc = CRC16_INIT;
}

void CfdUartLink::updateCrc(unsigned char *buf, unsigned int nCount)
{
//	nCount = 3;
//	buf[0] = 0x26;
//	buf[1] = 0x03;
//	buf[2] = 0x2F;

//	nCount = 6;
//	buf[0] = 0x02;
//	buf[1] = 0x06;
//	buf[2] = 0x83;
//	buf[3] = 0x01;
//	buf[4] = 0x01;
//	buf[5] = 0x01;

	if (nCount == 0)
	{
		return;
	}

	do
	{
		msgCrc = (msgCrc >> 8) ^ wCrcTable[*buf ^ (unsigned char) msgCrc];
		buf += 4;
	} while (--nCount > 0);
}

uint16_t CfdUartLink::getCrc()
{
	uint16_t temp = 0;
	msgCrc ^= CRC16_INIT;
	temp = msgCrc;
	msgCrc = (temp << 8) | (temp >> 8);

	return msgCrc;
}
