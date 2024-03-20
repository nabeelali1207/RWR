/*
 * threadutility.h
 *
 *  Created on: Oct 22, 2021
 *      Author: GMD
 */

#ifndef SRC_UTILITY_THREADUTILITY_H_
#define SRC_UTILITY_THREADUTILITY_H_

#include <pthread.h>
#include <thread>
#include <sched.h>

void getCurrentThreadIdAndAttrib(pthread_t& thread, pthread_attr_t& attribs);
void setCurrentThreadAffinity(pthread_t& thread, cpu_set_t& cpuSet,
		int8_t core);

void launchNewThread(pthread_t& thread, pthread_attr_t& attribs,
		cpu_set_t& cpuSet, int8_t core, void *(*__start_routine)(void *),
		void *__restrict __arg);

int32_t getMyProcessorCore();
#endif /* SRC_UTILITY_THREADUTILITY_H_ */
