/*
 * caenDriver.h
 *
 *  Created on: 25. mar. 2016
 *      Author: sisaev
 */

#ifndef CAENDRIVER_H_
#define CAENDRIVER_H_

#include <cstdint>
#include <map>
#include <queue>

#include <epicsTypes.h>
#include <epicsMutex.h>
#include <epicsEvent.h>
#include <epicsThread.h>

#include <CAENDigitizer.h>
#include <CAENDigitizerType.h>

#include <gtrBaseDriver.h>

#define NUM_CAEN_PARAMS (&LAST_CAEN_PARAM - &FIRST_CAEN_PARAM + 1)

typedef std::map<int, const char*> MapCAENErrorCodes;

class CAENDriver: public GTRBaseDriver
{
public:
	CAENDriver(const char *portName
			,int aLinkType
			,int aLinkNum
			,int aConetNode
			,int aVmeBaseAddress
			,int maxAddr, int numParams, int maxBuffers, size_t maxMemory,
	        int interfaceMask, int interruptMask,
	        int asynFlags, int autoConnect, int priority, int stackSize);

	virtual ~CAENDriver();

	asynStatus writeInt32 (asynUser *pasynUser, epicsInt32 value);
	asynStatus readInt32 (asynUser *pasynUser, epicsInt32 *value);

	/// Thread body to read data from digitizer
	/// It provides base handling for the CAEN digitizer
	void readData();

	/// Handles received buffer pool.
	/// Propagates data to the plugins.
	void processNDCallbacks();

	void testReadOut();

	virtual asynStatus drvUserCreate (asynUser *pasynUser, const char *drvInfo, const char **pptypeName, size_t *psize);

protected:
	// Parameters asyn ID's;
	epicsInt32 CAENManufSelf;
	#define FIRST_CAEN_PARAM CAENManufSelf

	epicsInt32 CAENModelSelf;
	epicsInt32 CAENChannelNumSelf;
	epicsInt32 CAENFormFactorSelf;
	epicsInt32 CAENFamilySelf;
	epicsInt32 CAENROCFWSelf;
	epicsInt32 CAENAMCFWSelf;
	epicsInt32 CAENSerialNumSelf;
	epicsInt32 CAENPCBRevSelf;
	epicsInt32 CAENADCBitsSelf;
	epicsInt32 CAENLicenseSelf;
	epicsInt32 CAENSWTrgModeSelf;
	epicsInt32 CAENExtTrgModeSelf;
	epicsInt32 CAENIOLevelSelf;
	epicsInt32 CAENACQStartSelf;
	epicsInt32 CAENACQStopSelf;
	epicsInt32 CAENRecordLengthSelf;
	epicsInt32 CAENPostTrgLenSelf;
	epicsInt32 CAENACQModeSelf;

	epicsInt32 CAENLastResultCodeSelf;
	epicsInt32 CAENLastResultSelf;
	epicsInt32 CAENCalibrateSelf;
	epicsInt32 CAENSerroSpMode;

	epicsInt32 CAENCHEnabledSelf;
	epicsInt32 CAENCHSelfTrgModeSelf;
	epicsInt32 CAENCHSelfTrgThresholdSelf;
	epicsInt32 CAENCHTrgPolaritySelf;
	epicsInt32 CAENCHPulsePolaritySelf;
	epicsInt32 CAENCHDCOffsetSelf;
	epicsInt32 CAENCHZerroModeSelf;
	epicsInt32 CAENCHTempSelf;
	#define LAST_CAEN_PARAM CAENCHTempSelf

	// CAEN specific
	CAEN_DGTZ_ConnectionType linkType;
	int linkNum;
	int conetNode;
	uint32_t vmeBaseAddress;

	int      devHandle;
	uint32_t bufferSize;
	char*    bufferSelf;


	size_t channelsNum;
	void*  event;

	epicsMutex unprocessedBuffersMutex;
	std::queue<NDArray*> unprocessedBuffers;

	epicsEvent evtProcessingNDBuffers;
	epicsEvent evtProcessingData;

	epicsThreadId thrNDBufferProcessing;
	epicsThreadId thrDataReadout;

	bool acquisitionStarted;

	asynStatus checkCAENResult(CAEN_DGTZ_ErrorCode code, const char*);

	CAEN_DGTZ_ErrorCode  writeRegister(uint32_t address, uint32_t data);
	CAEN_DGTZ_ErrorCode  readRegister (uint32_t  address, uint32_t *data);

	asynStatus calibrate();
	asynStatus sendSWtrigger();
	asynStatus setSWTriggerMode(CAEN_DGTZ_TriggerMode_t mode);
	asynStatus getSWTriggerMode();

	asynStatus setExtTriggerInputMode(CAEN_DGTZ_TriggerMode_t mode);
	asynStatus getExtTriggerInputMode();

	asynStatus setChannelTriggerThreshold( uint32_t channel, uint32_t value);
	asynStatus getChannelTriggerThreshold(uint32_t channel);

	asynStatus setChannelSelfTrigger(uint32_t channel, CAEN_DGTZ_TriggerMode_t mode);
	asynStatus getChannelSelfTrigger(uint32_t channel);

	asynStatus setChannelPulsePolarity(uint32_t channel, CAEN_DGTZ_PulsePolarity_t pol);
	asynStatus getChannelPulsePolarity(uint32_t channel);

	asynStatus setIOLevel(CAEN_DGTZ_IOLevel_t level);
	asynStatus getIOLevel ();

	asynStatus setTriggerPolarity (uint32_t channel, CAEN_DGTZ_TriggerPolarity_t polarity);
	asynStatus getTriggerPolarity (uint32_t channel);

	asynStatus setChannelEnableMask(uint32_t mask);
	asynStatus getChannelEnableMask(uint32_t *mask);

	asynStatus startAcquisition();
	asynStatus stopAcquisition();

	asynStatus setRecordLength (uint32_t size);
	asynStatus getRecordLength();

	asynStatus setPostTriggerSize(uint32_t percent);
	asynStatus getPostTriggerSize();

	asynStatus setChannelDCOffset(uint32_t channel, uint32_t value);
	asynStatus getChannelDCOffset(uint32_t channel);

	asynStatus setZeroSuppressionMode(CAEN_DGTZ_ZS_Mode_t mode);
	asynStatus getZeroSuppressionMod();

	asynStatus readChannelTemperature(int ch);

	asynStatus reset();

	asynStatus setChannelZSParams(uint32_t channel,
			CAEN_DGTZ_ThresholdWeight_t weight,
			int32_t threshold,
			int32_t nsamp);

	asynStatus getChannelZSParams(uint32_t channel,
			CAEN_DGTZ_ThresholdWeight_t *weight,
			int32_t *threshold,
			int32_t *nsamp);

	virtual CAEN_DGTZ_ErrorCode openDigitizer();
	virtual void closeDigitizer();

	virtual asynStatus allocateMemory();
	virtual void 	   freeMemory();

	virtual asynStatus allocateEvent();
	virtual void 	   freeEvent();

	virtual void updateStatus();
	virtual void updateBoardStatus();
	virtual void updateChannelStatus(uint32_t channel);
	void updateDeviceInfo();

	void getChannelEnableStatus();

	// Parsing event according to digitizer type.
	// Could be overwritten in specific digitizer implementation.
	// Current implementation covers common 16bit digitizers.
	asynStatus parseEvent (char *buffer, uint32_t BufferSize, uint32_t eventNum );

	/// Processing parsed event.
	/// Extrac data, update relevant PVs, put data into the queue.
	void processChannel(CAEN_DGTZ_UINT16_EVENT_t *event16, int channel);

	/// Create threads
	void createThreads();
private:
	CAENDriver(const CAENDriver&);
	const CAENDriver& operator = (const CAENDriver&);

};



#endif /* CAENDRIVER_H_ */
