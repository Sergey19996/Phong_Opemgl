#ifndef SOUND_BUFFER_H
#define SOUND_BUFFER_H
#include "OpenAl/al.h"
#include <vector>

class SoundBuffer
{
public:
	static SoundBuffer* get();

	ALuint addSoundEffect(const char* filename);
	bool removeSoundEffect(const ALuint& buffer);

private:
	SoundBuffer();
	~SoundBuffer();
std::vector<ALuint> p_SoundEffectBuffers;



};

#endif // !SPUND_BUFFER_H