#ifndef VOLUME_H_
#define VOLUME_H_

#include <windows.h>

class Volume
{
private:
	HMIXER mixerVol;
	HMIXER mixerMute;
	MIXERLINE mlVol;
	MIXERLINE mlMute;
	MIXERCONTROL mcVol;
	MIXERCONTROL mcMute;
	MIXERLINECONTROLS mlcVol;
	MIXERLINECONTROLS mlcMute;
	MIXERCONTROLDETAILS mcdVol;
	MIXERCONTROLDETAILS mcdMute;
	MIXERCONTROLDETAILS_UNSIGNED mcdu;
	MIXERCONTROLDETAILS_BOOLEAN mcdb;
	void setupMixers();
	void change(int steps);
	void reportError(LPCSTR msg);

public:
	Volume();
	~Volume();
	void up(int steps);
	void down(int steps);
	void toggleMute();
};

#endif