#pragma once

#include <thread>
#include <mutex>
#include <atomic>
#include <iunoplugincontroller.h>
#include <iunoplugin.h>
#include <iunostreamobserver.h>
#include <iunoaudioobserver.h>
#include <iunoaudioprocessor.h>
#include <iunostreamobserver.h>
#include <iunoannotator.h>

#include "SDRunoPlugin_RemoteBridgeUi.h"
#include "SDRunoPlugin_RemoteBridgeSerial.h"

#define DEBUG true;

class SDRunoPlugin_RemoteBridge : public IUnoPlugin
{

public:
	
	SDRunoPlugin_RemoteBridge(IUnoPluginController& controller);
	virtual ~SDRunoPlugin_RemoteBridge();
	void StartBridge(std::string addr);
	void StopBridge();
	bool IsRunning() { return m_started; }
	// TODO: change the plugin title here
	virtual const char* GetPluginName() const override { return "SDRuno Plugin Example"; }

	void UpdateSampleRate();
	//controller
	void UpdateFrequencies();

	//@todo: process me
	/*
	Lots of handlers for events
	*/

	// IUnoPlugin
	virtual void HandleEvent(const UnoEvent& ev) override;

	struct State {
		int8_t      Demodulator				= 0;//Demodulator type,        Enum:
											//    DemodulatorNone = 0,
											//    DemodulatorAM = 1,
											//    DemodulatorSAM = 2,
											//    DemodulatorNFM = 3,
											//    DemodulatorMFM = 4,
											//    DemodulatorWFM = 5,
											//    DemodulatorSWFM = 6,
											//    DemodulatorDSB = 7,
											//    DemodulatorLSB = 8,
											//    DemodulatorUSB = 9,
											//    DemodulatorCW = 10,
											//    DemodulatorDigital = 11,
											//    DemodulatorDAB = 12,
											//    DemodulatorIQOUT = 13
		int8_t      WfmDeemphasisMode		= 0;
											//    DeemphasisNone = 0,
											//    Deemphasis50us = 1,
											//    Deemphasis75us = 2,
		int8_t      NoiseBlankerMode		= 0;
											//    NoiseBlankerOff = 0,
											//    NoiseBlankerBwWide = 1,
											//    NoiseBlankerBwNarrow = 2
		int8_t      AgcMode					= 0;
											//    AGCModeOff = 0,
											//    AGCModeSlow = 1,
											//    AGCModeMedium = 2,
											//    AGCModeFast = 3
		int8_t      AgcThreshold			= 0;
		int8_t      NoiseBlankerLevel		= 0;
		int8_t      NoiseReductionLevel		= 0;
		int8_t      CwPeakFilterThreshold	= 0;
		int         AudioVolume				= 0; //220 is max of the bar value
		int8_t      SP1MinPower				= 0;
		uint32_t	VfoFrequency			= 0; //VFO Freq,                Hz
		uint32_t	CenterFrequency			= 0;
		uint32_t	SP1MaxFrequency			= 0;
		uint32_t	SP1MinFrequency			= 0;
		long int    MPXLevel				= 0;
		int         FilterBandwidth			= 0; //Filter bandwith,         Hz
		int         SquelchLevel			= 0; //220 is max of the bar value
		int         SquelchEnable			= 0; //boolean val
		int         FmNoiseReductionEnable	= 0; //boolean val
		int         AudioMute				= 0; //boolean val
		int         BiasTEnable				= 0; //boolean val
		long int    fingerprint;
	} state;

private:
	void WorkerFunction();
	bool m_started;
	bool isConnected = false;
	bool stateChanged = false;
	Serial Serial;
	std::thread* m_worker;
	std::mutex m_lock;
	std::string cPort;
	std::string error;
	SDRunoPlugin_RemoteBridgeUi m_form;
	double sampleRate;


};