/*
 * threadutility.cpp
 *
 *  Created on: Oct 22, 2021
 *      Author: GMD
 */

#include "threadutility.h"

void getCurrentThreadIdAndAttrib(pthread_t& thread, pthread_attr_t& attribs)
{
	thread = pthread_self();
	pthread_getattr_np(thread, &attribs);
}
void setCurrentThreadAffinity(pthread_t& thread, cpu_set_t& cpuSet, int8_t core)
{
	CPU_ZERO(&cpuSet);
	if (core < 0 || core > 3)
	{
		CPU_SET(0, &cpuSet);
		CPU_SET(1, &cpuSet);
		CPU_SET(2, &cpuSet);
		CPU_SET(3, &cpuSet);
	}
	else
	{
		CPU_SET((uint32_t )core, &cpuSet);
	}
	pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuSet);
}

void launchNewThread(pthread_t& thread, pthread_attr_t& attribs,
		cpu_set_t& cpuSet, int8_t core, void *(*__start_routine)(void *),
		void *__restrict __arg)
{
	CPU_ZERO(&cpuSet);
	if (core < 0 || core > 3)
	{
		CPU_SET(0, &cpuSet);
		CPU_SET(1, &cpuSet);
		CPU_SET(2, &cpuSet); // reserved
		CPU_SET(3, &cpuSet);
	}
	else
	{
		CPU_SET((uint32_t )core, &cpuSet);
	}
	// Set Threat Attribute to Core 1
	pthread_attr_setaffinity_np(&attribs, sizeof(cpu_set_t), &cpuSet);
	pthread_create((pthread_t *) &thread, &attribs, __start_routine, __arg);
}

int32_t getMyProcessorCore()
{
	return sched_getcpu();
}
