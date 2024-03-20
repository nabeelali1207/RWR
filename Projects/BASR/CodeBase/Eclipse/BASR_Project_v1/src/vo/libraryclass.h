/*
 * libraryclass.h
 *
 *  Created on: Nov 23, 2018
 *      Author: hassan
 */

#ifndef LIBRARYCLASS_H_
#define LIBRARYCLASS_H_

#include "../common/common.h"
#include "../utility/consthigh.h"

#define TL_PRI_TYPES_FIXED              1
#define TL_PRI_TYPES_DWELL              2
#define TL_PRI_TYPES_STAGGER            3
#define TL_PRI_TYPES_SLIDER             4
#define TL_PRI_TYPES_JITTER             5
#define TL_PRI_TYPES_COMPLEX            6
#define TL_PRI_TYPES_DIFFTOA            7

typedef struct GlobalScores
{
    uint8_t globalMatchPercentRequired;
    uint8_t priScore;
    uint8_t pwScore;
    uint8_t rfScore;
    uint8_t priTypeScore;
    uint8_t pwTypeScore;
    uint8_t rfTypeScore;
    uint8_t lpiScore;
    uint8_t priSequenceScore;
    uint8_t priCountSTTReq;
} GlobalScores;

typedef struct ThreatLibraryHeader
{
    uint16_t	LibraryID;	//identification number of library
    uint8_t     majorVersion[2];
    uint8_t     minorVersion[2];
    uint8_t     reserved;
    uint16_t	NumberOfThreats;//number of entries in the library
    uint8_t		Day;
    uint8_t		Month;
    uint8_t		Year;
    uint8_t		Second;
    uint8_t		Minute;
    uint8_t		Hour;
    uint8_t		Description[50];
    GlobalScores tlScores;
    uint8_t		padding[20];
} ThreatLibraryHeader;

typedef struct BandFilter
{
    uint16_t bandFilterLower;
    uint16_t bandFilterUpper;
}BandFilter;

typedef struct SequenceStructure
{
    uint8_t  band;
    uint32_t sMin;
    uint32_t sMax;
} SequenceStructure;

typedef struct RSS
{
    SequenceStructure s1Block[256];
    SequenceStructure s2Block[256];
}RSS;

typedef struct HwConfig
{
    uint8_t  thresholdLevel;
    int8_t   cdThreshold;
    int8_t   eThreshold;
    int8_t   gThreshold;
    int8_t   iThreshold;
    int8_t   jThreshold;
    int8_t   kThreshold;
    int8_t   cdCwThreshold;
    int8_t   eCwThreshold;
    int8_t   gCwThreshold;
    int8_t   iCwThreshold;
    int8_t   jCwThreshold;
} HwConfig;

typedef struct MDF
{
    uint8_t  	x;
    uint8_t  	y;
    uint8_t  	z;
    uint8_t  	k;
    uint8_t  	s1Reps;
    uint16_t 	numOfS1;
    uint16_t 	numOfS2;
    HwConfig	hwConfig;
}MDF;

typedef struct FuncConfig
{
    uint16_t 	displayTimeout;
    uint8_t  	recState;
    uint8_t  	audioState;
    uint8_t  	blankingState;
    BandFilter  bandFilter[10];
} FuncConfig;

typedef struct FuncKnownTrackProc
{
    uint8_t  	tte;
    uint8_t  	tpr;
    uint8_t  	idConfidenceLevel;
} FuncKnownTrackProc;

typedef struct UnkTrackTimeoutCriteria
{
    uint8_t  	trackAgeOutCriteria;
    uint16_t 	timeout;
    uint16_t 	timeoutCw;
    uint8_t  	noLostArp;
} UnkTrackTimeoutCriteria;

typedef struct UnkHighAmpTracks
{
    int8_t  	ampThreshold;
    uint8_t  	tteHighAmp;
    uint8_t  	tprHighAmp;
    uint8_t  	flagHighAmp;
} UnkHighAmpTracks;

typedef struct UnkDopplerTracks
{
    uint8_t  	dutyCycleThreshold;
    uint8_t  	tteDoppler;
    uint8_t  	tprDoppler;
    uint8_t  	flagDoppler;
} UnkDopplerTracks;

typedef struct UnkCwTracks
{
    uint8_t  	tteCw;
    uint8_t  	tprCw;
    uint8_t  	flagCw;
    uint16_t	cwLower;
    uint16_t	cwUpper;
} UnkCwTracks;

typedef struct UnkLockOnTracks
{
    uint8_t  	tteLockOn;
    uint8_t  	tprLockOn;
    uint8_t  	flagLockOn;
} UnkLockOnTracks;

typedef struct UnkHighPrfTracks
{
    uint8_t  	priThreshold;
    uint8_t  	tteHighPrf;
    uint8_t  	tprHighPrf;
    uint8_t  	flagHighPrf;
} UnkHighPrfTracks;

typedef struct FuncUnknownTrackProc
{
    uint8_t  	defaultTte;
    uint8_t  	defaultTpr;
    UnkTrackTimeoutCriteria unkTimeoutCriteria;
    UnkHighAmpTracks unkHighAmp;
    UnkDopplerTracks unkDoppler;
    UnkCwTracks unkCw;
    UnkLockOnTracks unkLockOn;
    UnkHighPrfTracks unkHighPrf;
} FuncUnknownTrackProc;

typedef struct FuncParams
{
    FuncConfig funcConfig;
    FuncKnownTrackProc funcKnownTrackProc;
    FuncUnknownTrackProc funcUnknownTrackProc;
} FuncParams;

typedef struct ThreatLibraryConstants
{
    uint8_t	minAMP;
    uint8_t	responseTime;
    uint8_t	dwellTime;
    uint8_t	deltaT;
    uint8_t	recordInterval;
    uint8_t	reserved;
    RSS     rss;
    MDF		mdf;
    FuncParams	funcParams;
    uint32_t	CRC;
} ThreatLibraryConstants;

typedef struct rfStruct
{
    uint16_t rfLower;
    uint16_t rfUpper;
}rfStruct;

typedef struct RF
{
    rfStruct	rfRanges[MAX_RF_LIBRARY];
    uint8_t		rfType;
    uint8_t		rfCount;
} RF;

/*
typedef struct FixedPRIStruct
{
    uint32_t priLower;
    uint32_t priUpper;
} FixedPRIStruct;

typedef struct JitterPRIStruct
{
    uint32_t priLower;
    uint32_t priUpper;
} JitterPRIStruct;

typedef struct SlidingPRIStruct
{
    uint32_t priLower;
    uint32_t priUpper;
} SlidingPRIStruct;

typedef struct StaggerPRIStruct
{
    uint32_t priLower;
    uint32_t priUpper;
} StaggerPRIStruct;

typedef struct DwellPRIStruct
{
    uint32_t priLower;
    uint32_t priUpper;
} DwellPRIStruct;

typedef struct ComplexPRIStruct
{
    uint32_t priLower;
    uint32_t priUpper;
} ComplexPRIStruct;

typedef struct PRI
{
    StaggerPRIStruct staggerPRIRanges[MAX_PRI_LIBRARY];
    DwellPRIStruct dwellPRIRanges[MAX_PRI_LIBRARY];
    ComplexPRIStruct complexPRIRanges[MAX_PRI_LIBRARY];
    JitterPRIStruct jitterPRIRanges[16];
    SlidingPRIStruct slidingPRIRanges[16];
    FixedPRIStruct fixedPRIRanges;
    uint8_t StaggerPRICount;
    uint8_t DwellPRICount;
    uint8_t ComplexPRICount;
    uint8_t JitterPRICount;
    uint8_t SlidingPRICount;
    uint8_t priType;
} PRI;*/

typedef struct PriSpots
{
    uint32_t priLower;
    uint32_t priUpper;
}PriSpots;

typedef struct StaggerFrame
{
    uint32_t priLower;
    uint32_t priUpper;
}StaggerFrame;

typedef struct PRI
{
    PriSpots priSpots[MAX_PRI_LIBRARY];
    StaggerFrame staggerFrame[8];
    uint8_t priCount;
    uint8_t priType;
} PRI;

typedef struct pwStruct
{
    uint16_t pwLower;
    uint16_t pwUpper;
}pwStruct;

typedef struct PW
{
    pwStruct 	pwRanges[MAX_PW_LIBRARY];
    uint8_t		pwType;
    uint8_t 	pwCount;
} PW;

typedef struct ARP
{
    uint16_t	arpLower;
    uint16_t	arpUpper;
    uint16_t    beamwidthLower;
    uint16_t    beamwidthUpper;
    uint16_t    illuminationTime;
    uint8_t		arpType;
} ARP;

typedef struct ERP
{
    uint16_t erp;
} ERP;

typedef struct PRI_Sequence
{
    uint16_t	seqSum;
    uint16_t 	silence;
    uint8_t 	sequence[MAX_SEQ_LIBRARY];
    uint8_t 	orderImportant;
    uint8_t 	seqCount;
}PRI_Sequence;

typedef struct EmitterScores
{
    uint8_t priScore;
    uint8_t pwScore;
    uint8_t rfScore;
    uint8_t priTypeScore;
    uint8_t pwTypeScore;
    uint8_t rfTypeScore;
    uint8_t lpiScore;
    uint8_t priCountSTTReq;
} EmitterScores;

typedef struct LPI
{
    uint8_t AMOP;
    uint8_t FMOP;
    uint8_t PMOP;
}LPI;

typedef struct EmitterData
{
    //Id of emitter PK
    uint16_t emitterID;
    //batch number 0-255
    uint16_t batchNumber;
    //type of emitter (string)
    uint8_t emitterType;
    //emitter name (combination of 0-9 and a-z) em01
    uint8_t emitterName[16];
    //combination of ASCII i-e AA-YZ
    uint8_t modeID[2];
    //demod pointer [0.0-2147483647]
    uint32_t demod_pointer;
    //String platform (Ship, single cannon)
    uint8_t platform;
    //Friend ,  Foe etc
    uint8_t attribute;
    //radar name String
    uint8_t radarName;
    //Threat Type Emitter [0.0-15.0]
    uint16_t TTE;
    //Threat Priority Rank [0.0-15.0]
    uint16_t TPR;
    //warning, missile, gun etc
    uint8_t emitterFunction;
    //tone1, tone2, tone3, tone4
    uint8_t warnings;
    uint8_t suppressEmitter;
    uint8_t legacyCorrelation;
    uint16_t SymbolCode1;
    uint16_t SymbolCode2;
    uint16_t SymbolCode3;
    uint16_t SymbolCode4;
    uint16_t SymbolCode5;
    uint8_t cmSuggested;
    uint8_t cmAlternative;
    uint8_t padding[25];

    PRI pri;
    PW pw;
    RF rf;
    ERP erp;
    LPI lpi;
    ARP arp;
    EmitterScores scores;
    PRI_Sequence SEQ[16];
} EmitterData;

typedef struct LibraryStruct
{
    ThreatLibraryHeader         header;
    EmitterData                 Emitters[MAX_LIB_ENTRIES];
    ThreatLibraryConstants      constants;
} ThreatLibrary;

typedef struct RFStruct
{
    int8_t rf[8];
} RFStruct;

typedef struct PRIStruct
{
    int8_t pri[8];
} PRIStruct;

typedef struct PWStruct
{
    int8_t pw[8];
} PWStruct;

typedef struct EmitterRecord
{
   int8_t eId[8];
   int8_t band[1];
   int8_t rfType[8];
   int8_t priType[8];
   int8_t pwType[8];
   RFStruct RF[MAX_RF_LIBRARY];
   PRIStruct PRI[MAX_PRI_LIBRARY];
   PWStruct PW[MAX_PW_LIBRARY];
   int8_t arpType[16];
} EmitterRecord;

typedef struct EmitterLibraryRecord
{
    EmitterRecord emitterRec[MAX_LIB_ENTRIES];
} EmitterLibraryRecord;

typedef struct MatchRecord
{
    int8_t rfTypePercent[2];
    int8_t priTypePercent[2];
    int8_t pwTypePercent[2];
    int8_t arpTypePercent[2];
    F32 rfValuePercent;
    F32 priValuePercent;
    F32 pwValuePercent;
} MatchRecord;

typedef struct MatchLibraryRecord
{
    MatchRecord matchRecords[MAX_LIB_ENTRIES];
} MatchLibraryRecord;

typedef struct ThreatLibraryUnencryptedHeader
{
	uint64_t delimeter;
    uint8_t hash[32];
    uint8_t iv[16];
} ThreatLibraryUnencryptedHeader;

typedef struct LibraryMatchScore
{
    UWord16 ID;
    float  PRIScore;
    float  PRItypeScore;
    float  RFScore;
    float  RFtypeScore;
    float  PWScore;
    float  PWtypeScore;
    float  LpiScore;
    float  Total;
    uint8_t dwellCount;
	uint8_t staggerCount;
	uint8_t slidingCount;
	uint8_t jitterCount;
	uint8_t complexCount;
	uint8_t diffToaCount;
	uint8_t noOfSpotsMatched;
	uint8_t libraryPriCount;
	uint16_t emitterBatchId;
	uint16_t symbolOne;
	uint16_t symbolTwo;
	uint16_t symbolThree;
	uint16_t symbolFour;
	uint16_t symbolFive;
	uint16_t symbolTop;
	uint16_t symbolOuter;
}LibraryMatchScore;

class LibraryMetaClass
{
public:
	static const int SIZE_OF_KEY = 33;
	static const long SIZE_OF_THREAT_LIBRARY = sizeof(LibraryStruct) + 1000;

	uint16_t numberOfEmmiters;
	uint32_t numberofPris;
	uint32_t numberofStaggerPRIs;
	uint32_t numberofComplexPRIs;
	uint32_t numberofJitterPRIs;
	uint32_t numberofSliderPRIs;
	uint32_t numberofPWs;
	uint32_t numberofRFs;
	uint32_t PRIs[20000][200];
	uint32_t PRIcount[20000];

	uint16_t RFs[256][200];
	uint16_t RFcount[256];
	uint16_t PWs[256][200];
	uint16_t PWcount[256];

	uint8_t	keyDigest[32];
	uint8_t	bufferForUse1[SIZE_OF_THREAT_LIBRARY];
	uint8_t	bufferForUse2[SIZE_OF_THREAT_LIBRARY];
	uint8_t bufferForKey[SIZE_OF_KEY];

	uint8_t rfBandExistsInLibraryFlags[10000][6];

	LibraryMetaClass();
	virtual ~LibraryMetaClass();
};


/*
 * Function Name:   returnCrcFromStructure
 * Purpose:         This function initializes Threat Library Structure
 * Inputs:          None
 * Outputs:         None
 * Scope:           Normal (can be called by any file that has header file included)
 */
//void libraryInitStructure(LibraryStruct& library);

#endif /* LIBRARYCLASS_H_ */
