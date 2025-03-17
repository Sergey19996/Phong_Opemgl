#ifndef SOUND_DEVICE_H
#define SOUND_DEVICE_H

#include "OpenAl/alc.h"

class SoundDevice
{
public:
	static SoundDevice* get();
private:
	SoundDevice();
	~SoundDevice();

	ALCdevice* p_ALCDevice;
	ALCcontext* p_ALCContext;



};

#endif // !SOUND_DEVICE_H
