/*
 * xadctemperaturehigh.cpp
 *
 *  Created on: Feb 9, 2021
 *      Author: saad
 */

#include "xadctemperaturehigh.h"

//This function initializes XADC temperature reading protocol
UWord8 XadcTemperatureHigh::initialize()
{
	UWord32 regValue = 0;
	UWord8 Status;
	MemoryMapAddressClass::xadcTemperatureReg.initialize(
			(UWord8*) MemoryMapAddressClass::XADC_TEMPERATURE_REGISTER);
	XadcTemperatureHigh::sendWord(XadcTemperatureHigh::XADCPS_UNLK_OFFSET,
			XadcTemperatureHigh::XADCPS_UNLK_VALUE);
	regValue = XadcTemperatureHigh::recvWord(
			XadcTemperatureHigh::XADCPS_CFG_OFFSET);
	regValue = regValue | XADCPS_CFG_ENABLE_MASK | XADCPS_CFG_CFIFOTH_MASK
			| XADCPS_CFG_DFIFOTH_MASK;
	XadcTemperatureHigh::sendWord(XadcTemperatureHigh::XADCPS_CFG_OFFSET,
			regValue);
	XadcTemperatureHigh::sendWord(XadcTemperatureHigh::XADCPS_MCTL_OFFSET, 0);

	/*
	 * Self Test the XADC/ADC device
	 */
	Status = XAdcPs_SelfTest(XAdcInstPtr);
	if (Status != 1)
	{
		return 0;
	}
	/*
	 * Disable the Channel Sequencer before configuring the Sequence
	 * registers.
	 */
	XAdcPs_SetSequencerMode(XAdcInstPtr, XADCPS_SEQ_MODE_SAFE);
	printf("Initializing xADC Done.\r\n");

	return 1;
}

//This function will clean up internal data structures
void XadcTemperatureHigh::clean()
{
//	memset(data, 0, 10000 * sizeof(UWord32));
}

//This function sends the word to address offset on XADC link
void XadcTemperatureHigh::sendWord(UWord32 address, UWord32 word)
{
	MemoryMapAddressClass::xadcTemperatureReg.writeDataToReg(address, word);
}

//This function receives the data from address offset on XADC link
UWord32 XadcTemperatureHigh::recvWord(UWord32 address)
{
	UWord32 val = MemoryMapAddressClass::xadcTemperatureReg.readDataFromReg(
			address);
	return val;
}

//This function returns whether temperature is greater/less than 70 degree C
UWord8 XadcTemperatureHigh::checkXadcTemperature()
{
	return xadcGetTemperature(XAdcInstPtr);
}

//This is the main logic of temperature reading and indication of it going
//above 70 degree C. It first reads the temperature in ADC counts, converts
//the ADC counts to degree C and applies check on the temperature
UWord8 XadcTemperatureHigh::xadcGetTemperature(XAdcPs *XAdcInstPtr)
{
	UWord32 TempRawData;
	F32 TempData;

//    printf("\nFetching Temperature and System VCC.\r\n");

	/*
	 * Read the on-chip Temperature Data (Current/Maximum/Minimum)
	 * from the ADC data registers.
	 */
	TempRawData = XAdcPs_GetAdcData(XAdcInstPtr, XADCPS_CH_TEMP);
	TempData = XAdcPs_RawToTemperature(TempRawData);

//    printf("The Current Temperature is %0d.%03d Centigrades.\r\n", (UWord32)(TempData), XAdcFractionToInt(TempData));

	if ((UWord32) (TempData) > 70)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

//This function converts ADC count to degree C
F32 XadcTemperatureHigh::XAdcPs_RawToTemperature(UWord32 AdcData)
{
	F32 temperature = 0;
	temperature = ((((float) (AdcData) / 65536.0f) / 0.00198421639f) - 273.15f);
	return temperature;
}

//This function reads Temperature in ADC counts
F32 XadcTemperatureHigh::XAdcPs_GetAdcData(XAdcPs *InstancePtr, UWord8 Channel)
{

	UWord32 RegData;

	RegData = XAdcPs_ReadInternalReg(InstancePtr,
			(XADCPS_TEMP_OFFSET + Channel));
	return (F32) RegData;
}

//This function reads data from RegOffset
UWord32 XadcTemperatureHigh::XAdcPs_ReadInternalReg(XAdcPs *InstancePtr,
		UWord32 RegOffset)
{

	UWord32 RegData;

	RegData = XAdcPs_FormatWriteData(RegOffset, 0x0, 0);

	/* Read cmd to FIFO*/
	XadcTemperatureHigh::sendWord(XADCPS_CMDFIFO_OFFSET, RegData);
//	XAdcPs_WriteFifo(InstancePtr, RegData);

	/* Do a Dummy read */
	RegData = XadcTemperatureHigh::recvWord(XADCPS_RDFIFO_OFFSET);
//	RegData = XAdcPs_ReadFifo(InstancePtr);

	/* Do a Dummy write to get the actual read */
	XadcTemperatureHigh::sendWord(XADCPS_CMDFIFO_OFFSET, RegData);
//	XAdcPs_WriteFifo(InstancePtr, RegData);

	/* Do the Actual read */
	RegData = XadcTemperatureHigh::recvWord(XADCPS_RDFIFO_OFFSET);
//	RegData = XAdcPs_ReadFifo(InstancePtr);

	return RegData;

}

//This function is formatting data to be sent to XADC
UWord32 XadcTemperatureHigh::XAdcPs_FormatWriteData(UWord32 RegOffset,
		UWord32 Data, UWord8 ReadWrite)
{
	UWord32 returnData = 0;
	if (ReadWrite == 1)
	{
		returnData =
				(XADCPS_JTAG_CMD_WRITE_MASK)
						| ((RegOffset << XADCPS_JTAG_ADDR_SHIFT)
								& XADCPS_JTAG_ADDR_MASK)
						| (Data & XADCPS_JTAG_DATA_MASK);
	}
	else
	{
		returnData =
				(XADCPS_JTAG_CMD_READ_MASK)
						| ((RegOffset << XADCPS_JTAG_ADDR_SHIFT)
								& XADCPS_JTAG_ADDR_MASK)
						| (Data & XADCPS_JTAG_DATA_MASK);
	}
	return returnData;
}

//This function is internal self test of XADC, in this, it sets the
//alarm threshold and reads it back, if data matches, test passes
UWord8 XadcTemperatureHigh::XAdcPs_SelfTest(XAdcPs *InstancePtr)
{
	UWord32 RegValue;

	/*
	 * Reset the device to get it back to its default state
	 */
	XAdcPs_Reset(InstancePtr);

	/*
	 * Write a value into the Alarm Threshold registers, read it back, and
	 * do the comparison
	 */
	XAdcPs_SetAlarmThreshold(InstancePtr, XADCPS_ATR_VCCINT_UPPER,
			XADCPS_ATR_TEST_VALUE);
	RegValue = XAdcPs_GetAlarmThreshold(InstancePtr, XADCPS_ATR_VCCINT_UPPER);

	if (RegValue == XADCPS_ATR_TEST_VALUE)
	{
		Status = 1;
	}
	else
	{
		Status = 0;
	}

	/*
	 * Reset the device again to its default state.
	 */
	XAdcPs_Reset(InstancePtr);
	/*
	 * Return the test result.
	 */
	return Status;
}

//This function resets the XADC protocol
void XadcTemperatureHigh::XAdcPs_Reset(XAdcPs *InstancePtr)
{
	/*
	 * Generate the reset by Control
	 * register and release from reset
	 */

	sendWord(XADCPS_MCTL_OFFSET, 0x10);
	sendWord(XADCPS_MCTL_OFFSET, 0x00);
//	XAdcPs_WriteReg((InstancePtr)->Config.BaseAddress, XADCPS_MCTL_OFFSET, 0x10);
//	XAdcPs_WriteReg((InstancePtr)->Config.BaseAddress, XADCPS_MCTL_OFFSET, 0x00);
}

//This function sets Alarm Threshold value on its offset
void XadcTemperatureHigh::XAdcPs_SetAlarmThreshold(XAdcPs *InstancePtr,
		UWord8 AlarmThrReg, UWord16 Value)
{
	/*
	 * Write the value into the specified Alarm Threshold Register.
	 */
	XAdcPs_WriteInternalReg(InstancePtr,
			XADCPS_ATR_TEMP_UPPER_OFFSET + AlarmThrReg, Value);

}

//This function writes data to Register offset
void XadcTemperatureHigh::XAdcPs_WriteInternalReg(XAdcPs *InstancePtr,
		UWord32 RegOffset, UWord32 Data)
{
	UWord32 RegData;

	/*
	 * Write the Data into the FIFO Register.
	 */
	RegData = XAdcPs_FormatWriteData(RegOffset, Data, TRUE);

	sendWord(XADCPS_CMDFIFO_OFFSET, RegData);
//	XAdcPs_WriteFifo(InstancePtr, RegData);

	/* Read the Read FIFO after any write since for each write
	 * one location of Read FIFO gets updated
	 */
	recvWord(XADCPS_RDFIFO_OFFSET);
//	XAdcPs_ReadFifo(InstancePtr);

}

//This function gets Alarm Threshold value from its offset
UWord16 XadcTemperatureHigh::XAdcPs_GetAlarmThreshold(XAdcPs *InstancePtr,
		UWord8 AlarmThrReg)
{
	UWord32 RegData;

	/*
	 * Read the specified Alarm Threshold Register and return
	 * the value
	 */
	RegData = XAdcPs_ReadInternalReg(InstancePtr,
			(XADCPS_ATR_TEMP_UPPER_OFFSET + AlarmThrReg));

	return (UWord16) RegData;
}

//This function sets XADC sequencer mode in its configuration register
void XadcTemperatureHigh::XAdcPs_SetSequencerMode(XAdcPs *InstancePtr,
		UWord8 SequencerMode)
{
	UWord32 RegValue;

	/*
	 * Set the specified sequencer mode in the Configuration Register 1.
	 */
	RegValue = XAdcPs_ReadInternalReg(InstancePtr, XADCPS_CFR1_OFFSET);
	RegValue &= (~XADCPS_CFR1_SEQ_VALID_MASK);
	RegValue |= ((SequencerMode << XADCPS_CFR1_SEQ_SHIFT)
			& XADCPS_CFR1_SEQ_VALID_MASK);
	XAdcPs_WriteInternalReg(InstancePtr, XADCPS_CFR1_OFFSET, RegValue);
}

