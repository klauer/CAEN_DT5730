/*
 * caenDriver.cpp
 *
 *  Created on: 24. mar. 2016
 *      Author: sisaev
 */

#include <map>
#include <cstring>
#include <sstream>
#include <math.h>
#include <asynPortDriver.h>
#include <cantProceed.h>
#include <epicsThread.h>

#include "mapInit.h"

#include "caenDriver.h"

#define TEST_READOUT 1

#define THREAD_TIMEOUT 1

MapCAENErrorCodes mapCAENErrorCodes;

static void readoutDataC(CAENDriver *driver)
{
    driver->readData();
}

static void processNDBufferC(CAENDriver *driver)
{
    driver->processNDCallbacks();
}

static void testReadOutC(CAENDriver *driver)
{
    driver->testReadOut();
}

CAENDriver::CAENDriver(const char *portName, int aLinkType, int aLinkNum,
        int aConetNode, int aVmeBaseAddress, int maxAddr, int numParams,
        int maxBuffers, size_t maxMemory, int interfaceMask, int interruptMask,
        int asynFlags, int autoConnect, int priority, int stackSize) :
        GTRBaseDriver(portName, maxAddr, NUM_CAEN_PARAMS + numParams,
                maxBuffers, maxMemory, interfaceMask, interruptMask, asynFlags,
                autoConnect, priority, stackSize), linkType(
                (CAEN_DGTZ_ConnectionType) aLinkType), linkNum(aLinkNum), conetNode(
                aConetNode), vmeBaseAddress(aVmeBaseAddress), devHandle(0), bufferSize(
                0), bufferSelf(0), channelsNum(0), event(0), acquisitionStarted(
                false)
{

    printf("%s:%s: Creating CAEN general device.\n", "CAEN", "CAENDriver");

    createParam("MANUF", asynParamOctet, &CAENManufSelf);
    createParam("MODEL", asynParamOctet, &CAENModelSelf);
    createParam("CHANNEL_NUM", asynParamInt32, &CAENChannelNumSelf);
    createParam("FORM_FACTOR", asynParamInt32, &CAENFormFactorSelf);
    createParam("FAMILY", asynParamOctet, &CAENFamilySelf);
    createParam("ROCFW", asynParamOctet, &CAENROCFWSelf);
    createParam("AMCFW", asynParamOctet, &CAENAMCFWSelf);
    createParam("SN", asynParamInt32, &CAENSerialNumSelf);
    createParam("PCB_REV", asynParamInt32, &CAENPCBRevSelf);
    createParam("ADC_N_BITS", asynParamInt32, &CAENADCBitsSelf);
    createParam("LICENSE", asynParamOctet, &CAENLicenseSelf);

    createParam("SW_TRG_MODE", asynParamInt32, &CAENSWTrgModeSelf);
    createParam("EXT_TRG_MODE", asynParamInt32, &CAENExtTrgModeSelf);
    createParam("IO_LEVEL", asynParamInt32, &CAENIOLevelSelf);
    createParam("ACQ_START", asynParamInt32, &CAENACQStartSelf);
    createParam("ACQ_STOP", asynParamInt32, &CAENACQStopSelf);
    createParam("RECORD_LENGTH", asynParamInt32, &CAENRecordLengthSelf);
    createParam("POST_TRG_LEN", asynParamInt32, &CAENPostTrgLenSelf);
    createParam("ACQ_MODE", asynParamInt32, &CAENACQModeSelf);

    createParam("ZERRO_SP_MODE", asynParamInt32, &CAENSerroSpMode);

    createParam("ERR_CODE", asynParamInt32, &CAENLastResultCodeSelf);
    createParam("ERR_DESC", asynParamInt32, &CAENLastResultSelf);
    createParam("CALIBRATE", asynParamInt32, &CAENCalibrateSelf);

    createParam("CH_ENABLED", asynParamInt32, &CAENCHEnabledSelf);
    createParam("CH_SELF_TRG_MODE", asynParamInt32, &CAENCHSelfTrgModeSelf);
    createParam("CH_SELF_TRG_THRSH", asynParamInt32,
            &CAENCHSelfTrgThresholdSelf);
    createParam("CH_TRG_POLARITY", asynParamInt32, &CAENCHTrgPolaritySelf);
    createParam("CH_PULSE_POLARITY", asynParamInt32, &CAENCHPulsePolaritySelf);
    createParam("CH_DC_OFFSET", asynParamInt32, &CAENCHDCOffsetSelf);
    createParam("CH_ZERRO_MODE", asynParamInt32, &CAENCHZerroModeSelf);
    createParam("CH_TEMP", asynParamInt32, &CAENCHTempSelf);

    createParam("READ_DATA", asynParamInt16Array, &GTRReadDataSelf);

    map_init(mapCAENErrorCodes)(CAEN_DGTZ_Success,
            "Operation completed successfully")(CAEN_DGTZ_CommError,
            "Communication error")(CAEN_DGTZ_GenericError, "Unspecified error")(
            CAEN_DGTZ_InvalidParam, "Invalid parameter")(
            CAEN_DGTZ_InvalidLinkType, "Invalid Link Type")(
            CAEN_DGTZ_InvalidHandle, "Invalid device handler")(
            CAEN_DGTZ_MaxDevicesError, "Maximum number of devices exceeded")(
            CAEN_DGTZ_BadBoardType,
            "Operation not allowed on this type of board")(
            CAEN_DGTZ_BadInterruptLev, "The interrupt level is not allowed")(
            CAEN_DGTZ_BadEventNumber, "The event number is bad")(
            CAEN_DGTZ_ReadDeviceRegisterFail, "Unable to read the registry")(
            CAEN_DGTZ_WriteDeviceRegisterFail,
            "Unable to write into the registry")(CAEN_DGTZ_InvalidChannelNumber,
            "The Channel is busy")(CAEN_DGTZ_ChannelBusy,
            "The channel number is invalid")(CAEN_DGTZ_FPIOModeInvalid,
            "Invalid FPIO Mode")(CAEN_DGTZ_WrongAcqMode,
            "Wrong acquisition mode")(CAEN_DGTZ_FunctionNotAllowed,
            "This function is not allowed for this module")(CAEN_DGTZ_Timeout,
            "Communication Timeout")(CAEN_DGTZ_InvalidBuffer,
            "The buffer is invalid")(CAEN_DGTZ_EventNotFound,
            "The event is not found")(CAEN_DGTZ_InvalidEvent,
            "The event is invalid")(CAEN_DGTZ_OutOfMemory, "Out of memory")(
            CAEN_DGTZ_CalibrationError, "Unable to calibrate the board")(
            CAEN_DGTZ_DigitizerNotFound, "Unable to open the digitizer")(
            CAEN_DGTZ_DigitizerAlreadyOpen, "The Digitizer is already open")(
            CAEN_DGTZ_DigitizerNotReady,
            "The Digitizer is not ready to operate")(
            CAEN_DGTZ_InterruptNotConfigured,
            "The Digitizer has not the IRQ configured")(
            CAEN_DGTZ_DigitizerMemoryCorrupted,
            "The digitizer flash memory is corrupted")(
            CAEN_DGTZ_DPPFirmwareNotSupported,
            "The digitizer DPP firmware is not supported in this lib version")(
            CAEN_DGTZ_InvalidLicense, "Invalid Firmware License")(
            CAEN_DGTZ_InvalidDigitizerStatus,
            "The digitizer is found in a corrupted status")(
            CAEN_DGTZ_UnsupportedTrace,
            "The given trace is not supported by the digitizer")(
            CAEN_DGTZ_InvalidProbe,
            "The given probe is not supported for the given digitizer's")(
            CAEN_DGTZ_NotYetImplemented, "The function is not yet implemented");

//	if ( CAEN_DGTZ_Success == openDigitizer() )
//	{
//		updateDeviceInfo();
//		updateStatus();
//		/* Update any changed parameters */
//		callParamCallbacks();
//	}else
//	{
//		// PROBLEM!!!
//		cantProceed("Can't open device! See log for details.");
//	}

}

CAENDriver::~CAENDriver()
{
    freeMemory();
    closeDigitizer();
}

asynStatus CAENDriver::drvUserCreate(asynUser *pasynUser, const char *drvInfo,
        const char **pptypeName, size_t *psize)
{
    asynStatus stat = GTRBaseDriver::drvUserCreate(pasynUser, drvInfo,
            pptypeName, psize);

    return stat;
}

CAEN_DGTZ_ErrorCode CAENDriver::openDigitizer()
{
    CAEN_DGTZ_ErrorCode ret = CAEN_DGTZ_OpenDigitizer(linkType, linkNum,
            conetNode, vmeBaseAddress, &devHandle);

    checkCAENResult(ret, "OpenDigitizer");
    return ret;
}

void CAENDriver::closeDigitizer()
{
    if (0 != devHandle)
    {
        CAEN_DGTZ_ErrorCode ret = CAEN_DGTZ_CloseDigitizer(devHandle);
        checkCAENResult(ret, "closeDigitizer");
    }
}

CAEN_DGTZ_ErrorCode CAENDriver::writeRegister(uint32_t address, uint32_t data)
{
    return CAEN_DGTZ_WriteRegister(devHandle, address, data);
}

CAEN_DGTZ_ErrorCode CAENDriver::readRegister(uint32_t address, uint32_t *data)
{
    return CAEN_DGTZ_ReadRegister(devHandle, address, data);
}

asynStatus CAENDriver::reset()
{
    CAEN_DGTZ_ErrorCode ret = CAEN_DGTZ_Reset(devHandle);
    return checkCAENResult(ret, "Reset");
}

void CAENDriver::updateDeviceInfo()
{
    CAEN_DGTZ_ErrorCode ret;
    CAEN_DGTZ_BoardInfo_t boardInfo;

    ret = CAEN_DGTZ_GetInfo(devHandle, &boardInfo);

    channelsNum = boardInfo.Channels;

    if (ret == 0)
    {
        setIntegerParam(CAENChannelNumSelf, boardInfo.Channels);
        setIntegerParam(CAENFormFactorSelf, boardInfo.FormFactor);
        setIntegerParam(CAENFamilySelf, boardInfo.FamilyCode);
        setIntegerParam(CAENSerialNumSelf, boardInfo.SerialNumber);
        setIntegerParam(CAENPCBRevSelf, boardInfo.PCB_Revision);
        setIntegerParam(CAENADCBitsSelf, boardInfo.ADC_NBits);

        setStringParam(CAENManufSelf, "CAEN");
        setStringParam(CAENModelSelf, boardInfo.ModelName);
        setStringParam(CAENROCFWSelf, boardInfo.ROC_FirmwareRel);
        setStringParam(CAENAMCFWSelf, boardInfo.AMC_FirmwareRel);
        setStringParam(CAENLicenseSelf, boardInfo.License);
    }
    else
    {
        checkCAENResult(ret, "GetInfo");
    }
}

asynStatus CAENDriver::readChannelTemperature(int ch)
{
    uint32_t temp;
    CAEN_DGTZ_ErrorCode ret = CAEN_DGTZ_ReadTemperature(devHandle,
            (uint32_t) ch, &temp);

    setIntegerParam(ch, CAENSWTrgModeSelf, (epicsUInt32)(temp));
    return checkCAENResult(ret, "ReadTemperature");
}

asynStatus CAENDriver::calibrate()
{
    CAEN_DGTZ_ErrorCode ret = CAENDGTZ_API
    CAEN_DGTZ_Calibrate(devHandle);
    return checkCAENResult(ret, "Calibrate");
}

asynStatus CAENDriver::sendSWtrigger()
{
    CAEN_DGTZ_ErrorCode ret = CAEN_DGTZ_SendSWtrigger(devHandle);
    return checkCAENResult(ret, "SendSWtrigger");
}

asynStatus CAENDriver::setSWTriggerMode(CAEN_DGTZ_TriggerMode_t mode)
{
    CAEN_DGTZ_ErrorCode ret = CAEN_DGTZ_SetSWTriggerMode(devHandle, mode);
    return checkCAENResult(ret, "SetSWTriggerMode");
}

asynStatus CAENDriver::getSWTriggerMode()
{
    CAEN_DGTZ_TriggerMode_t mode;
    CAEN_DGTZ_ErrorCode ret = CAEN_DGTZ_GetSWTriggerMode(devHandle, &mode);

    setIntegerParam(CAENSWTrgModeSelf, (epicsInt32)(mode));

    return checkCAENResult(ret, "GetSWTriggerMode");
}

asynStatus CAENDriver::setExtTriggerInputMode(CAEN_DGTZ_TriggerMode_t mode)
{
    CAEN_DGTZ_ErrorCode ret = CAEN_DGTZ_SetExtTriggerInputMode(devHandle, mode);

    return checkCAENResult(ret, "SetExtTriggerInputMode");
}

asynStatus CAENDriver::getExtTriggerInputMode()
{
    CAEN_DGTZ_TriggerMode_t mode;
    CAEN_DGTZ_ErrorCode ret = CAEN_DGTZ_GetExtTriggerInputMode(devHandle,
            &mode);
    setIntegerParam(CAENExtTrgModeSelf, (epicsInt32)(mode));
    return checkCAENResult(ret, "GetExtTriggerInputMode");
}

asynStatus CAENDriver::setChannelTriggerThreshold(uint32_t channel,
        uint32_t value)
{
    CAEN_DGTZ_ErrorCode ret = CAEN_DGTZ_SetChannelTriggerThreshold(devHandle,
            channel, value);
    return checkCAENResult(ret, "SetChannelTriggerThreshold");
}

asynStatus CAENDriver::getChannelTriggerThreshold(uint32_t channel)
{
    uint32_t value;
    CAEN_DGTZ_ErrorCode ret = CAEN_DGTZ_GetChannelTriggerThreshold(devHandle,
            channel, &value);

    setIntegerParam(channel, CAENCHSelfTrgThresholdSelf, value);
    return checkCAENResult(ret, "GetChannelTriggerThreshold");
}

asynStatus CAENDriver::setChannelSelfTrigger(uint32_t channel,
        CAEN_DGTZ_TriggerMode_t mode)
{
    CAEN_DGTZ_ErrorCode ret = CAEN_DGTZ_NotYetImplemented;
//			CAEN_DGTZ_SetChannelSelfTrigger(devHandle,
//					mode,
//					channelmask
//			);
    return checkCAENResult(ret, "SetChannelSelfTrigger");
}

asynStatus CAENDriver::getChannelSelfTrigger(uint32_t channel)
{
    CAEN_DGTZ_TriggerMode_t mode;
    CAEN_DGTZ_ErrorCode ret = CAEN_DGTZ_GetChannelSelfTrigger(devHandle,
            channel, &mode);
    setIntegerParam(channel, CAENCHSelfTrgModeSelf, mode);
    return checkCAENResult(ret, "GetChannelSelfTrigger");
}

asynStatus CAENDriver::setChannelPulsePolarity(uint32_t channel,
        CAEN_DGTZ_PulsePolarity_t pol)
{
    CAEN_DGTZ_ErrorCode ret = CAEN_DGTZ_SetChannelPulsePolarity(devHandle,
            channel, pol);

    return checkCAENResult(ret, "SetChannelPulsePolarity");
}

asynStatus CAENDriver::getChannelPulsePolarity(uint32_t channel)
{
    CAEN_DGTZ_PulsePolarity_t pol;
    CAEN_DGTZ_ErrorCode ret = CAEN_DGTZ_GetChannelPulsePolarity(devHandle,
            channel, &pol);

    setIntegerParam(channel, CAENCHPulsePolaritySelf, pol);
    return checkCAENResult(ret, "GetChannelPulsePolarity");
}

asynStatus CAENDriver::setIOLevel(CAEN_DGTZ_IOLevel_t level)
{
    CAEN_DGTZ_ErrorCode ret = CAEN_DGTZ_SetIOLevel(devHandle, level);
    return checkCAENResult(ret, "SetIOLevel");
}

asynStatus CAENDriver::getIOLevel()
{
    CAEN_DGTZ_IOLevel_t level;

    CAEN_DGTZ_ErrorCode ret = CAEN_DGTZ_GetIOLevel(devHandle, &level);

    setIntegerParam(CAENIOLevelSelf, level);
    return checkCAENResult(ret, "GetIOLevel");
}

asynStatus CAENDriver::setTriggerPolarity(uint32_t channel,
        CAEN_DGTZ_TriggerPolarity_t polarity)
{
    CAEN_DGTZ_ErrorCode ret = CAEN_DGTZ_SetTriggerPolarity(devHandle, channel,
            polarity);

    return checkCAENResult(ret, "SetTriggerPolarity");
}

asynStatus CAENDriver::getTriggerPolarity(uint32_t channel)
{
    CAEN_DGTZ_TriggerPolarity_t polarity;
    CAEN_DGTZ_ErrorCode ret = CAEN_DGTZ_GetTriggerPolarity(devHandle, channel,
            &polarity);

    setIntegerParam(channel, CAENCHTrgPolaritySelf, polarity);
    return checkCAENResult(ret, "GetTriggerPolarity");
}

asynStatus CAENDriver::setChannelEnableMask(uint32_t mask)
{
    CAEN_DGTZ_ErrorCode ret = CAEN_DGTZ_SetChannelEnableMask(devHandle, mask);
    return checkCAENResult(ret, "SetChannelEnableMask");
}

asynStatus CAENDriver::getChannelEnableMask(uint32_t *mask)
{
    CAEN_DGTZ_ErrorCode ret = CAEN_DGTZ_GetChannelEnableMask(devHandle, mask);
    return checkCAENResult(ret, "GetChannelEnableMask");
}

void CAENDriver::getChannelEnableStatus()
{
    uint32_t enableMask = 0, mask = 1;
    getChannelEnableMask(&enableMask);

    for (uint32_t idx = 0; channelsNum; ++idx)
    {
        if ((enableMask & mask) == mask)
        {
            setIntegerParam(idx, CAENCHEnabledSelf, 1);
        }
        else
        {
            setIntegerParam(idx, CAENCHEnabledSelf, 0);
        }
        mask = mask << 1;
    }
}

asynStatus CAENDriver::startAcquisition()
{
    char driverName[] = "CAEN";
    char functionName[] = "startAcquisition";

    if (acquisitionStarted)
    {
        asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR,
                "%s:%s: Acquisition has been already started.\n", driverName,
                functionName);

        return asynError;
    }

    callParamCallbacks ();

#ifndef TEST_READOUT
//	char driverName[]="CAEN";
//	char functionName[]="startAcquisition";
//
//	CAEN_DGTZ_ErrorCode ret =
//			CAEN_DGTZ_SWStartAcquisition(devHandle);
//
//	if (ret)
//	{
//		return checkCAENResult(ret, "SWStartAcquisition");
//	}
//
//	if (asynSuccess != allocateMemory())
//	{
//		asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR,
//			  "%s:%s: unable to allocate required memory.\n",
//			  driverName, functionName);
//		return asynError;
//	}
#endif

    createThreads();

    acquisitionStarted = true;
    return asynSuccess;
}

asynStatus CAENDriver::stopAcquisition()
{
    char driverName[] = "CAEN";
    char functionName[] = "startAcquisition";

    if (!acquisitionStarted)
    {
        asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR,
                "%s:%s: Acquisition has NOT been started yet.\n", driverName,
                functionName);

        return asynError;
    }

    evtProcessingData.signal();
    evtProcessingNDBuffers.signal();

    CAEN_DGTZ_ErrorCode ret = CAEN_DGTZ_SWStopAcquisition(devHandle);

    acquisitionStarted = false;
    return checkCAENResult(ret, "SWStopAcquisition");
}

asynStatus CAENDriver::setRecordLength(uint32_t size)
{
    CAEN_DGTZ_ErrorCode ret = CAEN_DGTZ_SetRecordLength(devHandle, size);

    return checkCAENResult(ret, "SetRecordLength");
}

asynStatus CAENDriver::getRecordLength()
{
    uint32_t size;
    CAEN_DGTZ_ErrorCode ret = CAEN_DGTZ_GetRecordLength(devHandle, &size);

    return checkCAENResult(ret, "GetRecordLength");
}

asynStatus CAENDriver::setPostTriggerSize(uint32_t percent)
{
    CAEN_DGTZ_ErrorCode ret = CAEN_DGTZ_SetPostTriggerSize(devHandle, percent);

    return checkCAENResult(ret, "SetPostTriggerSize");
}

asynStatus CAENDriver::getPostTriggerSize()
{
    uint32_t percent;
    CAEN_DGTZ_ErrorCode ret = CAEN_DGTZ_GetPostTriggerSize(devHandle, &percent);

    return checkCAENResult(ret, "GetPostTriggerSize");
}

asynStatus CAENDriver::setChannelDCOffset(uint32_t channel, uint32_t value)
{
    CAEN_DGTZ_ErrorCode ret = CAEN_DGTZ_SetChannelDCOffset(devHandle, channel,
            value);

    return checkCAENResult(ret, "SetChannelDCOffset");
}

asynStatus CAENDriver::getChannelDCOffset(uint32_t channel)
{
    uint32_t value;
    CAEN_DGTZ_ErrorCode ret = CAEN_DGTZ_GetChannelDCOffset(devHandle, channel,
            &value);

    return checkCAENResult(ret, "GetChannelDCOffset");
}

asynStatus CAENDriver::setZeroSuppressionMode(CAEN_DGTZ_ZS_Mode_t mode)
{
    CAEN_DGTZ_ErrorCode ret = CAEN_DGTZ_SetZeroSuppressionMode(devHandle, mode);
    return checkCAENResult(ret, "SetZeroSuppressionMode");
}

asynStatus CAENDriver::getZeroSuppressionMod()
{
    CAEN_DGTZ_ZS_Mode_t mode;
    CAEN_DGTZ_ErrorCode ret = CAEN_DGTZ_GetZeroSuppressionMode(devHandle,
            &mode);
    return checkCAENResult(ret, "GetZeroSuppressionMode");
}

asynStatus CAENDriver::setChannelZSParams(uint32_t channel,
        CAEN_DGTZ_ThresholdWeight_t weight, int32_t threshold, int32_t nsamp)
{
    CAEN_DGTZ_ErrorCode ret = CAEN_DGTZ_SetChannelZSParams(devHandle, channel,
            weight, threshold, nsamp);

    return checkCAENResult(ret, "SetChannelZSParams");
}

asynStatus CAENDriver::getChannelZSParams(uint32_t channel,
        CAEN_DGTZ_ThresholdWeight_t *weight, int32_t *threshold, int32_t *nsamp)
{
    CAEN_DGTZ_ErrorCode ret = CAEN_DGTZ_GetChannelZSParams(devHandle, channel,
            weight, threshold, nsamp);

    return checkCAENResult(ret, "GetChannelZSParams");
}

asynStatus CAENDriver::checkCAENResult(CAEN_DGTZ_ErrorCode code,
        const char* funcName)
{
    setIntegerParam(CAENLastResultCodeSelf, code);
    if (0 == code)
    {
        setStringParam(CAENLastResultSelf, "SUCCESS");
        return asynSuccess;
    }

    MapCAENErrorCodes::iterator it = mapCAENErrorCodes.find(code);
    if (it != mapCAENErrorCodes.end())
    {
        setStringParam(CAENLastResultSelf, it->second);
        asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR, "CAEN: %s: %s.\n",
                funcName, it->second);
    }
    else
    {
        setStringParam(CAENLastResultSelf, "UNKNOWN error.");
        asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR,
                "CAEN: %s: UNKNOWN error.\n", funcName);
    }

    /* Update any changed parameters */
    callParamCallbacks();

    return asynError;
}

asynStatus CAENDriver::writeInt32(asynUser *pasynUser, epicsInt32 value)
{
    char driverName[] = "CAEN";
    char functionName[] = "writeInt32";

#ifndef TEST_READOUT
    if (0==devHandle)
    {
        asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR,
                "%s:%s: Device handle is NULL.\n",
                driverName, functionName);

        return asynError;
    }
#endif

    int reason = pasynUser->reason;
    int channel;
    pasynManager->getAddr(pasynUser, &channel);

    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW,
            "%s:%s: Processing reason:%d addr:%d \n",
            driverName, functionName, reason, channel);

    // Device
    if (reason == CAENSWTrgModeSelf)
    {
        return setSWTriggerMode((CAEN_DGTZ_TriggerMode_t) value);
    }
    else if (reason == CAENExtTrgModeSelf)
    {
        return setExtTriggerInputMode((CAEN_DGTZ_TriggerMode_t) value);
    }
    else if (reason == CAENIOLevelSelf)
    {
        return setIOLevel((CAEN_DGTZ_IOLevel_t) value);
    }
    else if (reason == CAENACQStartSelf)
    {
        return startAcquisition();
    }
    else if (reason == CAENACQStopSelf)
    {
        return stopAcquisition();
    }
    else if (reason == CAENRecordLengthSelf)
    {
        return setRecordLength(value);
    }
    else if (reason == CAENPostTrgLenSelf)
    {
        return setPostTriggerSize(value);
    }
    else if (reason == CAENACQModeSelf)
    {

    }
    else if (reason == CAENCalibrateSelf)
    {
        return calibrate();
    }
    else if (reason == GTRSWTrgSelf)
    {
        return sendSWtrigger();
    }
    else
    {
        return GTRBaseDriver::writeInt32(pasynUser, value);
    }
    // Channel
    if (reason == CAENCHEnabledSelf)
    {
    }
    else if (reason == CAENCHSelfTrgModeSelf)
    {
        return setChannelSelfTrigger(channel, (CAEN_DGTZ_TriggerMode_t) value);
    }
    else if (reason == CAENCHSelfTrgThresholdSelf)
    {
        return setChannelTriggerThreshold(channel, value);
    }
    else if (reason == CAENCHTrgPolaritySelf)
    {

    }
    else if (reason == CAENCHPulsePolaritySelf)
    {
        return setChannelPulsePolarity(channel,
                (CAEN_DGTZ_PulsePolarity_t) value);
    }
    else if (reason == CAENCHDCOffsetSelf)
    {
        return setChannelDCOffset(channel, value);
    }
    else if (reason == CAENCHZerroModeSelf)
    {
        //return setChannelZSParams(channel, value);
    }
    else
    {
        return GTRBaseDriver::writeInt32(pasynUser, value);
    }

    asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR,
            "%s:%s: Functionality is not implemented (address: %d, reason %d).\n",
            driverName, functionName, channel, reason);

    return asynError;
}

asynStatus CAENDriver::readInt32(asynUser *pasynUser, epicsInt32 *value)
{
    if (0 == devHandle)
        return asynError;

    int reason = pasynUser->reason;
    int channel;
    pasynManager->getAddr(pasynUser, &channel);

    if (reason == CAENCHTempSelf)
    {
        return readChannelTemperature(channel);
    }
    return asynError;
}

asynStatus CAENDriver::allocateMemory()
{
    CAEN_DGTZ_ErrorCode ret = CAEN_DGTZ_MallocReadoutBuffer(devHandle,
            &bufferSelf, &bufferSize);

    checkCAENResult(ret, "MallocReadoutBuffer");

    if (0 != ret)
        return asynError;

    return allocateEvent();
}

asynStatus CAENDriver::allocateEvent()
{
    CAEN_DGTZ_ErrorCode ret = CAEN_DGTZ_AllocateEvent(devHandle, &event);
    return checkCAENResult(ret, "AllocateEvent");
}

void CAENDriver::freeMemory()
{
    CAEN_DGTZ_ErrorCode ret;
    if (0 != bufferSelf)
    {
        ret = CAEN_DGTZ_FreeReadoutBuffer(&bufferSelf);
        checkCAENResult(ret, "FreeReadoutBuffer");
    }

    freeEvent();
}

void CAENDriver::freeEvent()
{
    if (0 != event)
    {
        CAEN_DGTZ_ErrorCode ret = CAEN_DGTZ_FreeEvent(devHandle, &event);
        checkCAENResult(ret, "FreeEvent");
    }
}

void CAENDriver::updateStatus()
{
    updateBoardStatus();
    for (int32_t channel = 0; channelsNum; ++channel)
    {
        updateChannelStatus(channel);
    }
}

void CAENDriver::updateChannelStatus(uint32_t channel)
{
    readChannelTemperature(channel);

    getChannelTriggerThreshold(channel);
    getChannelSelfTrigger(channel);
    getChannelPulsePolarity(channel);
    getTriggerPolarity(channel);
    getChannelDCOffset(channel);

}

void CAENDriver::updateBoardStatus()
{
    getSWTriggerMode();
    getExtTriggerInputMode();
    getIOLevel();
    getRecordLength();
    getPostTriggerSize();
    getZeroSuppressionMod();

    getChannelEnableStatus();
}

void CAENDriver::readData()
{
    //	CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT = 0,
    //	CAEN_DGTZ_SLAVE_TERMINATED_READOUT_2eVME = 1,
    //	CAEN_DGTZ_SLAVE_TERMINATED_READOUT_2eSST = 2,
    //	CAEN_DGTZ_POLLING_MBLT  = 3,
    //	CAEN_DGTZ_POLLING_2eVME  = 4,
    //	CAEN_DGTZ_POLLING_2eSST  = 5,

    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: Started\n", "CAEN",
            "readData");

    int ndims = 1;
    size_t dims[] =
    { bufferSize };

    uint32_t readDataSize = 0;

    while (!evtProcessingData.wait(THREAD_TIMEOUT))
    {
        CAEN_DGTZ_ErrorCode ret = CAEN_DGTZ_ReadData(devHandle,
                CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT, bufferSelf,
                &readDataSize);

        if (readDataSize == 0)
            continue;

        uint32_t numEvents;
        ret = CAEN_DGTZ_GetNumEvents(devHandle, bufferSelf, readDataSize,
                &numEvents);

        for (uint32_t i = 0; i < numEvents; ++i)
        {
            if (asynSuccess != parseEvent(bufferSelf, readDataSize, i))
            {
                asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR,
                        "%s:%s: Event (%d) parsing has failed\n", "CAEN",
                        "readData", i);
            }
        }

        uint32_t lstatus;
        ret = CAEN_DGTZ_ReadRegister(devHandle, CAEN_DGTZ_ACQ_STATUS_ADD,
                &lstatus);
        if (ret || lstatus & (0x1 << 19))
        {
            // stop processing
            asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR,
                    "%s:%s: HW is over heated! Stopping acquisition...\n",
                    "CAEN", "readData");
            break;
        }
    }

    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: Finished\n", "CAEN",
            "readData");
}

// Parsing event according to digitizer type
// Could be overwritten in specific digitizer implementation
asynStatus CAENDriver::parseEvent(char *buffer, uint32_t bufferSize,
        uint32_t eventNum)
{
    char driverName[] = "CAEN";
    char functionName[] = "parseEvent";

    char *eventPtr = NULL;
    CAEN_DGTZ_EventInfo_t eventInfo;
    CAEN_DGTZ_UINT16_EVENT_t *event16 = NULL;

    /// Read the event
    CAEN_DGTZ_ErrorCode ret = CAEN_DGTZ_GetEventInfo(devHandle, buffer,
            bufferSize, eventNum, &eventInfo, &eventPtr);
    if (ret)
    {
        return checkCAENResult(ret, functionName);
    }

    /// Decoding the event, obtaining data.
    ret = CAEN_DGTZ_DecodeEvent(devHandle, eventPtr, (void**) &event16);
    if (ret)
    {
        return checkCAENResult(ret, functionName);
    }

    for (int ch = 0; ch < this->maxAddr; ch++)
    {
        // channel is not enabled
        if (!(eventInfo.ChannelMask & (1 << ch)))
        {
            continue;
        }

        processChannel(event16, ch);
    }
    return asynSuccess;
}

void CAENDriver::processChannel(CAEN_DGTZ_UINT16_EVENT_t *event16, int channel)
{
    char driverName[] = "CAEN";
    char functionName[] = "processChannel";

    ///Updating GTR waveform
    asynStatus stat = doCallbacksInt16Array(
            (epicsInt16*) event16->DataChannel[channel],
            (size_t) event16->ChSize[channel], GTRReadDataSelf, channel);
    if (stat)
    {
        asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR,
                "%s:%s: Error to update waveform for GTR (%d).\n", driverName,
                functionName, stat);
    }

    // Put data to the NDArrayPool.
    int ndims = 1;
    size_t dims[] =
    { (int) event16->ChSize[channel] };

    // Buffer allocation and reservation
    NDArray *ndBuffer = 0;
    ndBuffer = this->pNDArrayPool->alloc(ndims, dims, NDInt16,
            (size_t) sizeof(uint16_t) * event16->ChSize[channel], NULL);
    if (!ndBuffer)
    {
        asynPrint(this->pasynUserSelf, ASYN_TRACE_ERROR,
                "%s:%s: unable to allocate buffer from the pool.\n", driverName,
                functionName);
        return;
    }

    /// copying data
    memcpy(ndBuffer->pData, event16->DataChannel[channel],
            (int) event16->ChSize[channel]);

    /// setting the channel number
    ndBuffer->pAttributeList->add("Channel", "Channel number", NDAttrInt32,
            &channel);

    unprocessedBuffersMutex.lock();
    unprocessedBuffers.push(ndBuffer);
    unprocessedBuffersMutex.unlock();
}

void CAENDriver::testReadOut()
{
    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: Started\n", "CAEN",
            "testReadOut");

    size_t maxChannel = (this->maxAddr >= 4) ? 4 : this->maxAddr;
    CAEN_DGTZ_UINT16_EVENT_t event16;
    uint32_t samplesnumber = 360;

    for (size_t ch = 0; ch < maxChannel; ch++)
    {
        event16.ChSize[ch] = samplesnumber;
        event16.DataChannel[ch] = (uint16_t*) malloc(
                sizeof(uint16_t) * samplesnumber);

        for (uint32_t i = 0; i < samplesnumber; ++i)
        {
            event16.DataChannel[ch][i] = (uint16_t)(sin(M_PI * i / 360) * 100);
        }
    }

    while (!evtProcessingData.wait(2 * THREAD_TIMEOUT))
    {
        for (size_t ch = 0; ch < maxChannel; ch++)
        {
            processChannel(&event16, ch);
        }
    }

    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: Finished\n", "CAEN",
            "testReadOut");
}

///
/// Processing buffers from the queue.
///
void CAENDriver::processNDCallbacks()
{
    int arrayCallbacks;

    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: Starting...\n",
            "CAEN", "processNDCallbacks");

    while (!evtProcessingNDBuffers.wait(THREAD_TIMEOUT))
    {
        unprocessedBuffersMutex.lock();

        if (unprocessedBuffers.empty())
        {
            unprocessedBuffersMutex.unlock();
            continue;
        }

        NDArray* buffer = unprocessedBuffers.front();
        unprocessedBuffers.pop();
        unprocessedBuffersMutex.unlock();

        if (buffer)
        {
            /// Get channel address for the buffer.
            NDAttribute *attrAddress = buffer->pAttributeList->find("Channel");
            /// Channel attribute
            if (0 == attrAddress)
            {
                buffer->release();
                continue;
            }

            int address;
            /// Get the channel address
            int ret = attrAddress->getValue(NDAttrInt32, &address, 0);

//		this->lock();
//	    getIntegerParam(NDArrayCallbacks, &arrayCallbacks);
//	    this->unlock();
//	    if (arrayCallbacks)
            {
                /// Call the NDArray callback
                asynStatus stat = doCallbacksGenericPointer(buffer, NDArrayData,
                        address);
            }
            buffer->release();
        }
    }

    asynPrint(this->pasynUserSelf, ASYN_TRACE_FLOW, "%s:%s: Finishing...\n",
            "CAEN", "processNDCallbacks");
}

void CAENDriver::createThreads()
{
#ifdef TEST_READOUT
    thrDataReadout = epicsThreadCreate("DataReadout", epicsThreadPriorityMedium,
            epicsThreadGetStackSize(epicsThreadStackMedium),
            (EPICSTHREADFUNC) testReadOutC, this);
#else
    thrDataReadout = epicsThreadCreate("DataReadout",
            epicsThreadPriorityMedium,
            epicsThreadGetStackSize(epicsThreadStackMedium),
            (EPICSTHREADFUNC)readoutDataC,
            this);
#endif

    thrNDBufferProcessing = epicsThreadCreate("NDBufferProcessing",
            epicsThreadPriorityMedium,
            epicsThreadGetStackSize(epicsThreadStackMedium),
            (EPICSTHREADFUNC) processNDBufferC, this);
}

