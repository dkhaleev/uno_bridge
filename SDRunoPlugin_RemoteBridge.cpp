#include <sstream>
#include <unoevent.h>
#include <iunoplugincontroller.h>
#include <vector>
#include <string>
#include <sstream>
#include <chrono>
#include <Windows.h>

#include "SDRunoPlugin_RemoteBridge.h"
#include "SDRunoPlugin_RemoteBridgeUi.h"

const char* PreFormatString(const char * zcFormat, ...) 
{
	// initialize use of the variable argument array
	va_list vaArgs;
	va_start(vaArgs, zcFormat);

	// reliably acquire the size
	// from a copy of the variable argument array
	// and a functionally reliable call to mock the formatting
	va_list vaArgsCopy;
	va_copy(vaArgsCopy, vaArgs);
	const int iLen = std::vsnprintf(NULL, 0, zcFormat, vaArgsCopy);
	va_end(vaArgsCopy);

	// return a formatted string without risking memory mismanagement
	// and without assuming any compiler or platform specific behavior
	std::vector<char> zc(iLen + 1);
	std::vsnprintf(zc.data(), zc.size(), zcFormat, vaArgs);
	va_end(vaArgs);
	std::string strText(zc.data(), iLen);

	const char* c = strText.c_str();
	return c;
}

void DbgMsg(const char * zcFormat, ...)
{
	// initialize use of the variable argument array
	va_list vaArgs;
	va_start(vaArgs, zcFormat);

	// reliably acquire the size
	// from a copy of the variable argument array
	// and a functionally reliable call to mock the formatting
	va_list vaArgsCopy;
	va_copy(vaArgsCopy, vaArgs);
	const int iLen = std::vsnprintf(NULL, 0, zcFormat, vaArgsCopy);
	va_end(vaArgsCopy);

	// return a formatted string without risking memory mismanagement
	// and without assuming any compiler or platform specific behavior
	std::vector<char> zc(iLen + 1);
	std::vsnprintf(zc.data(), zc.size(), zcFormat, vaArgs);
	va_end(vaArgs);
	std::string strText(zc.data(), iLen);

	OutputDebugStringA(strText.c_str());
}

SDRunoPlugin_RemoteBridge::SDRunoPlugin_RemoteBridge(IUnoPluginController& controller) :
	IUnoPlugin(controller),
	m_form(*this, controller),
	m_worker(nullptr),
	m_started(false)

{
}

SDRunoPlugin_RemoteBridge::~SDRunoPlugin_RemoteBridge()
{	
}

void SDRunoPlugin_RemoteBridge::HandleEvent(const UnoEvent& ev)
{
	m_form.HandleEvent(ev);	
}

void SDRunoPlugin_RemoteBridge::WorkerFunction()
{
	
	int cycles = 0;
	byte crc;
	if (isConnected) 
	{
#ifdef DEBUG
		OutputDebugStringA("Bridge worker is connected to port \r\n");
#endif
		while (m_started) {
			cycles ++;

			if (isConnected)
			{
				if (Serial.peekReceiver())
				{
					error = "receiver peeked successfully";
					char buffer[300];
					Serial.readStringNoTimeOut(buffer, '\n', 300);					
					sscanf(buffer, 
						"%d %d %d %d %d %d %d %d %d %d %lu %lu %lu %lu %lu %lu %d %d %b %b %b %b %lu", 
						&state.Demodulator, 
						&state.WfmDeemphasisMode,
						&state.NoiseBlankerMode,
						&state.AgcMode,
						&state.AgcThreshold,
						&state.NoiseBlankerLevel,
						&state.NoiseReductionLevel,
						&state.CwPeakFilterThreshold,
						&state.AudioVolume,
						&state.SP1MinPower,
						&state.VfoFrequency,
						&state.CenterFrequency,
						&state.SP1MaxFrequency,
						&state.SP1MinFrequency,
						&state.MPXLevel,
						&state.FilterBandwidth,
						&state.SquelchLevel,
						&state.SquelchEnable,
						&state.FmNoiseReductionEnable,
						&state.AudioMute,
						&state.BiasTEnable,
						&state.fingerprint
					);
					
					crc = Serial.crc8((byte*)&state, sizeof(state)); // read crc frame complete
					
					if (crc == state.fingerprint) {
#ifdef DEBUG
						OutputDebugStringA("CRC Check passed \r\n");						
#endif //DEBUG
					} else {
						// data corrupted
#ifdef DEBUG
						OutputDebugStringA("CRC Check failed \r\n");
#endif // DEBUG
					}

					DbgMsg("Demodulator: %hu \t VFO: %lu \t CenterFrequency: %lu \t FilterBandwidth: %hu \t CRC: %lu \t CCRC: %lu\r\n ",
						state.Demodulator,
						state.VfoFrequency,
						state.CenterFrequency,
						state.FilterBandwidth,
						state.fingerprint, crc);
					//m_controller.SetVfoFrequency(0, state.VfoFrequency);					
					//m_controller.SetDemodulatorType(0, (IUnoPluginController::DemodulatorType)(state.Demodulator));
						//&state.WfmDeemphasisMode,
						//&state.NoiseBlankerMode,
						//&state.AgcMode,
						//&state.AgcThreshold,
						//&state.NoiseBlankerLevel,
						//&state.NoiseReductionLevel,
						//&state.CwPeakFilterThreshold,
					m_controller.SetAudioVolume(0, state.AudioVolume);
						//&state.SP1MinPower,
						//&state.VfoFrequency,
					m_controller.SetCenterFrequency(0, state.CenterFrequency);
						//&state.SP1MaxFrequency,
						//&state.SP1MinFrequency,
						//&state.MPXLevel,
						//&state.FilterBandwidth
					m_controller.SetSquelchLevel(0, state.SquelchLevel);
					m_controller.SetSquelchEnable(0, state.SquelchEnable);
						//&state.FmNoiseReductionEnable,
					m_controller.SetAudioMute(0, state.AudioMute);
						//&state.BiasTEnable,
						//&state.fingerprint
				}
				else
				{
					error = "Receiver not peeked";
				}
				if (stateChanged) {
					char output_buffer[300];
					sprintf(output_buffer,
						"%d %d %d %d %d %d %d %d %d %d %lu %lu %lu %lu %lu %lu %d %d %b %b %b %b %lu",
						&state.Demodulator,
						&state.WfmDeemphasisMode,
						&state.NoiseBlankerMode,
						&state.AgcMode,
						&state.AgcThreshold,
						&state.NoiseBlankerLevel,
						&state.NoiseReductionLevel,
						&state.CwPeakFilterThreshold,
						&state.AudioVolume,
						&state.SP1MinPower,
						&state.VfoFrequency,
						&state.CenterFrequency,
						&state.SP1MaxFrequency,
						&state.SP1MinFrequency,
						&state.MPXLevel,
						&state.FilterBandwidth,
						&state.SquelchLevel,
						&state.SquelchEnable,
						&state.FmNoiseReductionEnable,
						&state.AudioMute,
						&state.BiasTEnable,
						&state.fingerprint
					);
					Serial.writeString(output_buffer);
					stateChanged = false;
				}
			}
#ifdef DEBUG

			int vrx_num = m_controller.GetVRXCount();
			//DbgMsg("VRX Count %d", vrx_num);
			//Sleep(2000);
#endif // DEBUG

			//@todo: re-work me. 
			//Serial.writeString(PreFormatString(
			//	"Demodulator: %hu \t VFO: %lu \t CenterFrequency: %lu \t FilterBandwidth: %hu \t CRC: %lu\r\n ",
			//	state.Demodulator, state.VfoFrequency, state.CenterFrequency, state.FilterBandwidth, state.fingerprint
			//));
		}
	}
	else
	{
#ifdef DEBUG
		OutputDebugStringA("Bridge is not connected to port \r\n");
#endif
	}
	
	/*


	error = "Dummy";*/
}

void SDRunoPlugin_RemoteBridge::StartBridge(std::string addr)
{
	std::string error = "default";
	
#ifdef DEBUG
	OutputDebugStringA("Bridge is going to start core \r\n");
#endif
	
	std::lock_guard<std::mutex> l(m_lock);
	if (m_started)
	{
		return;
	}

	m_started = true;

	std::string portStub = "\\\\.\\" + addr;
	if ((int)Serial.openDevice(portStub.c_str(), 9600) != (int)1)
	{
		isConnected = false;
		error = "Error while opening port";
	}
	else {
		isConnected = true;
	}

	Sleep(100);
	
	if (isConnected) {
		m_worker = new std::thread(&SDRunoPlugin_RemoteBridge::WorkerFunction, this);
	}
	
#ifdef DEBUG
	OutputDebugStringA(error.c_str());
#endif


}

void SDRunoPlugin_RemoteBridge::StopBridge() 
{
	//Close port
#ifdef DEBUG
	//char out[MAXBUF];
	//sprintf(out, "SDRuno in %s", buffer);
	//OutputDebugStringA(out);
	OutputDebugStringA("Bridge is going to stop core");
#endif

	if (!m_started)
	{
		return;
	}

	Serial.closeDevice();
	m_started = false;
	m_worker->join();
	delete m_worker;
	m_worker = nullptr;
}

void SDRunoPlugin_RemoteBridge::UpdateSampleRate()
{
	sampleRate = (int)m_controller.GetSampleRate(0);
}

void SDRunoPlugin_RemoteBridge::UpdateCenterFrequency()
{
	state.CenterFrequency = (long int)m_controller.GetCenterFrequency(0);
	stateChanged = true;

}

void SDRunoPlugin_RemoteBridge::UpdateSP1MinFreq()
{
	state.SP1MinFrequency = (long int)m_controller.GetSP1MinFrequency(0);
	stateChanged = true;
}

void SDRunoPlugin_RemoteBridge::UpdateSP1MaxFreq() 
{
	state.SP1MaxFrequency = (long int)m_controller.GetSP1MaxFrequency(0);
	stateChanged = true;
}
//@todo: process me
/*
Lots of handlers for events
*/