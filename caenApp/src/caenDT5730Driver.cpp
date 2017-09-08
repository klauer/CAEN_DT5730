/*
 * caenDT5730Driver.cpp
 *
 *  Created on: 24. mar. 2016
 *      Author: sisaev
 */

#include <asynDriver.h>
#include "caenDT5730Driver.h"

CAENDT5730Driver::CAENDT5730Driver(const char *portName
		,int aLinkType
		,int aLinkNum
		,int aConetNode
		,int maxBuffers
		,size_t maxMemory
        ,int priority
        ,int stackSize):
	CAENDriver(portName
		,aLinkType
		,aLinkNum
		,aConetNode
		,1
		,8
		,NUM_CAEN_PARAMS
		,maxBuffers, maxMemory,
        	  asynCommonMask
        	| asynDrvUserMask
        	| asynOptionMask
        	| asynInt32Mask
        	| asynUInt32DigitalMask
        	| asynFloat64Mask
        	| asynOctetMask
        	| asynInt8ArrayMask
        	| asynInt16ArrayMask
        	| asynInt32ArrayMask
        	| asynFloat32ArrayMask
        	| asynFloat64ArrayMask
        	| asynGenericPointerMask

        	,asynInt32Mask
        			| asynUInt32DigitalMask
        			| asynFloat64Mask
        			| asynOctetMask
        			| asynInt8ArrayMask
        			| asynInt16ArrayMask
        			| asynInt32ArrayMask
        			| asynFloat32ArrayMask
        			| asynFloat64ArrayMask
        			| asynGenericPointerMask
        , 0 // AsynFlags
        , 1 // AutoConnect
        , priority, stackSize)
{
	printf("%s:%s: Creating CAEN DT5730 device.\n",
		  "CAENDT5730", "CAENDT5730Driver");

	//Board parameters
    createParam("FAIL_STATUS",  asynParamInt32,      &DT5730FailStatusSelf);
//    createParam("CLOCK",    	asynParamInt32,      &DT5730ClockSelf);
    createParam("ACQ_STATUS",   asynParamInt32,      &DT5730ACQStatusSelf);

    // Channel specific parameters
    createParam("CH_GAIN",    	asynParamInt32,      &DT5730CHGainSelf);
    createParam("CH_STATUS",    asynParamInt32,      &DT5730CHStatusSelf);

}

CAENDT5730Driver::~CAENDT5730Driver()
{
}


void CAENDT5730Driver::updateBoardStatus()
{
	CAENDriver::updateBoardStatus();

	getACQStatus();
	getFailStatus();
	getClock();
}

void CAENDT5730Driver::updateChannelStatus(uint32_t channel)
{
	CAENDriver::updateChannelStatus(channel);

	getChannelStatus(channel);
	getChannelGain(channel);
}

asynStatus CAENDT5730Driver::getFailStatus()
{
	uint32_t data;
	uint32_t address = 0x8178;
	CAEN_DGTZ_ErrorCode ret =
			readRegister(address, &data);

	// Apply mask
	data = data & 0x3f;

	setIntegerParam(DT5730FailStatusSelf, data);
	return checkCAENResult(ret, "readRegister:FailStatus:");
}

asynStatus CAENDT5730Driver::getACQStatus()
{
	uint32_t data;
	uint32_t address = 0x8104;

	CAEN_DGTZ_ErrorCode ret =
			readRegister(address, &data);

//  Apply mask
//	data = data >> 2;
//	data = ((data >> 6)&FFFFFF80) | (data & 0x7f);

	setIntegerParam(DT5730ACQStatusSelf, data);
	return checkCAENResult(ret, "readRegister:ACQStatus:");
}

asynStatus CAENDT5730Driver::setClock(epicsInt32 clocSource)
{
	uint32_t data;
	uint32_t address = 0x8100;

	CAEN_DGTZ_ErrorCode ret =
			readRegister(address, &data);

	if (0 == ret)
	{
		data = data & (clocSource << 6);
		ret = writeRegister(address, data);
	}
	return checkCAENResult(ret, "writeRegister:Clock:");
}

asynStatus CAENDT5730Driver::getClock()
{
	uint32_t data;
	uint32_t address = 0x8100;

	CAEN_DGTZ_ErrorCode ret =
			readRegister(address, &data);


	uint32_t clock = (data & 0x40) >> 6;

//    asynPrint(this->pasynUserSelf, ASYN_TRACE_WARNING, "CAEN: readRegister:Clock: %X, %X", data, clock );

	setIntegerParam(DT5730ClockSelf, clock);
	return checkCAENResult(ret, "readRegister:Clock:");
}

asynStatus CAENDT5730Driver::setChannelGain(uint32_t channel, uint32_t data)
{
	uint32_t address = 0x1028 + ((channel << 8) & 0x0f00);

	CAEN_DGTZ_ErrorCode ret =
			writeRegister(address, data);

	return checkCAENResult(ret, "writeRegister:ChannelGain:");
}

asynStatus CAENDT5730Driver::getChannelGain(uint32_t channel)
{
	uint32_t data;
	uint32_t address = 0x1028 + ((channel << 8) & 0x0f00);

	CAEN_DGTZ_ErrorCode ret =
			readRegister(address, &data);

	setIntegerParam(channel, DT5730CHGainSelf, data);
	return checkCAENResult(ret, "readRegister:ChannelGain:");
}

asynStatus CAENDT5730Driver::getChannelStatus(uint32_t channel)
{
	uint32_t data;
	uint32_t address = 0x1088 + ((channel << 8) & 0x0f00);

	CAEN_DGTZ_ErrorCode ret =
			readRegister(address, &data);

	data = data & 0x0f;

	setIntegerParam(channel, DT5730CHStatusSelf, data);
	return checkCAENResult(ret, "readRegister:ChannelStatus:");
}

asynStatus CAENDT5730Driver::writeInt32 (asynUser *pasynUser, epicsInt32 value)
{
	int reason = pasynUser->reason;
	int channel;
	pasynManager->getAddr(pasynUser, &channel);

	if (-1 == channel)
	{
		// Device
		if (reason == DT5730ClockSelf)
		{
			return setClock(value);
		} else {
			return CAENDriver::writeInt32 (pasynUser, value);
		}
	}else
	{
		if( reason == DT5730CHGainSelf)
		{
			return setChannelGain(channel, value);
		}else {
			return CAENDriver::writeInt32 (pasynUser, value);
		}
	}

	return asynError;
}

asynStatus CAENDT5730Driver::readInt32 (asynUser *pasynUser, epicsInt32 *value)
{
	if (0==devHandle)
		return asynError;

	int reason = pasynUser->reason;
	int channel;
	pasynManager->getAddr(pasynUser, &channel);

	if (-1 != channel)
	{
		//device
		if( reason == DT5730FailStatusSelf)
		{
			return getFailStatus();
		} if (reason == DT5730ACQStatusSelf)
		{
			return getACQStatus();
		}else{
			return CAENDriver::readInt32 (pasynUser, value);
		}
	}else
	{
		// channel
		if( reason == DT5730CHStatusSelf)
		{
			return getChannelStatus(channel);
		} else {
			return CAENDriver::readInt32 (pasynUser, value);
		}
	}

	return asynError;
}

