#!../../bin/linux-x86_64/test

## You may have to change test to something else
## everywhere it appears in this file

< envPaths

cd ${TOP}

## Register all support components
dbLoadDatabase "dbd/test.dbd"
test_registerRecordDeviceDriver pdbbase

               
## Port name	   asyn port name
## Link Type       (see CAEN documentation)
## LinkNum 	       (see CAEN documentation)
## ConetNode 	   (see CAEN documentation)
## maxBuffers  	 	
## maxMemory
## priority
## stackSize

epicsEnvSet("PORT",       "CAENPort")
epicsEnvSet("LinkType",   "1")
epicsEnvSet("LinkNum",    "0")
epicsEnvSet("ConetNode",  "0")
epicsEnvSet("maxBuffers", "32")
epicsEnvSet("maxMemory",  "404800")
epicsEnvSet("priority",   "1")
epicsEnvSet("stackSize",  "1000")

configureCAENDT5730("$(PORT)", $(LinkType), $(LinkNum), $(ConetNode), $(maxBuffers), $(maxMemory), $(priority), $(stackSize))
asynSetTraceMask    "$(PORT)", 0, "0xFFFF"


epicsEnvSet("P", "CAEN:")
epicsEnvSet("R", "DT5730:")

## Load record instances
dbLoadRecords("db/caen.db",        "P=$(P), R=$(R), PORT=$(PORT), ADDR=0, TIMEOUT=1")

dbLoadRecords("db/gtrchannel.db",  "P=$(P), R=$(R), PORT=$(PORT), TIMEOUT=1, CHID=CH0:, N=0, SIZE=360, TYPE=USHORT, DTYPE=asynInt16ArrayIn")
dbLoadRecords("db/caenchannel.db", "P=$(P), R=$(R), PORT=$(PORT), TIMEOUT=1, CHID=CH0:, N=0")
dbLoadRecords("db/caendt5730channel.db", "P=$(P), R=$(R), PORT=$(PORT), TIMEOUT=1, CHID=CH0:, N=0")

cd $(AREA_DETECTOR)/db
# This creates a waveform
NDStdArraysConfigure("WF0", 10, 0, "$(PORT)", 0)
dbLoadRecords("NDStdArrays.template", "P=$(P),R=$(R)WF0:,PORT=WF0, ADDR=0,TIMEOUT=1,NDARRAY_PORT=$(PORT),NDARRAY_ADDR=11,TYPE=Int16,FTVL=USHORT,NELEMENTS=3600")

NDStdArraysConfigure("WF1", 10, 1, "$(PORT)", 1)
dbLoadRecords("NDStdArrays.template", "P=$(P),R=$(R)WF1:,PORT=WF1, ADDR=1,TIMEOUT=1,NDARRAY_PORT=$(PORT),NDARRAY_ADDR=11,TYPE=Int16,FTVL=USHORT,NELEMENTS=3600")


cd ${TOP}/iocBoot/${IOC}
iocInit

