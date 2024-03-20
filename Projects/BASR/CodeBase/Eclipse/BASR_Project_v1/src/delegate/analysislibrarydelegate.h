/*
 * analysislibrary.h
 *
 *  Created on: Oct 17, 2018
 *      Author: hassan
 */

#ifndef ANALYSISLIBRARYDELEGATE_H_
#define ANALYSISLIBRARYDELEGATE_H_

#include "../common/common.h"
#include "../model/model.h"
#include "../vo/libraryclass.h"
#include "../vo/librarycfdclass.h"
#include "../io/fileinputbinary.h"
#include "../utility/aes.h"
#include "../utility/sha256.h"
#include "../io/cfcard.h"
#include <limits.h>

//notes
//add bound check in library hashing for all hash types
//end of notes


//All definitions

#ifdef ANALYSISLIBRARYDELEGATE_CPP_
//All file scope variables
unsigned short UnknownModeCodes[9] = {0xD220,0xD240,0xD270,0xD2A0,0xD2D0,0xD300,0xD330,0xD350,0xD380};

#else
//Global variables using extern

#endif
//End of conditional exclusion

void libraryAssignSymbolEmitterAll(vector<TrackDefaultClass> &tracks,
		LibraryStruct &library);
void libraryCalculatePriorityOfTracks(vector<TrackDefaultClass> &tracks,
		LibraryStruct &library);
void libraryHideTracksWithSameDemodPtr(vector<TrackDefaultClass> &tracks,
		LibraryStruct &library);
void libraryMatchEmitterPreprocess(TrackDefaultClass &track,
		LibraryStruct &library, LibraryMetaClass &libraryMeta);
void libraryMatchEmitterPostprocess(TrackDefaultClass &track,
		LibraryStruct &library, LibraryMetaClass &libraryMeta, CfdLibrary &cfdLib);

/*
 * Function Name:   libraryMatchEmitter
 * Purpose:			This function matches a single Track to library, sequence of operation is as:
 * 					First, we extract PRIs, PWs, RFs from track irrespective of their types
 * 					Then we extract Library Emmiter IDs from PRI,RF,PW hash of library
 * 					After extraction, track is matched with all library emmiters and scored
 * 					Like for like types are matched in PRI,RF anfd PW. In all these, there are
 * 					3 types of matching, points on points, points on ranges and ranges on ranges
 * 					overlap. In all these, percentage of overlap is multiplied by total score
 * 					In the end, top 3 scorers are copied to track as library match
 * Inputs:          Track, Library and metadata of library
 * Outputs:         none
 * Scope:           Can be called by any function
 */
void libraryMatchEmitter(TrackDefaultClass &track, LibraryStruct &library,
		LibraryMetaClass &libraryMeta);
void libraryCountTrackPriTypes(TrackPriClass &pri);
void libraryCountTrackRfTypes(TrackRfClass &rf);
uint16_t libraryNumberOfPri(TrackPriClass &pri, TrackLibraryMatchClass &match);
uint16_t libraryNumberOfRf(TrackRfClass &rf);
uint16_t libraryNumberOfPw(TrackPwClass &pw);
void libraryScoreInit(vector<LibraryMatchScore> &scoresTotal, Word32 number);
void libraryCalculateTotal(LibraryMatchScore * sc);
void libraryTopThreeIds(vector<LibraryMatchScore> &sc, uint32_t n, vector<uint8_t> &indexes);
void libraryCopyTopScores(vector<LibraryMatchScore> &sc, TrackLibraryMatchClass &match ,UWord32 n);
void libraryRequestMatch(TrackDefaultClass &track);
void libraryRequestForceUnknown(TrackDefaultClass &track);
void libraryRequestResetTrackParameters(TrackDefaultClass &track);
void libraryResetTrackParameters(TrackDefaultClass &track);
Word32 libraryRemoveDifferentBandsFromArray(TrackDefaultClass &track,
		LibraryMetaClass &libraryMeta, UWord16 outputArr[], UWord16 arr[],
		UWord32 size);
void libraryBandQuery(LibraryStruct &threatLibrary,
				LibraryMetaClass &libraryMeta);

//
//Function Name:   returnCrcFromStructure
//Purpose:         This function initializes Threat Library Structure
//Inputs:          None
//Outputs:         None
//Scope:           Normal (can be called by any file that has header file included)
//
void libraryInitStructure(LibraryStruct& library);

void libraryLoading(LibraryStruct &library, LibraryMetaClass &libraryMeta,
		CfdLibraryStruct &cfdLibrary, CfdLibraryMetaClass &cfdLibraryMeta,
		CfCard &cfCard);

//
// Function Name:   hashThreatLibrary
// Purpose:         This function extracts Threat Library CRC and matches it with
//                  calculated CRC, if both are Same; Threat Library is hashed
// Inputs:          Threat Library File Name
// Outputs:         Acknowledgement of CRC matching and Hashing done
// Scope:           Normal (can be called by any file that has header file included)
///
void loadDefaultLibrary(LibraryStruct &library, LibraryMetaClass &libraryMeta);
Word32 libraryHashing(LibraryStruct& library, LibraryMetaClass& libraryMeta);

//
// Function Name:   priHashing
// Purpose:         This function extracts PRI spots from Threat Library and Hashes
//                  it into our PRI hash tables
// Inputs:          Threat Library
// Outputs:         None
// Scope:           Normal (can be called by any file that has header file included)
//
void libraryPriHashing(LibraryStruct& threatLibrary, LibraryMetaClass& libraryMeta);

//
// Function Name:   rfHashing
// Purpose:         This function extracts RF spots from Threat Library and Hashes
//                  it into our PRI hash tables
// Inputs:          Threat Library
// Outputs:         None
// Scope:           Normal (can be called by any file that has header file included)
//
void libraryRfHashing(LibraryStruct& threatLibrary, LibraryMetaClass& libraryMeta);

//
// Function Name:   pwHashing
// Purpose:         This function extracts PW spots from Threat Library and Hashes
//                  it into our PRI hash tables
// Inputs:          Threat Library
// Outputs:         None
// Scope:           Normal (can be called by any file that has header file included)
//
void libraryPwHashing(LibraryStruct& threatLibrary, LibraryMetaClass& libraryMeta);

//
// Function Name:   initPriArray
// Purpose:         This function zero initializes the PRI array
// Inputs:          None
// Outputs:         None
// Scope:           Normal (can be called by any file that has header file included)
//
void libraryInitPriArray(LibraryMetaClass& libraryMeta);

//
// Function Name:   initRfArray
// Purpose:         This function zero initializes the RF array
// Inputs:          None
// Outputs:         None
// Scope:           Normal (can be called by any file that has header file included)
//
void libraryInitRfArray(LibraryMetaClass& libraryMeta);

//
// Function Name:   initPwArray
// Purpose:         This function zero initializes the PW array
// Inputs:          None
// Outputs:         None
// Scope:           Normal (can be called by any file that has header file included)
//
void libraryInitPwArray(LibraryMetaClass& libraryMeta);

//
// Function Name:   initHashingStructure
// Purpose:         This function zero initializes all the hashing arrays
// Inputs:          None
// Outputs:         None
// Scope:           Normal (can be called by any file that has header file included)
//
void libraryInitHashingStructure(LibraryMetaClass& libraryMeta);

//
// Function Name:   priLookup
// Purpose:         This function extracts all the IDs from spots sequentially
//                  and copy them to IDs
// Inputs:          PRI Spots
//                  Number of PRI spots
//                  All IDs (to be returned from function itself)
// Outputs:         Number of IDs extracted for PRI spots
// Scope:           Normal (can be called by any file that has header file included)
//
uint32_t libraryPriLookup(vector <uint32_t> &spots, uint32_t num,
		uint16_t IDs[], LibraryMetaClass &libraryMeta);

//
// Function Name:   rfLookup
// Purpose:         This function extracts all the IDs from spots sequentially
//                  and copy them to IDs
// Inputs:          RF Spots
//                  Number of RF spots
//                  All IDs (to be returned from function itself)
// Outputs:         Number of IDs extracted for RF spots
// Scope:           Normal (can be called by any file that has header file included)
//
uint32_t libraryRfLookup(circular_buffer<LimitRfClass> &spots, uint32_t num, uint16_t IDs[],
		LibraryMetaClass& libraryMeta);

//
// Function Name:   pwLookup
// Purpose:         This function extracts all the IDs from spots sequentially
//                  and copy them to IDs
// Inputs:          PW Spots
//                  Number of PW spots
//                  All IDs (to be returned from function itself)
// Outputs:         Number of IDs extracted for PW spots
// Scope:           Normal (can be called by any file that has header file included)
//
uint32_t libraryPwLookup(circular_buffer<LimitPwClass> &spots, uint32_t num, uint16_t IDs[],
		LibraryMetaClass& libraryMeta);

//
// Function Name:   getMax
// Purpose:         Maximum Element of Array is discovered
// Inputs:          Array
//                  Size of Array
// Outputs:         Maximum Element of Array
// Scope:           Normal (can be called by any file that has header file included)
//

Word32 libraryGetMax(UWord16 arr[], UWord32 n);

//
// Function Name:   countSort
// Purpose:         Count Sort is applied on array according to given exponent
// Inputs:          Array
//                  Size of Array
//                  Exponent at which it is to be sorted
// Outputs:         Sorted Array
// Scope:           Normal (can be called by any file that has header file included)
//

void libraryCountSort(UWord16 arr[], UWord32 n, UWord32 exp);

//
// Function Name:   radixSort
// Purpose:         Radix Sort is applied on array
// Inputs:          Array
//                  Size of Array
// Outputs:         Sorted Array
// Scope:           Normal (can be called by any file that has header file included)
//

void libraryRadixSort(UWord16 arr[], UWord32 n);

//
// Function Name:   printArray
// Purpose:         Array is printed/displayed
// Inputs:          Array
//                  Size of Array
// Outputs:         None
// Scope:           Normal (can be called by any file that has header file included)
//

void libraryPrintArray(UWord16 arr[], UWord32 n);

//
// Function Name:   removeDuplicateFromArray
// Purpose:         Duplicates and recurring items are removed from the array
// Inputs:          Array
//                  Size of Array
// Outputs:         Size of unique/duplicate-free array
// Scope:           Normal (can be called by any file that has header file included)
//

Word32 libraryRemoveDuplicateFromArray(UWord16 * arr, UWord32 size);

//
// Function Name:   Union
// Purpose:         This function performs union on array 1 and array 2 and stores
//                  it to array 3
// Inputs:          Array 1
//                  Array 2
//                  Array 3 (output unified array will be returned in it)
//                  Size of Array 1
//                  Size of Array 2
// Outputs:         Size of Array 3
// Scope:           Normal (can be called by any file that has header file included)
//

Word32 libraryUnion(UWord16 arr1[], UWord16 arr2[], UWord16 arr3[], Word32 m, Word32 n);

//
// Function Name:   Intersection
// Purpose:         This function performs intersection on array 1 and array 2 and stores
//                  it to array 3
// Inputs:          Array 1
//                  Array 2
//                  Array 3 (output array will be returned in it)
//                  Size of Array 1
//                  Size of Array 2
// Outputs:         Size of Array 3
// Scope:           Normal (can be called by any file that has header file included)
//

void swap(uint16_t* a, uint16_t* b);
int16_t partition (uint16_t arr[], int16_t low, int16_t high);
void quickSort(uint16_t arr[], int16_t low, int16_t high);

Word32 libraryIntersection(UWord16 arr1[], UWord16 arr2[],UWord16 arr3[], Word32 m, Word32 n);
F32 libraryFindOverlapRegionPercentage(UWord32 regionX1, UWord32 regionX2, UWord32 regionY1, UWord32 regionY2);
UWord16 libraryRoundFloat(F64 number);
void libraryPrintHighEmitter(TrackDefaultClass &track);

void libraryRFBandExists(RF rf, uint32_t iterator, LibraryMetaClass &libraryMeta);

#endif // ANALYSISLIBRARYDELEGATE_H_
