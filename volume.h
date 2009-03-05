#ifndef VOLUME_H_
#define VOLUME_H_

#include <windows.h>

class Volume
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

	void setupMixerStructs();
	void setupMixerControlDetails(HMIXER &mixer, MIXERLINECONTROLS &mlc, MIXERCONTROLDETAILS &mcd);
	void change(int steps);
	void reportError(LPCSTR msg);
	bool isMuted();
	void setMuted(bool mute);

public:
	Volume();
	void up(int steps);
	void down(int steps);
	void toggleMute();
};

#endif