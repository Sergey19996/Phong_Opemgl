#include "SoundDevice.h"
#include "OpenAl/al.h"
#include <stdio.h>
SoundDevice* SoundDevice::get()
{
    static SoundDevice* snd_device = new SoundDevice();
    return snd_device;
}
SoundDevice::SoundDevice()
{
    p_ALCDevice = alcOpenDevice(nullptr); // get default device
    if (!p_ALCDevice)
        throw("failed to get sound device");

    p_ALCContext = alcCreateContext(p_ALCDevice, nullptr); // set context
    if (!p_ALCContext)
        throw("failed to set sound context");

    if (!alcMakeContextCurrent(p_ALCContext)) // make context curremt
        throw("failed to make context current");

    const ALCchar* name = nullptr;
    if (alcIsExtensionPresent(p_ALCDevice, "ALC_ENUMERATE_ALL_EXT"))
        name = alcGetString(p_ALCDevice, ALC_ALL_DEVICES_SPECIFIER);
    if (!name || alcGetError(p_ALCDevice) != ALC_NO_ERROR)
        name = alcGetString(p_ALCDevice, ALC_DEVICE_SPECIFIER);
    printf("Opened \"%s\"\n", name);

}
SoundDevice::~SoundDevice()
{
}

