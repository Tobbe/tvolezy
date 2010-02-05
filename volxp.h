#ifndef VOL_XP_H_
#define VOL_XP_H_

#include "tvesettings.h"
#include "volume.h"
#include <windows.h>
#include <string>

class VolXP : public Volume
{
private:
	MIXERLINE ml;
	MIXERCONTROL mc;
	MIXERLINECONTROLS mlcVol;
	MIXERLINECONTROLS mlcMute;
	MIXERCONTROLDETAILS mcdVol;
	MIXERCONTROLDETAILS mcdMute;
	MIXERCONTROLDETAILS_UNSIGNED mcdu;
	MIXERCONTROLDETAILS_BOOLEAN mcdb;
	HWND hCallbackWnd;
	HINSTANCE hInstance;
	HMIXER mixer;
	int minVol;
	int maxVol;
	const char* WND_CLASS_NAME;

	void setupMixerStructs();
	void openMixerAndGetLineInfo();
	void getLineControls();
	void setControlDetails(MIXERCONTROLDETAILS *mcd);
	void getControlDetails(MIXERCONTROLDETAILS *mcd);
	void change(int steps);
	bool isMuted();
	void setMuted(bool mute);
	int getVolume();
	void onControlChanged(HMIXER hMixer, DWORD dwControlID);
	void createCallbackWindow();
	static LRESULT CALLBACK callbackWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

public:
	VolXP(const TveSettings &settings);
	~VolXP();
	bool up(int steps);
	bool down(int steps);
	bool toggleMute();
	int getError() const;
};

#endif