/*
 * gtrBaseDriver.h
 *
 *  Created on: 25. mar. 2016
 *      Author: sisaev
 */

#ifndef GTRBASEDRIVER_H_
#define GTRBASEDRIVER_H_

#include <cstdint>
#include <epicsTypes.h>

#include <asynNDArrayDriver.h>

#define NUM_GTR_PARAMS (&LAST_GTR_PARAM - &FIRST_GTR_PARAM + 1)

class GTRBaseDriver : public asynNDArrayDriver
{
protected:
	// Parameters asyn IDs
    epicsInt32 GTRPortNameSelf;
	#define FIRST_GTR_PARAM GTRPortNameSelf

    epicsInt32 GTRArmSelf;
    epicsInt32 GTRClockSelf;
    epicsInt32 GTRTriggerSelf;
    epicsInt32 GTRMultiEventSelf;
    epicsInt32 GTRPreAverageSelf;
    epicsInt32 GTRNumberPTSSelf;
    epicsInt32 GTRNumberPPSSelf;
    epicsInt32 GTRNumberPTESelf;
    epicsInt32 GTRSWTrgSelf;
    epicsInt32 GTRAutoRearmSelf;
    epicsInt32 GTRReadDataSelf;
	#define LAST_GTR_PARAM GTRReadDataSelf


public:
    GTRBaseDriver(const char *portName, int maxAddr, int numParams, int maxBuffers, size_t maxMemory,
            int interfaceMask, int interruptMask,
            int asynFlags, int autoConnect, int priority, int stackSize);

    virtual ~GTRBaseDriver();

    void updateWaveform();

private:
    GTRBaseDriver(const GTRBaseDriver&);
    GTRBaseDriver& operator=(const GTRBaseDriver&);
};

#endif /* GTRBASEDRIVER_H_ */
