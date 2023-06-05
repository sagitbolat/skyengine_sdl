#pragma once
#include <stdio.h>
#include <stdint.h>

#include <AL/al.h>
#include <AL/alc.h>

#include "stb/stb_vorbis.c"


struct AudioContext {
    ALCdevice* device;
    ALCcontext* context;
};

struct SoundClip {
    ALuint al_sound_buffer; // handle to the OpenAL buffer
};

struct AudioSource {
    bool is_looping;
    float volume;
    ALuint al_source; // handle to the OpenAL source
    bool IsPlaying();
};

bool AudioSource::IsPlaying() {
    ALint source_state;
    
    alGetSourcei(this->al_source, AL_SOURCE_STATE, &source_state);
    
    if (source_state == AL_PLAYING) {

        return true;
    }

    return false;
}


AudioContext InitAudioContext() {
    AudioContext ac = {0};

    // NOTE: Open device
    ac.device = alcOpenDevice(NULL);

    if (!ac.device) {
        printf("Failed to open OpenAL device\n");
    }

    ac.context = alcCreateContext(ac.device, NULL);

    if (!ac.context) {
        printf("Failed to create OpenAL context\n");
        alcCloseDevice(ac.device);
    }
    
    alcMakeContextCurrent(ac.context);

    return ac;
}


SoundClip LoadSoundClip(const char* sound_file_path) {
    SoundClip clip = {0};

    // NOTE: Load the ogg file:
    FILE* file = fopen(sound_file_path, "rb");


    stb_vorbis* vorbis = stb_vorbis_open_file(file, 0, 0, NULL);
    stb_vorbis_info info = stb_vorbis_get_info(vorbis);
    int sample_count = stb_vorbis_stream_length_in_samples(vorbis);

    int buffer_size = info.channels * sample_count * sizeof(short);
    short* buffer_data = (short*)malloc(buffer_size);

    stb_vorbis_get_samples_short_interleaved(
        vorbis, info.channels, buffer_data, buffer_size / sizeof(short));

    stb_vorbis_close(vorbis);
    fclose(file);

    // NOTE: Init OpenAL buffer
    alGenBuffers(1, &(clip.al_sound_buffer));
    
    ALenum format;
    if (info.channels == 1) {
        format = AL_FORMAT_MONO16;
    } else {
        format = AL_FORMAT_STEREO16;
    }

    alBufferData(clip.al_sound_buffer, format, buffer_data, buffer_size, info.sample_rate);

    free(buffer_data);

    return clip;
}


AudioSource LoadAudioSource(bool source_looping = false, float volume = 1.0f) {
    AudioSource source = {0};
    source.is_looping = source_looping;
    source.volume = volume;

    alGenSources(1, &(source.al_source));
    
    if (source.is_looping) alSourcei(source.al_source, AL_LOOPING, AL_TRUE);
    else alSourcei(source.al_source, AL_LOOPING, AL_FALSE);
    alSourcef(source.al_source, AL_GAIN, source.volume);
    
    return source; 
}


// NOTE: Stops currently playing sound from source if any, and plays this sound instead.
// Example usecase: Playing a shooting sound the moment the player shoots.
// Its fine to not let previous shooting sound not finish, because we want instant audio feedback
// to player actions, hence we interrupt whatever is playing, and play the shooting sound immediately
void PlaySoundFromSource(const AudioSource* source, const SoundClip* clip) {

    // NOTE: If source is playing a sound, stop it:
    ALint source_state;
    alGetSourcei(source->al_source, AL_SOURCE_STATE, &source_state);
    if (source_state == AL_PLAYING) {
        alSourceStop(source->al_source);
    } 
    
    // NOTE: Set source parameters 
    if (source->is_looping) alSourcei(source->al_source, AL_LOOPING, AL_TRUE);
    else alSourcei(source->al_source, AL_LOOPING, AL_FALSE);
    alSourcef(source->al_source, AL_GAIN, source->volume);
    
    // NOTE: Refresh buffer to 0:
    alSourcei(source->al_source, AL_BUFFER, 0);

    // NOTE: Attach buffer to source
    alSourcei(source->al_source, AL_BUFFER, clip->al_sound_buffer);

    // NOTE: Play the sound from source
    alSourcePlay(source->al_source);
}


void FreeAudioSource(AudioSource* source) {
    alDeleteSources(1, &(source->al_source));
}

void FreeSoundClip(SoundClip* sound_clip) {
    alDeleteBuffers(1, &(sound_clip->al_sound_buffer));
}


void FreeAudioContext(AudioContext ac) {
    alcMakeContextCurrent(NULL);
    alcDestroyContext(ac.context);
    alcCloseDevice(ac.device);
}
