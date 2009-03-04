#define STRICT
#define NOCRYPT

#include <windows.h>
#include "tvolezy.h"

HWND hWnd;
HWND hWndParent;
LPCSTR className = "tVolEzyWndClass";
LPCSTR revID = "tVolEzy 0.1 by Tobbe";
BOOL showErrors;

void bangVol(HWND caller, const char *bangName, const char *args);
void bangToggleMute(HWND caller, const char *bangName, const char *args);
void changeVolume(HWND caller, int steps);
void reportError(LPCSTR msg);
LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

BOOL WINAPI DllMain(HANDLE hInst, ULONG ul_reason_for_call, LPVOID lpReserved)
{
	return TRUE;
}

// Actual main function
extern "C" int initModuleEx(HWND parentWnd, HINSTANCE dllInst, LPCSTR szPath)
{
	szPath=szPath;
	hWndParent = parentWnd;

	// Get tVolEzy settings
	showErrors = GetRCBool("tVolEzyShowErrors", TRUE);

	WNDCLASS wc;
	memset(&wc, 0, sizeof(wc));
	wc.lpfnWndProc = (WNDPROC)wndProc;
	wc.hInstance = dllInst;
	wc.lpszClassName = className;
	if (!RegisterClass(&wc))
	{
		reportError("Error registering tVolEzy window class");
		return 1;
	}

	hWnd = CreateWindowEx(WS_EX_TOOLWINDOW, className, "", WS_CHILD,
		0, 0, 0, 0, hWndParent, NULL, dllInst, NULL);
	if (hWnd == NULL)
	{
		reportError("Error creating tVolEzy window");
		UnregisterClass("", dllInst);
		return 1;
	}

	// Register our bangs with LiteStep
	AddBangCommandEx("!tVolEzyUp", bangVol);
	AddBangCommandEx("!tVolEzyDown", bangVol);
	AddBangCommandEx("!tVolEzyToggleMute", bangToggleMute);

	// Register message for version info
	UINT msgs[] = {LM_GETREVID, 0};
	SendMessage(hWndParent, LM_REGISTERMESSAGE, (WPARAM)hWnd, (LPARAM)msgs);

	return 0;
}

void bangVol(HWND caller, const char* bangName, const char* args)
{
	int direction = 1;

	if (bangName[8] == 'D' || bangName[8] == 'd')
	{
		direction = -1;
	}

	if (args && args[0] != '\0')
	{
		changeVolume(caller, atol(args) * direction);
	}
	else
	{
		changeVolume(caller, direction);
	}
}

void changeVolume(HWND caller, int steps)
{
	//
	// Open the mixer, set hMixer
	//

	HMIXER hMixer;
	if (mixerOpen(&hMixer, 0, 0, 0, MIXER_OBJECTF_HMIXER) != MMSYSERR_NOERROR)
	{
		reportError("Couldn't open mixer");
		return;
	}

	//
	// get dwLineID
	//

	MIXERLINE ml = {0};
	ml.cbStruct = sizeof(MIXERLINE);
	ml.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
	if (mixerGetLineInfo((HMIXEROBJ)hMixer, &ml, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE) != MMSYSERR_NOERROR)
	{
		reportError("Couldn't get line info");
		return;
	}

	//
	// get dwControlID
	//

	MIXERLINECONTROLS mlc = {0};
	MIXERCONTROL mc = {0};

	mlc.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
	mlc.cbStruct = sizeof(MIXERLINECONTROLS);
	mlc.dwLineID = ml.dwLineID;
	mlc.cControls = 1;
	mlc.cbmxctrl = sizeof(MIXERCONTROL);
	mlc.pamxctrl = &mc;

	if (mixerGetLineControls((HMIXEROBJ)hMixer, &mlc, MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE) != MMSYSERR_NOERROR)
	{
		reportError("Couldn't get line controls");
		return;
	}

	MIXERCONTROLDETAILS mcd = {0};
	MIXERCONTROLDETAILS_BOOLEAN mcdb = {0};
	MIXERCONTROLDETAILS_UNSIGNED mcdu = {0};

	mcd.paDetails = &mcdu;
	mcd.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
	mcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mcd.dwControlID = mc.dwControlID;
	mcd.cChannels = 1;
	mcd.cMultipleItems = 0;

	if (mixerGetControlDetails((HMIXEROBJ) hMixer, &mcd, MIXER_SETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR)
	{
		reportError("Couldn't get control details");
		return;
	}

	mcdb.fValue = !mcdb.fValue;

	//full volume at 65535, max dwords can handle is 4294967295
	if ((long)mcdu.dwValue + steps < 0)
	{
		mcdu.dwValue = 0;
	}
	else if (mcdu.dwValue + steps > 65535)
	{
		mcdu.dwValue = 65535;
	}
	else
	{
		mcdu.dwValue += steps;
	}

	if (mixerSetControlDetails((HMIXEROBJ)hMixer, &mcd, MIXER_SETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR)
	{
		reportError("Couldn't set volume");
		return;
	}

	mixerClose(hMixer);
}

void bangToggleMute(HWND caller, const char* bangName, const char* args)
{
	//
	// Open the mixer, set hMixer
	//

	HMIXER hMixer;
	if (mixerOpen(&hMixer, 0, 0, 0, MIXER_OBJECTF_HMIXER) != MMSYSERR_NOERROR)
	{
		reportError("Couldn't open mixer");
		return;
	}

	//
	// get dwLineID
	//

	MIXERLINE ml = {0};
	ml.cbStruct = sizeof(MIXERLINE);
	ml.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;
	if (mixerGetLineInfo((HMIXEROBJ)hMixer, &ml, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE) != MMSYSERR_NOERROR)
	{
		reportError("Couldn't get line info");
		return;
	}

	//
	// get dwControlID
	//

	MIXERLINECONTROLS mlc = {0};
	MIXERCONTROL mc = {0};

	mlc.dwControlType = MIXERCONTROL_CONTROLTYPE_MUTE;
	mlc.cbStruct = sizeof(MIXERLINECONTROLS);
	mlc.dwLineID = ml.dwLineID;
	mlc.cControls = 1;
	mlc.cbmxctrl = sizeof(MIXERCONTROL);
	mlc.pamxctrl = &mc;

	if (mixerGetLineControls((HMIXEROBJ)hMixer, &mlc, MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE) != MMSYSERR_NOERROR)
	{
		reportError("Couldn't get line controls");
		return;
	}

	MIXERCONTROLDETAILS mcd = {0};
	MIXERCONTROLDETAILS_BOOLEAN mcdb = {0};
	MIXERCONTROLDETAILS_UNSIGNED mcdu = {0};

	mcd.paDetails = &mcdb;
	mcd.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
	mcd.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mcd.dwControlID = mc.dwControlID;
	mcd.cChannels = 1;
	mcd.cMultipleItems = 0;

	if (mixerGetControlDetails((HMIXEROBJ) hMixer, &mcd, MIXER_SETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR)
	{
		reportError("Couldn't get control details");
		return;
	}

	mcdb.fValue = !mcdb.fValue;

	if (mixerSetControlDetails((HMIXEROBJ)hMixer, &mcd, MIXER_SETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR)
	{
		reportError("Couldn't set volume");
		return;
	}

	mixerClose(hMixer);
}

void reportError(LPCSTR msg)
{
	if (showErrors)
	{
		MessageBox(NULL, msg, "tVolEzy error", MB_OK | MB_ICONERROR);
	}
}

LRESULT CALLBACK wndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message){
		case LM_GETREVID:
			lstrcpy((LPSTR)lParam, revID);
			return strlen((LPTSTR)lParam);

		case WM_CLOSE:
			return 0;

		case WM_SYSCOMMAND:
			switch (wParam){
				case SC_CLOSE:
					return 0;
				default:
					break;
			}
			break;

		case WM_DESTROY:
			hWnd = NULL;
			return 0;

		case WM_APPCOMMAND:
			MessageBox(hWndParent, "Error registering tVolEzy window class", "tVolEzy error", MB_OK);
			return 1;

		default:
			break;
	}
	return DefWindowProc(hwnd,message,wParam,lParam);
}

// -------------------------------------------------------
// cleanup (opposite of init())
extern "C" void quitModule(HINSTANCE dllInst)
{
	RemoveBangCommand("!tVolEzyUp");
	RemoveBangCommand("!tVolEzyDown");
	RemoveBangCommand("!tVolEzyToggleMute");
}