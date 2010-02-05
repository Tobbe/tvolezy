#include "volxp.h"
#include "tvesettings.h"
#include <windows.h>

VolXP::VolXP(const TveSettings &settings) :
	WND_CLASS_NAME("tVolEzy Callback Wnd"), Volume(settings),
	hCallbackWnd(NULL), mixer(NULL)
{
	error = ERROR_NOERROR;

	createCallbackWindow();
	setupMixerStructs();

	openMixerAndGetLineInfo();
	getLineControls();
}

VolXP::~VolXP()
{
	mixerClose(mixer);
	DestroyWindow(hCallbackWnd);
	UnregisterClass(WND_CLASS_NAME, hInstance);
}

void VolXP::createCallbackWindow()
{
	hInstance = GetModuleHandle(0);

	WNDCLASSEX wcx;
	memset(&wcx, 0, sizeof(WNDCLASSEX));
	wcx.cbSize = sizeof(WNDCLASSEX);
	wcx.lpszClassName = WND_CLASS_NAME;
	wcx.lpfnWndProc = (WNDPROC)callbackWndProc;
	wcx.hInstance = hInstance;
	wcx.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	RegisterClassEx(&wcx);

	hCallbackWnd = CreateWindow(WND_CLASS_NAME,
		"tVolEzy Callback Wnd",
		WS_POPUP | WS_DISABLED,
		0, 0, 0, 0,
		NULL, NULL, hInstance, (void *)this);

	if (!hCallbackWnd)
	{
		error = ERROR_CALLBACK;
		return;
	}
}

LRESULT CALLBACK VolXP::callbackWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	VolXP *pThis = NULL;

	if (uMsg == WM_NCCREATE)
	{
		// get the pointer to the window from lpCreateParams which was set in CreateWindow
		SetWindowLong(hwnd, GWL_USERDATA, (long)((LPCREATESTRUCT(lParam))->lpCreateParams));
	}

	pThis = (VolXP *)GetWindowLong(hwnd, GWL_USERDATA);

	if (pThis && uMsg == MM_MIXM_CONTROL_CHANGE)
	{
		pThis->onControlChanged(reinterpret_cast<HMIXER>(wParam), static_cast<DWORD>(lParam));
	}

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void VolXP::onControlChanged(HMIXER hMixer, DWORD dwControlID)
{
	if (hMixer == mixer)
	{
		if (dwControlID == mcdVol.dwControlID)
		{
			if (volChangedCallback != NULL)
			{
				volChangedCallback(getVolume());
			}
		}
		else if (dwControlID == mcdMute.dwControlID)
		{
			if (muteChangedCallback != NULL)
			{
				muteChangedCallback(isMuted());
			}
		}
	}
}

void VolXP::setupMixerStructs()
{
	// General
	ZeroMemory(&ml, sizeof(ml));
	ml.cbStruct = sizeof(MIXERLINE);
	ml.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;

	ZeroMemory(&mc, sizeof(mc));

	// Volume
	ZeroMemory(&mlcVol, sizeof(mlcVol));
	mlcVol.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
	mlcVol.cbStruct = sizeof(MIXERLINECONTROLS);
	mlcVol.dwLineID = ml.dwLineID;
	mlcVol.cControls = ml.cControls;
	mlcVol.cbmxctrl = sizeof(MIXERCONTROL);
	mlcVol.pamxctrl = &mc;

	ZeroMemory(&mcdVol, sizeof(mcdVol));
	ZeroMemory(&mcdu, sizeof(mcdu));
	mcdVol.paDetails = &mcdu;
	mcdVol.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
	mcdVol.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mcdVol.dwControlID = mlcVol.dwControlID;
	mcdVol.cChannels = 1;
	mcdVol.cMultipleItems = 0;

	// Mute
	memcpy(&mlcMute, &mlcVol, sizeof(mlcMute));
	mlcMute.dwControlType = MIXERCONTROL_CONTROLTYPE_MUTE;

	memcpy(&mcdMute, &mcdVol, sizeof(mcdMute));
	ZeroMemory(&mcdb, sizeof(mcdb));
	mcdMute.paDetails = &mcdb;
	mcdMute.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
}

void VolXP::openMixerAndGetLineInfo()
{
	if (mixerOpen(&mixer, 0, reinterpret_cast<DWORD>(hCallbackWnd), 0, CALLBACK_WINDOW | MIXER_OBJECTF_MIXER) != MMSYSERR_NOERROR)
	{
		error = ERROR_OPENMIXER;
		return;
	}

	// used to get dwLineID
	if (mixerGetLineInfo(reinterpret_cast<HMIXEROBJ>(mixer), &ml, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE) != MMSYSERR_NOERROR)
	{
		error = ERROR_LINEINFO;
		return;
	}
}

void VolXP::getLineControls()
{
	mlcVol.dwLineID = ml.dwLineID;

	if (mixerGetLineControls(reinterpret_cast<HMIXEROBJ>(mixer), &mlcVol,
		MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE) != 
		MMSYSERR_NOERROR)
	{
		error = ERROR_LINECONTROLS;
		return;
	}

	minVol = mc.Bounds.dwMinimum;
	maxVol = mc.Bounds.dwMaximum;
	mcdVol.dwControlID = mc.dwControlID;

	mlcMute.dwLineID = ml.dwLineID;

	if (mixerGetLineControls(reinterpret_cast<HMIXEROBJ>(mixer), &mlcMute,
		MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE)
		!= MMSYSERR_NOERROR)
	{
		error = ERROR_LINECONTROLS;
		return;
	}

	mcdMute.dwControlID = mc.dwControlID;
}

void VolXP::getControlDetails(MIXERCONTROLDETAILS *mcd)
{
	if (mixerGetControlDetails(reinterpret_cast<HMIXEROBJ>(mixer), mcd,
		MIXER_OBJECTF_HMIXER | MIXER_SETCONTROLDETAILSF_VALUE)
		!= MMSYSERR_NOERROR)
	{
		error = ERROR_CONTROLDETAILS;
	}
}

void VolXP::setControlDetails(MIXERCONTROLDETAILS *mcd)
{
	if (mixerSetControlDetails(reinterpret_cast<HMIXEROBJ>(mixer), mcd,
		MIXER_OBJECTF_HMIXER | MIXER_SETCONTROLDETAILSF_VALUE)
		!= MMSYSERR_NOERROR)
	{
		error = ERROR_SETDETAILS;
	}
}

int VolXP::getVolume()
{
	getControlDetails(&mcdVol);
	
	return mcdu.dwValue;
}

void VolXP::change(int steps)
{
	steps = (int)(steps * maxVol/100.0);
	mcdu.dwValue = max(minVol, min(getVolume() + steps, maxVol));

	setControlDetails(&mcdVol);
}

bool VolXP::isMuted()
{
	getControlDetails(&mcdMute);
	
	return mcdb.fValue != 0;
}

void VolXP::setMuted(bool mute)
{
	mcdb.fValue = mute;

	setControlDetails(&mcdMute);
}

bool VolXP::up(int steps)
{
	change(steps);

	if (settings.unmuteOnVolUp)
	{
		setMuted(false);
	}

	return error == ERROR_NOERROR;
}

bool VolXP::down(int steps)
{
	change(-steps);

	if (settings.unmuteOnVolDown)
	{
		setMuted(false);
	}

	return error == ERROR_NOERROR;
}

bool VolXP::toggleMute()
{
	setMuted(!isMuted());
	
	return error == ERROR_NOERROR;
}

int VolXP::getError() const
{
	return error;
}
