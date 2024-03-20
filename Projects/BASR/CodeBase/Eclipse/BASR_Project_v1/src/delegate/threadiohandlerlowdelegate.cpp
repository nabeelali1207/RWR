/*
 * threadiohandlerlowdelegate.cpp
 *
 *  Created on: Jul 9, 2020
 *      Author: GMD
 */

#include "threadiohandlerlowdelegate.h"
#include "../utility/threadutility.h"
void * threadIoHandlerLow(void *)
{
//	uint64_t lastBenchmarkTime = Utility::getSystemTime();
	cout << "PDW Thread:" << getMyProcessorCore() <<endl;


	while (1)
	{
		Model::dspLivePdwReceiver.readAvailableData();
		Model::dspSelfTestController.performSelfTest();
		Model::dspSelfTestController.handleVdmPcieTriggeredSelfTest();

//		uint64_t currTime = Utility::getSystemTime();
//
//		if (currTime - lastBenchmarkTime > 1000000)
//		{
//			lastBenchmarkTime=currTime;
//			cout << "Benchmarks (Itr,pdws,memcpyTime,parseTime,enqueTime): {"
//					<< Model::dspLivePdwReceiver.memcpyItrs			<< ", "
//					<< Model::dspLivePdwReceiver.memcpyBytes		<< ", "
//					<< Model::dspLivePdwReceiver.memcpyTimeBenchmark<< ", "
//					<< Model::dspLivePdwReceiver.parseTimeBenchmark	<< ", "
//					<< Model::dspLivePdwReceiver.enqueTimeBenchmark	<< "}"
//					<< endl;
//		}

//		usleep(1000);
	}
	return NULL;
}
