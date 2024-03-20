/*
 * xadctemperaturehigh.h
 *
 *  Created on: Feb 9, 2021
 *      Author: saad
 */

#ifndef XADCTEMPERATUREHIGH_H_
#define XADCTEMPERATUREHIGH_H_

#include "../common/common.h"
#include "memorymap.h"

/**
 * This typedef contains configuration information for the XADC/ADC
 * device.
 */
typedef struct {
	UWord16 DeviceId; /**< Unique ID of device */
	UWord32 BaseAddress; /**< Device base address */
} XAdcPs_Config;

/**
 * The driver's instance data. The user is required to allocate a variable
 * of this type for every XADC/ADC device in the system. A pointer to
 * a variable of this type is then passed to the driver API functions.
 */
typedef struct {
	XAdcPs_Config Config; /**< XAdcPs_Config of current device */
	UWord32 IsReady; /**< Device is initialized and ready  */
} XAdcPs;

class XadcTemperatureHigh {
	static const unsigned int XADCPS_UNLK_OFFSET = 0x034;/**< Unlock Register */
	static const unsigned int XADCPS_UNLK_VALUE = 0x757BDF0D;/**< Unlock Value */
	static const unsigned int XADCPS_CFG_OFFSET = 0x00; /**< Configuration Register */
	static const unsigned int XADCPS_CFG_ENABLE_MASK = 0x80000000; /**< Enable access from PS mask */
	static const unsigned int XADCPS_CFG_CFIFOTH_MASK = 0x00F00000; /**< Command FIFO Threshold mask */
	static const unsigned int XADCPS_CFG_DFIFOTH_MASK = 0x000F0000;/**< Data FIFO Threshold mask */
	static const unsigned int XADCPS_MCTL_OFFSET = 0x18;
	static const unsigned int XADCPS_CH_TEMP = 0x0;/**< On Chip Temperature */
	static const unsigned int XADCPS_TEMP_OFFSET = 0x00; /**< On-chip Temperature Reg */
	static const unsigned int XADCPS_JTAG_DATA_MASK = 0x0000FFFF; /**< Mask for the Data */
	static const unsigned int XADCPS_JTAG_ADDR_MASK = 0x03FF0000; /**< Mask for the Addr */
	static const unsigned int XADCPS_JTAG_ADDR_SHIFT = 16; /**< Shift for the Addr */
	static const unsigned int XADCPS_JTAG_CMD_MASK = 0x3C000000; /**< Mask for the Cmd */
	static const unsigned int XADCPS_JTAG_CMD_WRITE_MASK = 0x08000000; /**< Mask for CMD Write */
	static const unsigned int XADCPS_JTAG_CMD_READ_MASK = 0x04000000;/**< Mask for CMD Read */
	static const unsigned int XADCPS_JTAG_CMD_SHIFT = 26; /**< Shift for the Cmd */
	static const unsigned int XADCPS_CMDFIFO_OFFSET = 0x10;/**< Command FIFO Register */
	static const unsigned int XADCPS_RDFIFO_OFFSET = 0x14; /**< Read FIFO Register */
	static const unsigned int XADCPS_ATR_TEMP_UPPER_OFFSET = 0x50; /**< Temp Upper Alarm Register */
	static const unsigned int XADCPS_ATR_TEST_VALUE = 0x55;
	static const unsigned int XADCPS_ATR_VCCINT_UPPER = 1; /**< VCCINT high voltage limit register */
	static const unsigned int XADCPS_CFR1_OFFSET = 0x41; /**< Configuration Register 1 */
	static const unsigned int XADCPS_CFR1_SEQ_VALID_MASK = 0xF000;/**< Sequence bit Mask */
	static const unsigned int XADCPS_CFR1_SEQ_SHIFT = 12; /**< Sequence bit shift */
	static const unsigned int XADCPS_SEQ_MODE_SAFE = 0; /**< Default Safe Mode */
private:
	UWord8 Status;
	XAdcPs XAdcInst; /* XADC driver instance */
	XAdcPs *XAdcInstPtr = &XAdcInst;

	/*
	 * Function Name:   clean
	 * Purpose:			This function will clean up internal data structures
	 * Inputs:          none
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void clean();

	/*
	 * Function Name:   sendWord
	 * Purpose:			This function sends the word to address offset on XADC link
	 * Inputs:          address
	 * 					word
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void sendWord(UWord32 address, UWord32 word);

	/*
	 * Function Name:   recvWord
	 * Purpose:			This function receives the data from address offset on XADC link
	 * Inputs:          address
	 * Outputs:         word
	 * Scope:           Can be called by any function
	 */
	UWord32 recvWord(UWord32 address);

	/*
	 * Function Name:   xadcGetTemperature
	 * Purpose:			This is the main logic of temperature reading and indication of it going
	 * 					above 70 degree C. It first reads the temperature in ADC counts, converts
	 * 					the ADC counts to degree C and applies check on the temperature
	 * Inputs:          Pointer to XADC
	 * Outputs:         Flag to indicate temperature greater than 70 degree
	 * Scope:           Can be called by any function
	 */
	UWord8 xadcGetTemperature(XAdcPs *XAdcInstPtr);

	/*
	 * Function Name:   XAdcPs_RawToTemperature
	 * Purpose:			This function converts ADC count to degree C
	 * Inputs:          ADC count
	 * Outputs:         Temperature in degree C
	 * Scope:           Can be called by any function
	 */
	F32 XAdcPs_RawToTemperature(UWord32 AdcData);

	/*
	 * Function Name:   XAdcPs_GetAdcData
	 * Purpose:			This function reads Temperature in ADC counts
	 * Inputs:			Pointer to XADC and channel of Temperature
	 * Outputs:         Temperature in ADC count
	 * Scope:           Can be called by any function
	 */
	F32 XAdcPs_GetAdcData(XAdcPs *InstancePtr, UWord8 Channel);

	/*
	 * Function Name:   XAdcPs_ReadInternalReg
	 * Purpose:			This function reads data from RegOffset
	 * Inputs:			Pointer to XADC and offset of data
	 * Outputs:         Data
	 * Scope:           Can be called by any function
	 */
	UWord32 XAdcPs_ReadInternalReg(XAdcPs *InstancePtr, UWord32 RegOffset);

	/*
	 * Function Name:   XAdcPs_FormatWriteData
	 * Purpose:			This function is formatting data to be sent to XADC
	 * Inputs:			Reg Offset
	 * 					Data
	 * 					ReadWrite flag
	 * Outputs:         Formatted Data
	 * Scope:           Can be called by any function
	 */
	UWord32 XAdcPs_FormatWriteData(UWord32 RegOffset, UWord32 Data,
			UWord8 ReadWrite);

	/*
	 * Function Name:   XAdcPs_SelfTest
	 * Purpose:			This function is internal self test of XADC, in this, it sets the
	 * 					alarm threshold and reads it back, if data matches, test passes
	 * Inputs:			Pointer to XADC and offset of data
	 * Outputs:         Result of self test
	 * Scope:           Can be called by any function
	 */
	UWord8 XAdcPs_SelfTest(XAdcPs *InstancePtr);

	/*
	 * Function Name:   XAdcPs_Reset
	 * Purpose:			This function resets the XADC protocol
	 * Inputs:			Pointer to XADC and offset of data
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void XAdcPs_Reset(XAdcPs *InstancePtr);

	/*
	 * Function Name:   XAdcPs_SetAlarmThreshold
	 * Purpose:			This function sets Alarm Threshold value on its offset
	 * Inputs:			Pointer to XADC and offset of data
	 * 					Threshold offset
	 * 					Value
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void XAdcPs_SetAlarmThreshold(XAdcPs *InstancePtr, UWord8 AlarmThrReg,
			UWord16 Value);

	/*
	 * Function Name:   XAdcPs_WriteInternalReg
	 * Purpose:			This function writes data to Register offset
	 * Inputs:			Pointer to XADC and offset of data
	 * 					Register offset
	 * 					Data
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void XAdcPs_WriteInternalReg(XAdcPs *InstancePtr, UWord32 RegOffset,
			UWord32 Data);

	/*
	 * Function Name:   XAdcPs_GetAlarmThreshold
	 * Purpose:			This function gets Alarm Threshold value from its offset
	 * Inputs:			Pointer to XADC and offset of data
	 * 					Threshold offset
	 * Outputs:         Value
	 * Scope:           Can be called by any function
	 */
	UWord16 XAdcPs_GetAlarmThreshold(XAdcPs *InstancePtr, UWord8 AlarmThrReg);

	/*
	 * Function Name:   XAdcPs_SetSequencerMode
	 * Purpose:			This function sets XADC sequencer mode in its configuration register
	 * Inputs:			Pointer to XADC and offset of data
	 * 					Sequencer Mode
	 * Outputs:         none
	 * Scope:           Can be called by any function
	 */
	void XAdcPs_SetSequencerMode(XAdcPs *InstancePtr, UWord8 SequencerMode);
public:

	/*
	 * Function Name:   initialize
	 * Purpose:			This function initializes XADC temperature reading protocol
	 * Inputs:          none
	 * Outputs:         Status of initialization
	 * Scope:           Can be called by any function
	 */
	UWord8 initialize();

	/*
	 * Function Name:   checkXadcTemperature
	 * Purpose:			This function returns whether temperature is greater/less than 70 degree C
	 * Inputs:          none
	 * Outputs:         Flag to indicate temperature greater than 70 degree
	 * Scope:           Can be called by any function
	 */
	UWord8 checkXadcTemperature();
};

#endif /* XADCTEMPERATUREHIGH_H_ */
