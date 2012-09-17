
#ifndef SPARTOR_AUDIO_H_
#define SPARTOR_AUDIO_H_


extern int *a_waveform;
extern int  a_waveform_len;
extern int  a_musictest;


void audioinit();
void audiodestroy();
void play(const char *file);
void make_sure_sound_is_loaded(const char *file);


#endif

