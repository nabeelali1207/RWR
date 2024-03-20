/*
 * common.h
 *
 *  Created on: Jul 24, 2018
 *      Author: hassan
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <list>
#include <vector>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <cmath>
#include <cstdlib>
#include <type_traits>
#include <cstdint>
#include <fstream>
#include <sys/stat.h>
#include <sys/mount.h>
#include <time.h>

using namespace std;

#define TRUE 	1
#define FALSE 	0

typedef 	char					Word8;
typedef		unsigned	char		UWord8;
typedef		short					Word16;
typedef		unsigned	short		UWord16;
typedef		int						Word32;
typedef		unsigned	int			UWord32;
typedef		long                    L32;
typedef		unsigned long           UL32;
typedef		long long				Word64;
typedef		unsigned	long long 	UWord64;

typedef signed char		int8_t;
typedef short int		int16_t;
typedef int				int32_t;
typedef unsigned char		uint8_t;
typedef unsigned short int	uint16_t;
typedef unsigned int		uint32_t;
//typedef unsigned long long	uint64_t;



/*-------------------FLOAT----------------------*/
typedef		float 				F32;
typedef		double				F64;

//functions
#ifdef COMMON_CPP_
//global variables
#else
//externs
#endif


#endif /* COMMON_COMMON_H_ */
