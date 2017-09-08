
#include <iocsh.h>
#include <epicsExport.h>

#include "caenDT5730Driver.h"

static void CAENDT5730Config(const char *portName
		,int aLinkType
		,int aLinkNum
		,int aConetNode
		,int maxBuffers
		,size_t maxMemory
		,int priority
		,int stackSize)
{
	CAENDT5730Driver *drv = new CAENDT5730Driver(portName
			,aLinkType
			,aLinkNum
			,aConetNode
		,maxBuffers
		,maxMemory
        ,priority
        ,stackSize);
}

/* Code for iocsh registration */
static const iocshArg CAENDT5730ConfigArg0 = {"Port name",    	iocshArgString};
static const iocshArg CAENDT5730ConfigArg1 = {"Link Type",      iocshArgInt};
static const iocshArg CAENDT5730ConfigArg2 = {"LinkNum", 	    iocshArgInt};
static const iocshArg CAENDT5730ConfigArg3 = {"ConetNode",      iocshArgInt};
static const iocshArg CAENDT5730ConfigArg4 = {"maxBuffers",   	iocshArgInt};
static const iocshArg CAENDT5730ConfigArg5 = {"maxMemory", 		iocshArgInt};
static const iocshArg CAENDT5730ConfigArg6 = {"priority", 		iocshArgInt};
static const iocshArg CAENDT5730ConfigArg7 = {"stackSize", 		iocshArgInt};


static const iocshArg * const CAENDT5730ConfigArgs[] = {&CAENDT5730ConfigArg0,
                                                       &CAENDT5730ConfigArg1,
                                                       &CAENDT5730ConfigArg2,
                                                       &CAENDT5730ConfigArg3,
                                                       &CAENDT5730ConfigArg4,
                                                       &CAENDT5730ConfigArg5,
                                                       &CAENDT5730ConfigArg6,
                                                       &CAENDT5730ConfigArg7
                                                       };

static const iocshFuncDef configcaenDT5730 = {"configureCAENDT5730", 8, CAENDT5730ConfigArgs};

static void configCAENDT5730CallFunc(const iocshArgBuf *args)
{
    CAENDT5730Config(args[0].sval
    				,args[1].ival
    				,args[2].ival
                    ,args[3].ival
                    ,args[4].ival
                    ,args[5].ival
                    ,args[6].ival
                    ,args[7].ival
                    );
}


static void caenDT5730Registrar(void)
{
    iocshRegister(&configcaenDT5730, configCAENDT5730CallFunc);
}

extern "C" {
epicsExportRegistrar(caenDT5730Registrar);
}
