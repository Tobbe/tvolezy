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
		ERROR_SETDETAILS};

	Volume(const TveSettings &settings) : settings(settings) {}
	virtual ~Volume() {}
	virtual bool up(int steps) = 0;
	virtual bool down(int steps) = 0;
	virtual bool toggleMute() = 0;
	virtual int getError() const = 0;
};

#endif