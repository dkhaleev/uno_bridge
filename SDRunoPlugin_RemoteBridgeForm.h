#pragma once

#include <nana/gui.hpp>
#include <nana/gui/widgets/button.hpp>
#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/widgets/slider.hpp>
#include <nana/gui/widgets/label.hpp>
#include <nana/gui/widgets/combox.hpp>
#include <nana/gui/timer.hpp>
#include <nana/gui/widgets/picture.hpp>
#include <nana/gui/filebox.hpp>
#include <nana/gui/dragger.hpp>
#include <map>
#include <vector>

#include <iostream>
#include <iomanip>
#include <sstream>

#include <iunoplugincontroller.h>

// Shouldn't need to change these
#define topBarHeight (27)
#define bottomBarHeight (8)
#define sideBorderWidth (8)

// TODO: Change these numbers to the height and width of your form
#define formWidth (297)
#define formHeight (240)


class SDRunoPlugin_RemoteBridgeUi;

class SDRunoPlugin_RemoteBridgeForm : public nana::form
{

public:
	//enumerator
	typedef std::vector<std::basic_string<TCHAR>> StringVector;
	StringVector portsList;

	SDRunoPlugin_RemoteBridgeForm(SDRunoPlugin_RemoteBridgeUi& parent, IUnoPluginController& controller);		
	~SDRunoPlugin_RemoteBridgeForm();

	void SetStartButtonCaption(std::string text)
	{
		startBtn.caption(text);
	}

	void SetPrtComboBoxState(bool state)
	{
		portCb.enabled(state);
	}

	int GetPortIndex()
	{
		return portCb.option();
	}

	void StartTimer()
	{
		m_timerCount = 0.0;
		m_timer.start();
	}

	void StopTimer()
	{
		m_timer.stop();
	}
	
	void Run();
	
private:

	void Setup();

	// The following is to set up the panel graphic to look like a standard SDRuno panel
	nana::picture bg_border{ *this, nana::rectangle(0, 0, formWidth, formHeight) };
	nana::picture bg_inner{ bg_border, nana::rectangle(sideBorderWidth, topBarHeight, formWidth - (2 * sideBorderWidth), formHeight - topBarHeight - bottomBarHeight) };
	nana::picture header_bar{ *this, true };
	nana::label title_bar_label{ *this, true };
	nana::dragger form_dragger;
	nana::label form_drag_label{ *this, nana::rectangle(0, 0, formWidth, formHeight) };
	nana::paint::image img_min_normal;
	nana::paint::image img_min_down;
	nana::paint::image img_close_normal;
	nana::paint::image img_close_down;
	nana::paint::image img_header;
	nana::picture close_button{ *this, nana::rectangle(0, 0, 20, 15) };
	nana::picture min_button{ *this, nana::rectangle(0, 0, 20, 15) };

	//form elements
	//start button
	nana::button startBtn{ *this, nana::rectangle(18, 165, 80, 20) };
	//save button
	nana::button saveBtn{ *this, nana::rectangle(18, 195, 80, 20) };
	//port label
	nana::label portLbl{ *this, nana::rectangle(138, 87, 70, 20) };
	//port combobox
	nana::combox portCb{ *this, nana::rectangle(208, 85, 70, 20) };
	//status
	nana::label statusLbl{ *this, nana::rectangle(18, 137, 270, 20) };
	//timer
	nana::timer m_timer;

	// Uncomment the following 4 lines if you want a SETT button and panel
	//nana::paint::image img_sett_normal;
	//nana::paint::image img_sett_down;
	//nana::picture sett_button{ *this, nana::rectangle(0, 0, 40, 15) };
	//void SettingsButton_Click();

	// TODO: Now add your UI controls here

	SDRunoPlugin_RemoteBridgeUi & m_parent;
	IUnoPluginController & m_controller;

	double m_timerCount;

};
