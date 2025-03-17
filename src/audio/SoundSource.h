#ifndef SOUND_SOURCE_H
#define SOUND_SOURCE_H
#include "OpenAl/al.h"

class SoundSource
{
public:
	SoundSource();
	~SoundSource();
	void generate();
	void Play(const ALuint buffer_to_play);
	

private:
	ALuint p_source;
	float p_pitch = 1.0f;
	float p_gain = 1.0f;
	float p_position[3] = { 0,0,0 };
	float p_velocity[3] = { 0,0,0 };
	bool p_loopSound = false;
	ALuint p_Buffer = 0;
		
	




};




#endif // !SOUND_SOURCE_H

