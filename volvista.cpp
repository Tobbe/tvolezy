#include "volvista.h"
#include <windows.h>
#include <audioclient.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <strsafe.h>

VolVista::VolVista(const TveSettings &settings) : Volume(settings), volNotification(NULL)
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
	checkAudioEndpointResultForError(hr);

	deviceEnumerator->Release();
	deviceEnumerator = NULL;

	endpointVolume = NULL;

	if (hr != S_OK)
	{
		return;
	}

	if (defaultDevice)
	{
		hr = defaultDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_INPROC_SERVER, NULL, (LPVOID *)&endpointVolume);
		checkActivateResultForError(hr);

		volNotification = new VolumeNotification(*this);
		hr = endpointVolume->RegisterControlChangeNotify(volNotification); 

		defaultDevice->Release();
		defaultDevice = NULL; 
	}
}

VolVista::~VolVista()
{
	if (endpointVolume)
	{
		endpointVolume->UnregisterControlChangeNotify(volNotification); 
		endpointVolume->Release();
		endpointVolume = NULL;
		volNotification->Release(); 
	}

	CoUninitialize();
}

void VolVista::checkAudioEndpointResultForError(HRESULT hr)
{
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
}

void VolVista::checkActivateResultForError(HRESULT hr)
{
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

	if (settings.unmuteOnVolUp)
	{
		hr = endpointVolume->SetMute(false, NULL);
	}

	return error == ERROR_NOERROR;
}

bool VolVista::down(int steps)
{
	change(-steps);

	if (settings.unmuteOnVolDown)
	{
		hr = endpointVolume->SetMute(false, NULL);
	}

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

VolVista::VolumeNotification::VolumeNotification(const VolVista &outer) : refCount(1), currentVolume(-1), parent(outer)
{ 
}

VolVista::VolumeNotification::~VolumeNotification()
{
}

STDMETHODIMP_(ULONG) VolVista::VolumeNotification::AddRef()
{
	return InterlockedIncrement(&refCount);
}

STDMETHODIMP_(ULONG) VolVista::VolumeNotification::Release()
{
	LONG ref = InterlockedDecrement(&refCount);

	if (ref == 0)
	{
		delete this; 
	}

	return ref;
}

STDMETHODIMP VolVista::VolumeNotification::QueryInterface(REFIID IID, void **returnValue)
{
	if (IID == IID_IUnknown || IID == __uuidof(IAudioEndpointVolumeCallback))
	{
		*returnValue = static_cast<IUnknown*>(this);
		AddRef();

		return S_OK;
	}

	*returnValue = NULL;

	return E_NOINTERFACE;
}

STDMETHODIMP VolVista::VolumeNotification::OnNotify(PAUDIO_VOLUME_NOTIFICATION_DATA notificationData)
{
	int tmpVol = static_cast<int>(notificationData->fMasterVolume * 100 + 0.5);
	if (currentVolume != tmpVol)
	{
		currentVolume = tmpVol;
		parent.volChangedCallback(currentVolume);
	}
	else
	{
		parent.muteChangedCallback(notificationData->bMuted != FALSE);
	}

	return S_OK;
}
