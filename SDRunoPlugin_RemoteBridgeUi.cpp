#include <windows.h>

#include <stdio.h>
#include <tchar.h>
#include <setupapi.h>
#include <locale.h>
#include <vector>

#include <sstream>
#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/widgets/slider.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/timer.hpp>
#include <unoevent.h>

#include "SDRunoPlugin_RemoteBridge.h"
#include "SDRunoPlugin_RemoteBridgeUi.h"
#include "SDRunoPlugin_RemoteBridgeForm.h"

using namespace std;


// Ui constructor - load the Ui control into a thread
SDRunoPlugin_RemoteBridgeUi::SDRunoPlugin_RemoteBridgeUi(SDRunoPlugin_RemoteBridge& parent, IUnoPluginController& controller) :
	m_parent(parent),
	m_form(nullptr),
	m_controller(controller)
{
	m_thread = std::thread(&SDRunoPlugin_RemoteBridgeUi::ShowUi, this);
}

// Ui destructor (the nana::API::exit_all();) is required if using Nana UI library
SDRunoPlugin_RemoteBridgeUi::~SDRunoPlugin_RemoteBridgeUi()
{	
	nana::API::exit_all();
	m_thread.join();	
}

// Show and execute the form
void SDRunoPlugin_RemoteBridgeUi::ShowUi()
{	
	m_lock.lock();
	m_form = std::make_shared<SDRunoPlugin_RemoteBridgeForm>(*this, m_controller);
	m_lock.unlock();

	m_form->Run();
}

// Load X from the ini file (if exists)
// TODO: Change RemoteBridge to plugin name
int SDRunoPlugin_RemoteBridgeUi::LoadX()
{
	std::string tmp;
	m_controller.GetConfigurationKey("RemoteBridge.X", tmp);
	if (tmp.empty())
	{
		return -1;
	}
	return stoi(tmp);
}

// Load Y from the ini file (if exists)
// TODO: Change RemoteBridge to plugin name
int SDRunoPlugin_RemoteBridgeUi::LoadY()
{
	std::string tmp;
	m_controller.GetConfigurationKey("RemoteBridge.Y", tmp);
	if (tmp.empty())
	{
		return -1;
	}
	return stoi(tmp);
}

int SDRunoPlugin_RemoteBridgeUi::GetPortIndex()
{
	return m_form->GetPortIndex();
}

// Handle events from SDRuno
// TODO: code what to do when receiving relevant events
void SDRunoPlugin_RemoteBridgeUi::HandleEvent(const UnoEvent& ev)
{
	switch (ev.GetType())
	{
	case UnoEvent::UndefinedEvent: //UndefinedEvent = 0
#ifdef DEBUG
		OutputDebugStringA("UndefinedEvent = 0 \r\n");
#endif DEBUG
		break;

	case UnoEvent::DemodulatorChanged: //DemodulatorChanged = 1
#ifdef DEBUG
		OutputDebugStringA("DemodulatorChanged = 1 \r\n");
#endif DEBUG
		break;

	case UnoEvent::BandwidthChanged: //BandwidthChanged = 2
#ifdef DEBUG
		OutputDebugStringA("BandwidthChanged = 2 \r\n");
#endif DEBUG
		break;

	case UnoEvent::FrequencyChanged: //FrequencyChanged = 3
#ifdef DEBUG
		OutputDebugStringA("FrequencyChanged = 3 \r\n");
#endif DEBUG
		m_parent.UpdateFrequencies();
		break;

	case UnoEvent::CenterFrequencyChanged: //CenterFrequencyChanged = 4
#ifdef DEBUG
		OutputDebugStringA("CenterFrequencyChanged = 4 \r\n");
#endif DEBUG
		m_parent.UpdateFrequencies();
		break;

	case UnoEvent::SampleRateChanged: //SampleRateChanged = 5
#ifdef DEBUG
		OutputDebugStringA("SampleRateChanged = 5 \r\n");
#endif DEBUG
		break;

	case UnoEvent::StreamingStarted: //StreamingStarted = 6
#ifdef DEBUG
		OutputDebugStringA("StreamingStarted = 6 \r\n");
#endif DEBUG
		break;

	case UnoEvent::StreamingStopped: //StreamingStopped = 7
#ifdef DEBUG
		OutputDebugStringA("StreamingStopped = 7 \r\n");
#endif DEBUG
		break;

	case UnoEvent::SquelchEnableChanged: //SquelchEnableChanged = 8
#ifdef DEBUG
		OutputDebugStringA("SquelchEnableChanged = 8 \r\n");
#endif DEBUG
		break;

	case UnoEvent::SquelchThresholdChanged: //SquelchThresholdChanged = 9
#ifdef DEBUG
		OutputDebugStringA("SquelchThresholdChanged = 9 \r\n");
#endif DEBUG
		break;

	case UnoEvent::AgcThresholdChanged: //AgcThresholdChanged = 10
#ifdef DEBUG
		OutputDebugStringA("AgcThresholdChanged = 10 \r\n");
#endif DEBUG
		break;

	case UnoEvent::AgcModeChanged: //AgcModeChanged = 11
#ifdef DEBUG
		OutputDebugStringA("AgcModeChanged = 11 \r\n");
#endif DEBUG
		break;

	case UnoEvent::NoiseBlankerLevelChanged: //NoiseBlankerLevelChanged = 12
#ifdef DEBUG
		OutputDebugStringA("NoiseBlankerLevelChanged = 12 \r\n");
#endif DEBUG
		break;

	case UnoEvent::NoiseReductionLevelChanged: //NoiseReductionLevelChanged = 13
#ifdef DEBUG
		OutputDebugStringA("NoiseReductionLevelChanged = 13 \r\n");
#endif DEBUG
		break;

	case UnoEvent::CwPeakFilterThresholdChanged: //CwPeakFilterThresholdChanged = 14
#ifdef DEBUG
		OutputDebugStringA("CwPeakFilterThresholdChanged = 14 \r\n");
#endif DEBUG
		break;

	case UnoEvent::FmNoiseReductionEnabledChanged: //FmNoiseReductionEnabledChanged = 15
#ifdef DEBUG
		OutputDebugStringA("FmNoiseReductionEnabledChanged = 15 \r\n");
#endif DEBUG
		break;

	case UnoEvent::FmNoiseReductionThresholdChanged: //FmNoiseReductionThresholdChanged = 16
#ifdef DEBUG
		OutputDebugStringA("FmNoiseReductionThresholdChanged = 16 \r\n");
#endif DEBUG
		break;

	case UnoEvent::WfmDeemphasisModeChanged: //WfmDeemphasisModeChanged = 17
#ifdef DEBUG
		OutputDebugStringA("WfmDeemphasisModeChanged = 17 \r\n");
#endif DEBUG
		break;

	case UnoEvent::AudioVolumeChanged: //AudioVolumeChanged = 18
#ifdef DEBUG
		OutputDebugStringA("AudioVolumeChanged = 18 \r\n");
#endif DEBUG
		break;

	case UnoEvent::AudioMuteChanged: //AudioMuteChanged = 19
#ifdef DEBUG
		OutputDebugStringA("AudioMuteChanged = 19 \r\n");
#endif DEBUG
		break;

	case UnoEvent::IFGainChanged: //IFGainChanged = 20
#ifdef DEBUG
		OutputDebugStringA("IFGainChanged = 20 \r\n");
#endif DEBUG
		break;

	case UnoEvent::SavingWorkspace: //SavingWorkspace = 21
#ifdef DEBUG
		OutputDebugStringA("SavingWorkspace = 21 \r\n");
#endif DEBUG
		break;

	case UnoEvent::VRXCountChanged: //VRXCountChanged = 22
#ifdef DEBUG
		OutputDebugStringA("VRXCountChanged = 22 \r\n");
#endif DEBUG
		break;

	case UnoEvent::VRXStateChanged: //VRXStateChanged = 23
#ifdef DEBUG
		OutputDebugStringA("VRXStateChanged = 23 \r\n");
#endif DEBUG
		break;

	case UnoEvent::StepSizeChanged: //StepSizeChanged = 24
#ifdef DEBUG
		OutputDebugStringA("SavingWorkspace = 21 \r\n");
#endif DEBUG
		break;

	case UnoEvent::VFOChanged: //VFOChanged = 25
#ifdef DEBUG
		OutputDebugStringA("VFOChanged = 25 \r\n");
#endif DEBUG
		break;

	case UnoEvent::ClosingDown: //ClosingDown = 26
#ifdef DEBUG
		OutputDebugStringA("ClosingDown = 26 \r\n");
		FormClosed();
#endif DEBUG
		break;

	case UnoEvent::SP1MinFreqChanged: //SP1MinFreqChanged = 27
#ifdef DEBUG
		OutputDebugStringA("SP1MinFreqChanged = 27 \r\n");
#endif DEBUG
		m_parent.UpdateFrequencies();
		break;

	case UnoEvent::SP1MaxFreqChanged: //SP1MaxFreqChanged = 28
#ifdef DEBUG
		OutputDebugStringA("SP1MaxFreqChanged = 28 \r\n");
		m_parent.UpdateFrequencies();
#endif DEBUG
		break;

	case UnoEvent::BiasTEnableChanged: //BiasTEnableChanged = 29
#ifdef DEBUG
		OutputDebugStringA("BiasTEnableChanged \r\n");
#endif DEBUG
		break;

	case UnoEvent::SP1MinPowerChanged: //SP1MinPowerChanged = 30
#ifdef DEBUG
		OutputDebugStringA("SP1MinPowerChanged \r\n");
#endif DEBUG
		break;

	case UnoEvent::SP1MaxPowerChanged: //SP1MaxPowerChanged = 31
#ifdef DEBUG
		OutputDebugStringA("SP1MaxPowerChanged \r\n");
#endif DEBUG
		break;

	default:
#ifdef DEBUG
		OutputDebugStringA("Default event case \r\n");
#endif DEBUG
		break;
	}
}

void SDRunoPlugin_RemoteBridgeUi::EnumeratePorts() {
	TCHAR pName[MAX_PORT_NUM][MAX_STR_LEN];
	int i;
	int n;

	char* nativeLocale;

	nativeLocale = _strdup(setlocale(LC_CTYPE, NULL));
	
	SDRunoPlugin_RemoteBridgeUi::PortsEnumerator(&n, &pName[0][0], MAX_STR_LEN);
	
	setlocale(LC_CTYPE, "");
	
	for (i = 0; i < n; i++) {
		ports.push_back(&pName[i][0]);
	}
	
	setlocale(LC_CTYPE, nativeLocale);
}

bool SDRunoPlugin_RemoteBridgeUi::PortsEnumerator(int *pNumber, TCHAR *pPortName, int strMaxLen)
{
	int i, jj;
	INT ret;

	OSVERSIONINFOEX osvi;
	ULONGLONG dwlConditionMask;
	DWORD dwChars;

	TCHAR *pDevices;
	UINT nChars;

	ret = FALSE;

	memset(&osvi, 0, sizeof(osvi));
	osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	osvi.dwPlatformId = VER_PLATFORM_WIN32_NT;
	dwlConditionMask = 0;

	VER_SET_CONDITION(dwlConditionMask, VER_PLATFORMID, VER_EQUAL);

	if (FALSE == VerifyVersionInfo(&osvi, VER_PLATFORMID, dwlConditionMask))
	{
		DWORD dwError = GetLastError();
		//_tprintf(TEXT("VerifyVersionInfo error, %d\n", dwError));
		return false;
	}/*if*/


	pDevices = NULL;

	nChars = 4096;
	pDevices = (TCHAR*)HeapAlloc(GetProcessHeap(),
		HEAP_GENERATE_EXCEPTIONS, nChars * sizeof(TCHAR));

	while (0 < nChars)
	{
		dwChars = QueryDosDevice(NULL, pDevices, nChars);

		if (0 == dwChars)
		{
			DWORD dwError = GetLastError();

			if (ERROR_INSUFFICIENT_BUFFER == dwError)
			{
				nChars *= 2;
				HeapFree(GetProcessHeap(), 0, pDevices);
				pDevices = (TCHAR*)HeapAlloc(GetProcessHeap(),
					HEAP_GENERATE_EXCEPTIONS, nChars * sizeof(TCHAR));

				continue;
			}/*if ERROR_INSUFFICIENT_BUFFER == dwError*/

			_tprintf(TEXT("QueryDosDevice error, %d\n", dwError));
			return false;
		}/*if */


		//printf("dwChars = %d\n", dwChars);
		i = 0;
		jj = 0;
		while (TEXT('\0') != pDevices[i])
		{
			TCHAR* pszCurrentDevice;
			size_t nLen;
			pszCurrentDevice = &(pDevices[i]);
			nLen = _tcslen(pszCurrentDevice);

			//_tprintf(TEXT("%s\n"), &pTargetPathStr[i]);
			if (3 < nLen)
			{
				if ((0 == _tcsnicmp(pszCurrentDevice, TEXT("COM"), 3))
					&& FALSE != isdigit(pszCurrentDevice[3]))
				{
					//Work out the port number                
					_tcsncpy(pPortName + jj * strMaxLen,
						pszCurrentDevice, strMaxLen);
					jj++;

				}
			}

			i += (nLen + 1);
				}

		break;
			}/*while*/

	if (NULL != pDevices)
		HeapFree(GetProcessHeap(), 0, pDevices);


	*pNumber = jj;

	if (0 < jj)
		ret = TRUE;

	return ret;
		}/*EnumerateComPortByQueryDosDevice*/

std::string SDRunoPlugin_RemoteBridgeUi::LoadPort()
{
	std::string tmp;
	m_controller.GetConfigurationKey("Bridge.Port", tmp);
	if (tmp.empty()) 
	{
		return "";
	}

	return tmp;

}

bool SDRunoPlugin_RemoteBridgeUi::IsRunning()
{
	return m_parent.IsRunning();
}

void SDRunoPlugin_RemoteBridgeUi::StartButtonClicked(std::string addr)
{
	if (!m_started)
	{
		StartBridge(addr);
	}
	else 
	{
		StopBridge();
	}
}

void SDRunoPlugin_RemoteBridgeUi::StartBridge(std::string addr)
{
#ifdef DEBUG
		OutputDebugStringA("Bridge is going to start UI wrapper on port \r\n");
#endif

	std::lock_guard<std::mutex> l(m_lock);
	if (!m_started)
	{
		m_parent.StartBridge(addr);
		if (m_form != nullptr) 
		{
			m_form->SetStartButtonCaption("Stop");
			m_form->SetPrtComboBoxState(false);
			m_form->StartTimer();
		}

		m_started = true;
	}
}

void SDRunoPlugin_RemoteBridgeUi::StopBridge()
{
#ifdef DEBUG
	OutputDebugStringA("Bridge is going to stop UI wrapper \r\n");
#endif

}

// Required to make sure the plugin is correctly unloaded when closed
void SDRunoPlugin_RemoteBridgeUi::FormClosed()
{
	m_controller.RequestUnload(&m_parent);
}
