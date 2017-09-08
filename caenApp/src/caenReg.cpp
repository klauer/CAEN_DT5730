
#include <iocsh.h>
#include <epicsExport.h>

#include "caenDriver.h"

static void CAENConfig(const char *portName
		,int aLinkType
		,int aLinkNum
		,int aConetNode
		,int aVmeBaseAddress
		,int maxAddr, int maxBuffers, size_t maxMemory, int priority, int stackSize)
{
	int interfaceMask =	asynCommonMask
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
	| asynGenericPointerMask;

	int interruptMask = asynInt32Mask
			| asynUInt32DigitalMask
			| asynFloat64Mask
			| asynOctetMask
			| asynInt8ArrayMask
			| asynInt16ArrayMask
			| asynInt32ArrayMask
			| asynFloat32ArrayMask
			| asynFloat64ArrayMask
			| asynGenericPointerMask;

	CAENDriver *drv = new CAENDriver(portName
		,aLinkType
		,aLinkNum
		,aConetNode
		,aVmeBaseAddress
		,maxAddr
		,0
		,maxBuffers
		,maxMemory
        ,interfaceMask
        ,interruptMask
        ,0
        ,1
        ,priority
        ,stackSize);
}

/* Code for iocsh registration */
static const iocshArg CAENConfigArg0 = {"Port name",      iocshArgString};
static const iocshArg CAENConfigArg1 = {"Link Type",      iocshArgInt};
static const iocshArg CAENConfigArg2 = {"LinkNum", 	      iocshArgInt};
static const iocshArg CAENConfigArg3 = {"ConetNode",      iocshArgInt};
static const iocshArg CAENConfigArg4 = {"VmeBaseAddress", iocshArgInt};
static const iocshArg CAENConfigArg5 = {"Channels Num",   iocshArgInt};
static const iocshArg CAENConfigArg6 = {"maxBuffers",     iocshArgInt};
static const iocshArg CAENConfigArg7 = {"maxMemory", 	  iocshArgInt};
static const iocshArg CAENConfigArg8 = {"priority", 	  iocshArgInt};
static const iocshArg CAENConfigArg9 = {"stackSize", 	  iocshArgInt};

static const iocshArg * const CAENConfigArgs[] = {&CAENConfigArg0,
                                                       &CAENConfigArg1,
                                                       &CAENConfigArg2,
                                                       &CAENConfigArg3,
                                                       &CAENConfigArg4,
                                                       &CAENConfigArg5,
                                                       &CAENConfigArg6,
                                                       &CAENConfigArg7,
                                                       &CAENConfigArg8,
                                                       &CAENConfigArg9
                                                       };

static const iocshFuncDef configcaen = {"configureCAEN", 10, CAENConfigArgs};

static void configCAENCallFunc(const iocshArgBuf *args)
{
    CAENConfig(args[0].sval, args[1].ival, args[2].ival,
                    args[3].ival, args[4].ival, args[5].ival
                    ,args[6].ival
                    ,args[7].ival
                    ,args[8].ival
                    ,args[9].ival
                    );
}


static void caenRegistrar(void)
{

    iocshRegister(&configcaen, configCAENCallFunc);
}

extern "C" {
epicsExportRegistrar(caenRegistrar);
}
