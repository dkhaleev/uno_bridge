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
	//@todo: process me
	/*
	Lots of handlers for events
	*/

	// IUnoPlugin
	virtual void HandleEvent(const UnoEvent& ev) override;

	struct State {
		int8_t      DT = 0;				//Demodulator type,        Enum:
										//0 - 
										//1 -
										//2 -
										//3 -
										//...
										//x -
		long int    VFOFreq = 0;		//VFO Freq,                Hz
		long int    CFreq = 0;			//Center Frequency,        Hz
		int         FB = 0;				//Filter bandwith,         Hz
		long int	fingerprint;		//random fingerprint. 
	} state;

private:
	void WorkerFunction();
	bool m_started;
	bool isConnected = false;
	Serial Serial;
	std::thread* m_worker;
	std::mutex m_lock;
	std::string cPort;
	std::string error;
	SDRunoPlugin_RemoteBridgeUi m_form;
	int sampleRate;
};