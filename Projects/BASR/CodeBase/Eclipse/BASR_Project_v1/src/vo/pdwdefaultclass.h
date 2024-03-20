/*
 * PdwDefault.h
 *
 *  Created on: Oct 15, 2018
 *      Author: test
 */

#ifndef PDWDEFAULTCLASS_H_
#define PDWDEFAULTCLASS_H_

#include "../common/common.h"
#include "../vo/trackintrapulseclass.h"
enum class BandCodeEnum : uint8_t {band_uncomputed , band_cd, band_e, band_g,
	band_i, band_j, band_k, band_ej, band_eg, band_ij};

struct PdwSmall32 {
	uint8_t ANT; // antenna of the pulse
	uint8_t POP; // pulse on pulse flag
	uint8_t LPI;
	uint8_t LPI_valid;
	uint8_t Blanking;
	uint8_t ST; // self test flag
	uint8_t Filter_Applied;
	uint8_t PoP_Info;

	uint16_t RF, PW;
	uint8_t RF_V;
	uint8_t OV;
	uint8_t BC;
	uint8_t N;

	uint64_t TOA;
	uint16_t Pa, Pb, Pc, Pd;

	uint64_t getTOA();
	static bool toaComparison(const PdwSmall32 &a,const PdwSmall32 &b);
};

struct PdwSmall {
	uint64_t timeOfArrivalStartRaw;
	uint16_t pulseWidth;
	uint16_t frequency;
	uint8_t powerA;
	uint8_t powerB;
	uint8_t powerC;
	uint8_t powerD;
	uint8_t band;
	uint8_t fmrAnt;
	uint8_t freqValid;
};
class PdwDefaultClass {
public:
	//do we need an ambiguity flag here if each pdw is sunk into multiple tracks?
	bool powerThresholdFlag;
	bool pulseTrackedFlag;
	bool pulseSentForClusteringFlag;
	bool trackProcessFlag;
	bool clusterProcessFlag;

	uint8_t trackProcessCount; //number of times pulse has been tracked ( not necessary and counter intuitive)
	uint8_t clusterProcessCount; //number of times the pulse has been clustered

	int16_t trackingPriScore;
	int16_t trackingRfScore;
	int16_t trackingPwScore;
	int16_t trackingPowerScore;

	BandCodeEnum band;
	IntraPulseType intraPulse;
	uint8_t selfTestFlag;
	uint8_t frequencyValidFlag;
	uint8_t popFlag;
	uint8_t quadrantCode;
	uint8_t antennaCode;
	uint8_t dominantAntennaCode;
	uint8_t secondaryAntennaCode;
	uint8_t pxAntenna;
	uint8_t pyAntenna;
	uint16_t pulseWidth;
	uint16_t angleOfArrival;
	uint16_t powerA;
	uint16_t powerB;
	uint16_t powerC;
	uint16_t powerD;
	uint16_t powerX;
	uint16_t powerY;
	uint16_t powerAmplitude;
	uint16_t frequency;
	uint64_t timeOfArrivalStart;
	uint64_t timeOfArrivalEnd;

	uint8_t bandRaw;
	uint8_t frequencyValidFlagRaw;
	uint8_t popFlagRaw;
	uint8_t quadrantCodeRaw;
	uint8_t antennaCodeRaw;
	uint16_t rfAmplitude;
	uint16_t pulseWidthRaw;
	uint16_t angleOfArrivalRaw;
	uint16_t powerARaw;
	uint16_t powerBRaw;
	uint16_t powerCRaw;
	uint16_t powerDRaw;
	uint16_t powerXRaw;
	uint16_t powerYRaw;
	uint16_t powerAmplitudeRaw;
	uint16_t frequencyRaw;
	uint64_t timeOfArrivalStartRaw;
	uint64_t timeOfArrivalEndRaw;

	static bool cmpToaStart(const PdwDefaultClass &a, const PdwDefaultClass &b);

	PdwDefaultClass();
	PdwDefaultClass(uint8_t bandRaw, uint8_t frequencyValidFlagRaw,
			uint8_t popFlagRaw, uint8_t quadrantCodeRaw, uint8_t antennaCodeRaw,
			uint16_t pulseWidthRaw, uint16_t angleOfArrivalRaw,
			uint16_t powerARaw, uint16_t powerBRaw, uint16_t powerCRaw,
			uint16_t powerDRaw, uint16_t powerXRaw, uint16_t powerYRaw,
			uint16_t powerAmplitudeRaw, uint16_t frequencyRaw,
			uint64_t timeOfArrivalStartRaw, uint64_t timeOfArrivalEndRaw);
	void ParseFromPdwSmall(PdwSmall &smallPdw);
	void ParseFromPdwSmall32(PdwSmall32 &smallPdw32);
	~PdwDefaultClass();
};

#endif /* PDWDEFAULTCLASS_H_ */
