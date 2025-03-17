#include "SoundBuffer.h"
#include  <sndfile.h>
#include <inttypes.h>
#include  "OpenAl/alext.h"
SoundBuffer* SoundBuffer::get()
{

	static SoundBuffer* buffer = new SoundBuffer();
	return buffer;

}

ALuint SoundBuffer::addSoundEffect(const char* filename)
{
	ALenum err, format;
	ALuint buffer;
	SNDFILE* sndFile;
	SF_INFO sfinfo;
	short* membuf;
	sf_count_t num_frames;
	ALsizei num_bytes;

	// open the audio file and check that it's usable
	sndFile = sf_open(filename, SFM_READ, &sfinfo);

	if (!sndFile) {
		fprintf(stderr, "Could not open audio in %s: %s\n", filename, sf_strerror(sndFile));
		return 0;
	}
	if (sfinfo.frames < 1 || sfinfo.frames >(sf_count_t)(INT_MAX / sizeof(short)) / sfinfo.channels) {
		fprintf(stderr, "Bad sample count in %s (%" PRId64 ")\n", filename, sfinfo.frames);
		sf_close(sndFile);

		return 0;
	}

	//Get the sound format and figure out the OpenAL format
	format = AL_NONE;
	if (sfinfo.channels == 1)
		format = AL_FORMAT_MONO16;
	else if (sfinfo.channels == 2)
		format = AL_FORMAT_STEREO16;
	else if (sfinfo.channels == 3)
	{
		if (sf_command(sndFile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT)
			format = AL_FORMAT_BFORMAT2D_16;
	}
	if (!format) {

		fprintf(stderr, "Unupported channel count: %d\n", sfinfo.channels);
		sf_close(sndFile);
		return 0;

	}

	//Decode the whole audio file to a buffer
	membuf = static_cast<short*>(malloc((size_t)(sfinfo.frames * sfinfo.channels) * sizeof(short)));

	num_frames = sf_readf_short(sndFile, membuf, sfinfo.frames);
	if (num_frames < 1) {

		free(membuf);
		sf_close(sndFile);
		fprintf(stderr, "Failed to read samples in %s (%" PRId64 ")\n", filename, num_frames);
		return 0;

	}
	num_bytes = (ALsizei)(num_frames * sfinfo.channels) * (ALsizei)sizeof(short);

	//Buffer the audio data into a new buffer object, then free the data and Close the file
	buffer = 0;
	alGenBuffers(1, &buffer);
	alBufferData(buffer, format, membuf, num_bytes, sfinfo.samplerate);

	free(membuf);
	sf_close(sndFile);

	//check if an error occured, and clean up if so.
	err = alGetError();
	if (err != AL_NO_ERROR)
	{
		fprintf(stderr, "OpenA: Error: %s\n", alGetString(err));
		if (buffer && alIsBuffer(buffer))
			alDeleteBuffers(1, &buffer);
		return 0;
	}
	p_SoundEffectBuffers.push_back(buffer); //add to the list of known buffers

	return buffer;

}

bool SoundBuffer::removeSoundEffect(const ALuint& buffer)
{
	auto it = p_SoundEffectBuffers.begin();
	while (it != p_SoundEffectBuffers.end())
	{
		if (*it == buffer) {
			alDeleteBuffers(1, &*it);
			it = p_SoundEffectBuffers.erase(it);
			return true;
		}
		else
		{
			++it;
		}


	}
	return false; // coludn't find to remove
}

SoundBuffer::SoundBuffer(){
	
	p_SoundEffectBuffers.clear();

}

SoundBuffer::~SoundBuffer(){
	alDeleteBuffers(p_SoundEffectBuffers.size(), p_SoundEffectBuffers.data());
	p_SoundEffectBuffers.clear();
}
