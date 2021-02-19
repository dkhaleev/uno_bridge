#pragma once

#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/timer.hpp>
#include <iunoplugin.h>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <tchar.h>
#include <vector>

#include <iunoplugincontroller.h>
#include "SDRunoPlugin_RemoteBridgeForm.h"

#define MAX_PORT_NUM      (256)
#define MAX_STR_LEN       (256*sizeof(TCHAR))

// Forward reference
class SDRunoPlugin_RemoteBridge;

class SDRunoPlugin_RemoteBridgeUi
{
public:

	SDRunoPlugin_RemoteBridgeUi(SDRunoPlugin_RemoteBridge& parent, IUnoPluginController& controller);
	~SDRunoPlugin_RemoteBridgeUi();

	void HandleEvent(const UnoEvent& evt);
	void FormClosed();

	void ShowUi();
	void StartButtonClicked(std::string addr);
	bool IsRunning();

	int GetPortIndex();

	int LoadX();
	int LoadY();

	//enumerator
	typedef std::vector<std::basic_string<TCHAR>> StringVector;
	StringVector ports;
	int portNumbers;
	void EnumeratePorts();
	bool PortsEnumerator(int *pNumber, TCHAR *pPortName, int strMaxLen);

	std::string LoadPort();

private:
	
	SDRunoPlugin_RemoteBridge & m_parent;
	std::thread m_thread;
	std::shared_ptr<SDRunoPlugin_RemoteBridgeForm> m_form;
	void StartBridge(std::string port);
	void StopBridge();

	bool m_started = false;

	std::mutex m_lock;

	IUnoPluginController & m_controller;
};
