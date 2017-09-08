/*
 * caenDT5730Driver.h
 *
 *  Created on: 25. mar. 2016
 *      Author: sisaev
 */

#ifndef CAENDT5730DRIVER_H_
#define CAENDT5730DRIVER_H_

#include "caenDriver.h"

class CAENDT5730Driver : public CAENDriver
{

private:
	CAENDT5730Driver(const CAENDT5730Driver&);
	const CAENDT5730Driver& operator = (const CAENDT5730Driver&);

protected:
	// Parameters asyn IDs
    epicsInt32 DT5730FailStatusSelf;
	#define FIRST_DT5730_PARAM DT5730FailStatusSelf

    epicsInt32 DT5730ClockSelf;
    epicsInt32 DT5730ACQStatusSelf;
    epicsInt32 DT5730CHGainSelf;
    epicsInt32 DT5730CHStatusSelf;
	#define LAST_DT5730_PARAM DT5730CHStatusSelf

public:
	CAENDT5730Driver(const char *portName
			,int aLinkType
			,int aLinkNum
			,int aConetNode
			,int maxBuffers, size_t maxMemory
	        ,int priority, int stackSize);

	virtual ~CAENDT5730Driver();

	virtual void updateBoardStatus();
	virtual void updateChannelStatus(uint32_t channel);

	asynStatus getFailStatus();
	asynStatus getACQStatus();

	asynStatus setClock(epicsInt32);
	asynStatus getClock();


	asynStatus setChannelGain(uint32_t channel, uint32_t val);
	asynStatus getChannelGain(uint32_t channel);

	asynStatus getChannelStatus(uint32_t channel);

	asynStatus writeInt32 (asynUser *pasynUser, epicsInt32 value);
	asynStatus readInt32  (asynUser *pasynUser, epicsInt32 *value);
};


#endif /* CAENDT5730DRIVER_H_ */
