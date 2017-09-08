/*
 * gtrBaseDriver.cpp
 *
 *  Created on: 24. mar. 2016
 *      Author: Vyacheslav Isaev
 */

#include "gtrBaseDriver.h"

GTRBaseDriver::GTRBaseDriver(const char *portName, int maxAddr, int numParams, int maxBuffers, size_t maxMemory,
        int interfaceMask, int interruptMask,
        int asynFlags, int autoConnect, int priority, int stackSize)
        :asynNDArrayDriver (
        		portName
        		,maxAddr
        		,numParams+NUM_GTR_PARAMS
        		,maxBuffers
        		,maxMemory
                ,interfaceMask
                ,interruptMask
                ,asynFlags, autoConnect, priority, stackSize)
{

    createParam("NAME",    		asynParamOctet,           &GTRPortNameSelf);
    createParam("ARM",      	asynParamInt32,           &GTRArmSelf);
    createParam("CLOCK",    	asynParamInt32,           &GTRClockSelf);
    createParam("TRIGGER",  	asynParamInt32,           &GTRTriggerSelf);
    createParam("MULTI_EVENT",  asynParamInt32,           &GTRMultiEventSelf);
    createParam("PRE_AVERAGE",  asynParamInt32,           &GTRPreAverageSelf);
    createParam("NUMBER_PTS",   asynParamInt32,           &GTRNumberPTSSelf);
    createParam("NUMBER_PPS",   asynParamInt32,           &GTRNumberPPSSelf);
    createParam("NUMBER_PTE",   asynParamInt32,           &GTRNumberPTESelf);
    createParam("SW_TRG",       asynParamInt32,           &GTRSWTrgSelf);
    createParam("AUTO_REARM",   asynParamInt32,           &GTRAutoRearmSelf);
}

GTRBaseDriver::~GTRBaseDriver()
{
}

