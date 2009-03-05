#include "volume.h"
#include <windows.h>

Volume::Volume()
{
	setupMixerStructs();
}

void Volume::setupMixerStructs()
{
	// General
	memset(&ml, 0, sizeof(ml));
	ml.cbStruct = sizeof(MIXERLINE);
	ml.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;

	memset(&mc, 0, sizeof(mc));

	// Volume
	memset(&mlcVol, 0, sizeof(mlcVol));
	mlcVol.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
	mlcVol.cbStruct = sizeof(MIXERLINECONTROLS);
	mlcVol.dwLineID = 0;
	mlcVol.cControls = 1;
	mlcVol.cbmxctrl = sizeof(MIXERCONTROL);
	mlcVol.pamxctrl = &mc;

	memset(&mcdVol, 0, sizeof(mcdVol));
	memset(&mcdu, 0, sizeof(mcdu));
	mcdVol.paDetails = &mcdu;
	mcdVol.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
	mcdVol.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mcdVol.dwControlID = 0;
	mcdVol.cChannels = 1;
	mcdVol.cMultipleItems = 0;

	// Mute
	memcpy(&mlcMute, &mlcVol, sizeof(mlcMute));
	mlcMute.dwControlType = MIXERCONTROL_CONTROLTYPE_MUTE;

	memcpy(&mcdMute, &mcdVol, sizeof(mcdMute));
	memset(&mcdb, 0, sizeof(mcdb));
	mcdMute.paDetails = &mcdb;
	mcdMute.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
}

void Volume::setupMixerControlDetails(HMIXER &mixer, MIXERLINECONTROLS &mlc, MIXERCONTROLDETAILS &mcd)
{
	if (mixerOpen(&mixer, 0, 0, 0, MIXER_OBJECTF_HMIXER) != MMSYSERR_NOERROR)
	{
		reportError("Couldn't open mixer");
		return;
	}

	if (mixerGetLineInfo((HMIXEROBJ)mixer, &ml, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE) != MMSYSERR_NOERROR)
	{
		reportError("Couldn't get line info");
		return;
	}

	mlc.dwLineID = ml.dwLineID;

	if (mixerGetLineControls((HMIXEROBJ)mixer, &mlc, 
		MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE) != 
		MMSYSERR_NOERROR)
	{
		reportError("Couldn't get line controls");
		return;
	}

	mcd.dwControlID = mc.dwControlID;

	if (mixerGetControlDetails((HMIXEROBJ)mixer, &mcd, MIXER_SETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR)
	{
		reportError("Couldn't get control details");
		return;
	}
}

void Volume::change(int steps)
{
	HMIXER mixer;
	setupMixerControlDetails(mixer, mlcVol, mcdVol);

	//full volume at 65535
	mcdu.dwValue = max(0, min((long)mcdu.dwValue + steps, 65535));

	if (mixerSetControlDetails((HMIXEROBJ)mixer, &mcdVol, MIXER_SETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR)
	{
		reportError("Couldn't set volume");
		return;
	}

	mixerClose(mixer);
}

void Volume::reportError(LPCSTR msg)
{
	//if (showErrors)
	{
		MessageBox(NULL, msg, "tVolEzy error", MB_OK | MB_ICONERROR);
	}
}

bool Volume::isMuted()
{
	HMIXER mixer;
	setupMixerControlDetails(mixer, mlcMute, mcdMute);

	return mcdb.fValue;
}

void Volume::setMuted(bool mute)
{
	HMIXER mixer;
	setupMixerControlDetails(mixer, mlcMute, mcdMute);

	mcdb.fValue = mute;

	if (mixerSetControlDetails((HMIXEROBJ)mixer, &mcdMute, MIXER_SETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR)
	{
		reportError("Couldn't set volume");
		return;
	}

	mixerClose(mixer);
}

void Volume::up(int steps)
{
	change(steps);
	setMuted(false);
}

void Volume::down(int steps)
{
	change(-steps);
}

void Volume::toggleMute()
{
	setMuted(!isMuted());
}