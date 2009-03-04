#include "volume.h"
#include <windows.h>

Volume::Volume()
{
	setupMixers();
}

Volume::~Volume()
{
	if (mixerVol)
	{
		mixerClose(mixerVol);
	}

	if (mixerMute)
	{
		mixerClose(mixerMute);
	}
}

void Volume::setupMixers()
{
	mixerVol = NULL;

	memset(&mlVol, 0, sizeof(mlVol));
	mlVol.cbStruct = sizeof(MIXERLINE);
	mlVol.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;

	memset(&mcVol, 0, sizeof(mcVol));
	memset(&mlcVol, 0, sizeof(mlcVol));
	mlcVol.dwControlType = MIXERCONTROL_CONTROLTYPE_VOLUME;
	mlcVol.cbStruct = sizeof(MIXERLINECONTROLS);
	mlcVol.dwLineID = mlVol.dwLineID;
	mlcVol.cControls = 1;
	mlcVol.cbmxctrl = sizeof(MIXERCONTROL);
	mlcVol.pamxctrl = &mcVol;

	memset(&mcdVol, 0, sizeof(mcdVol));
	memset(&mcdu, 0, sizeof(mcdu));
	mcdVol.paDetails = &mcdu;
	mcdVol.cbDetails = sizeof(MIXERCONTROLDETAILS_UNSIGNED);
	mcdVol.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mcdVol.dwControlID = mcVol.dwControlID;
	mcdVol.cChannels = 1;
	mcdVol.cMultipleItems = 0;

	mixerMute = NULL;

	memset(&mlMute, 0, sizeof(mlMute));
	mlMute.cbStruct = sizeof(MIXERLINE);
	mlMute.dwComponentType = MIXERLINE_COMPONENTTYPE_DST_SPEAKERS;

	memset(&mcMute, 0, sizeof(mcMute));
	memset(&mlcMute, 0, sizeof(mlcMute));
	mlcMute.dwControlType = MIXERCONTROL_CONTROLTYPE_MUTE;
	mlcMute.cbStruct = sizeof(MIXERLINECONTROLS);
	mlcMute.dwLineID = mlMute.dwLineID;
	mlcMute.cControls = 1;
	mlcMute.cbmxctrl = sizeof(MIXERCONTROL);
	mlcMute.pamxctrl = &mcMute;

	memset(&mcdMute, 0, sizeof(mcdMute));
	memset(&mcdb, 0, sizeof(mcdb));
	mcdMute.paDetails = &mcdb;
	mcdMute.cbDetails = sizeof(MIXERCONTROLDETAILS_BOOLEAN);
	mcdMute.cbStruct = sizeof(MIXERCONTROLDETAILS);
	mcdMute.dwControlID = mcMute.dwControlID;
	mcdMute.cChannels = 1;
	mcdMute.cMultipleItems = 0;
}

void Volume::change(int steps)
{
	if (mixerOpen(&mixerVol, 0, 0, 0, MIXER_OBJECTF_HMIXER) != MMSYSERR_NOERROR)
	{
		reportError("Couldn't open mixer");
		return;
	}

	if (mixerGetLineInfo((HMIXEROBJ)mixerVol, &mlVol, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE) != MMSYSERR_NOERROR)
	{
		reportError("Couldn't get line info");
		return;
	}

	mlcVol.dwLineID = mlVol.dwLineID;

	if (mixerGetLineControls((HMIXEROBJ)mixerVol, &mlcVol, 
		MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE) != 
		MMSYSERR_NOERROR)
	{
		reportError("Couldn't get line controls");
		return;
	}

	mcdVol.dwControlID = mcVol.dwControlID;

	if (mixerGetControlDetails((HMIXEROBJ) mixerVol, &mcdVol, MIXER_SETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR)
	{
		reportError("Couldn't get control details");
		return;
	}

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

	//mcdu.dwValue = max(0, min(mcdu.dwValue + steps, 65535));

	if (mixerSetControlDetails((HMIXEROBJ)mixerVol, &mcdVol, MIXER_SETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR)
	{
		reportError("Couldn't set volume");
		return;
	}

	mixerClose(mixerVol);
}

void Volume::reportError(LPCSTR msg)
{
	//if (showErrors)
	{
		MessageBox(NULL, msg, "tVolEzy error", MB_OK | MB_ICONERROR);
	}
}

void Volume::toggleMute()
{
	if (mixerOpen(&mixerMute, 0, 0, 0, MIXER_OBJECTF_HMIXER) != MMSYSERR_NOERROR)
	{
		reportError("Couldn't open mixer");
		return;
	}

	if (mixerGetLineInfo((HMIXEROBJ)mixerMute, &mlMute, MIXER_OBJECTF_HMIXER | MIXER_GETLINEINFOF_COMPONENTTYPE) != MMSYSERR_NOERROR)
	{
		reportError("Couldn't get line info");
		return;
	}

	mlcMute.dwLineID = mlMute.dwLineID;

	if (mixerGetLineControls((HMIXEROBJ)mixerMute, &mlcMute, 
		MIXER_OBJECTF_HMIXER | MIXER_GETLINECONTROLSF_ONEBYTYPE) != 
		MMSYSERR_NOERROR)
	{
		reportError("Couldn't get line controls");
		return;
	}

	mcdMute.dwControlID = mcMute.dwControlID;

	if (mixerGetControlDetails((HMIXEROBJ) mixerMute, &mcdMute, MIXER_SETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR)
	{
		reportError("Couldn't get control details");
		return;
	}

	mcdb.fValue = !mcdb.fValue;

	if (mixerSetControlDetails((HMIXEROBJ)mixerMute, &mcdMute, MIXER_SETCONTROLDETAILSF_VALUE) != MMSYSERR_NOERROR)
	{
		reportError("Couldn't set volume");
		return;
	}

	mixerClose(mixerMute);
}

void Volume::up(int steps)
{
	change(steps);
}

void Volume::down(int steps)
{
	change(-steps);
}