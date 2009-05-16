#ifndef VOLUME_H_
#define VOLUME_H_

#include "tvesettings.h"

class Volume
{
protected:
	const TveSettings &settings;
	int error;

public:
	enum {ERROR_NOERROR,
		ERROR_OPENMIXER,
		ERROR_LINEINFO,
		ERROR_LINECONTROLS,
		ERROR_CONTROLDETAILS,
		ERROR_SETDETAILS,
		ERROR_GETMUTE,
		ERROR_SETMUTE,
		ERROR_GETVOL,
		ERROR_SETVOL,
		ERROR_PPINTERFACENULL,
		ERROR_PPDEVICENULL,
		ERROR_OUTOFRANGE,
		ERROR_NOTFOUND,
		ERROR_OUTOFMEM,
		ERROR_GETDEFAULT,
		ERROR_NOIFACE,
		ERROR_INVALIDARG,
		ERROR_REMOVED,
		ERROR_ACTIVATE};

	Volume(const TveSettings &settings) : settings(settings) {}
	virtual ~Volume() {}
	virtual bool up(int steps) = 0;
	virtual bool down(int steps) = 0;
	virtual bool toggleMute() = 0;
	virtual int getError() const = 0;
};

#endif