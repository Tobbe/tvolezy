#include "volvista.h"
#include <windows.h>
#include <audioclient.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>

VolVista::VolVista(const TveSettings &settings) : Volume(settings)
{
	init();
}

void VolVista::init()
{
	error = ERROR_NOERROR;
	CoInitialize(NULL);
	IMMDeviceEnumerator *deviceEnumerator = NULL;
	hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_INPROC_SERVER, __uuidof(IMMDeviceEnumerator), (LPVOID *)&deviceEnumerator);
	IMMDevice *defaultDevice = NULL;

	hr = deviceEnumerator->GetDefaultAudioEndpoint(eRender, eConsole, &defaultDevice);

	switch (hr)
	{
	case S_OK:
		break;
	case E_POINTER:
		error = ERROR_PPDEVICENULL;
		break;
	case E_INVALIDARG:
		error = ERROR_OUTOFRANGE;
		break;
	case E_OUTOFMEMORY:
		error = ERROR_OUTOFMEM;
		break;
	default:
		error = ERROR_GETDEFAULT;
	}

	// E_NOTFOUND is not a constant, so it can't be used in
	// the switch statement above
	if (hr == E_NOTFOUND)
	{
		error = ERROR_NOTFOUND;
	}

	if (deviceEnumerator != NULL)
	{
		deviceEnumerator->Release();
		deviceEnumerator = NULL;
	}

	endpointVolume = NULL;

	if (hr != S_OK)
	{
		return;
	}

	if (defaultDevice)
	{
		hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume);
	}

	switch (hr)
	{
	case S_OK:
		break;
	case E_NOINTERFACE:
		error = ERROR_NOIFACE;
		break;
	case E_POINTER:
		error = ERROR_PPINTERFACENULL;
		break;
	case E_INVALIDARG:
		error = ERROR_INVALIDARG;
		break;
	case E_OUTOFMEMORY:
		error = ERROR_OUTOFMEM;
		break;
	case AUDCLNT_E_DEVICE_INVALIDATED:
		error = ERROR_REMOVED;
		break;
	default:
		error = ERROR_ACTIVATE;
	}

	if (defaultDevice != NULL)
	{
		defaultDevice->Release();
		defaultDevice = NULL; 
	}
}

VolVista::~VolVista()
{
	if (endpointVolume)
	{
		endpointVolume->Release();
		endpointVolume = NULL;
	}

	CoUninitialize();
}

void VolVista::change(int steps)
{
	float vol;
	error = ERROR_NOERROR;

	hr = endpointVolume->GetMasterVolumeLevelScalar(&vol);

	if (hr != S_OK)
	{
		error = ERROR_GETVOL;
		return;
	}

	vol = max(0, min(vol + steps/100.0f, 1.0f));
	hr = endpointVolume->SetMasterVolumeLevelScalar(vol, NULL);

	if (hr != S_OK)
	{
		error = ERROR_SETVOL;
	}
}

bool VolVista::up(int steps)
{
	change(steps);
	return error == ERROR_NOERROR;
}

bool VolVista::down(int steps)
{
	change(-steps);
	return error == ERROR_NOERROR;
}

bool VolVista::toggleMute()
{
	BOOL mute;
	hr = endpointVolume->GetMute(&mute);

	if (hr != S_OK)
	{
		error = ERROR_GETMUTE;
		return false;
	}

	hr = endpointVolume->SetMute(!mute, NULL);

	if (hr != S_OK)
	{
		error = ERROR_SETMUTE;
		return false;
	}

	return true;
}

int VolVista::getError() const
{
	return error;
}