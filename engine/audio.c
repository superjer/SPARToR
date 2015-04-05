/**
 **  SPARToR
 **  Network Game Engine
 **  Copyright (C) 2010-2012  Jer Wilson
 **
 **  See COPYING for details.
 **
 **  http://www.superjer.com/
 **  http://www.spartor.com/
 **  http://github.com/superjer/SPARToR
 **/

#include <GL/glew.h>
#include "SDL.h"
#include "SDL_audio.h"
#include "mod.h"
#include "main.h"
#include "console.h"
#include "font.h"
#include "audio.h"
#include "sjglob.h"


#define NUM_PLAYING 2

#define WAVEFORM_LEN 1024
#define FULL_MULT 32

#define TWELFTH_ROOT 1.059463094

#define NUM_ENVS 20

struct sample {
    Uint8 *data;
    Uint32 dpos;
    Uint32 dlen;
} playing[NUM_PLAYING];

typedef struct {
  char *name;
  SDL_AudioCVT cvt;
} SOUND_T;

int *a_waveform     = NULL;
int  a_waveform_len = WAVEFORM_LEN;
int  a_musictest    = 0;

static int full_waveform[WAVEFORM_LEN*FULL_MULT] = {0};
static int full_waveform_len = WAVEFORM_LEN*FULL_MULT;
static int full_waveform_pos = 0;

static int sound_count = 0;
static int sound_alloc = 0;
static SOUND_T *sounds = NULL;
static int inited = 0;
static SDL_AudioSpec spec;

enum WVSHAPE { SINE, SQUARE, TRIANGLE, SAWTOOTH, NOISE, NUM_SHAPES };

typedef struct {
  enum WVSHAPE shape;
  double start_freq;
  double volume;
  double attack, decay, sustain, release, attack_ratio;
  double duty_start, duty_end, duty_speed, duty_accel;
  double tremolo_steps,    tremolo_period, tremolo_speed,    tremolo_accel;
  double vibrato_steps,    vibrato_period, vibrato_speed,    vibrato_accel;
  double portamento_steps, portamento_len, portamento_speed, portamento_accel;
  double glissando_steps,  glissando_len,  glissando_speed,  glissando_accel;
  double pos, duty;
} ENVELOPE;

ENVELOPE envs[NUM_ENVS];

struct {
  char name[3];
  int octave;
  double frequency;
  double wavelength;
} music_notes[] = {
/*{"C" ,0,16.351   ,20.812},
  {"C#",0,17.324   ,19.643},
  {"D" ,0,18.354   ,18.540},
  {"D#",0,19.445   ,17.500},
  {"E" ,0,20.601   ,16.518},
  {"F" ,0,21.827   ,15.590},
  {"F#",0,23.124   ,14.716},
  {"G" ,0,24.499   ,13.890},
  {"G#",0,25.956   ,13.110},*/
  {"A" ,0,27.5     ,12.374}, // Piano low
  {"A#",0,29.135   ,11.680},
  {"B" ,0,30.868   ,11.024},
  {"C" ,1,32.703   ,10.405},
  {"C#",1,34.648   ,9.821 },
  {"D" ,1,36.708   ,9.270 },
  {"D#",1,38.891   ,8.750 },
  {"E" ,1,41.203   ,8.259 },
  {"F" ,1,43.654   ,7.795 },
  {"F#",1,46.249   ,7.358 },
  {"G" ,1,48.999   ,6.945 },
  {"G#",1,51.913   ,6.555 },
  {"A" ,1,55       ,6.187 },
  {"A#",1,58.27    ,5.840 },
  {"B" ,1,61.735   ,5.512 },
  {"C" ,2,65.406   ,5.203 },
  {"C#",2,69.296   ,4.911 },
  {"D" ,2,73.416   ,4.635 },
  {"D#",2,77.782   ,4.375 },
  {"E" ,2,82.407   ,4.129 },
  {"F" ,2,87.307   ,3.898 },
  {"F#",2,92.499   ,3.679 },
  {"G" ,2,97.999   ,3.472 },
  {"G#",2,103.826  ,3.278 },
  {"A" ,2,110      ,3.094 },
  {"A#",2,116.541  ,2.920 },
  {"B" ,2,123.471  ,2.756 },
  {"C" ,3,130.813  ,2.601 },
  {"C#",3,138.591  ,2.455 },
  {"D" ,3,146.832  ,2.318 },
  {"D#",3,155.563  ,2.187 },
  {"E" ,3,164.814  ,2.065 },
  {"F" ,3,174.614  ,1.949 },
  {"F#",3,184.997  ,1.839 },
  {"G" ,3,195.998  ,1.736 },
  {"G#",3,207.652  ,1.639 },
  {"A" ,3,220      ,1.547 },
  {"A#",3,233.082  ,1.460 },
  {"B" ,3,246.942  ,1.378 },
  {"C" ,4,261.626  ,1.301 }, // middle C
  {"C#",4,277.183  ,1.228 },
  {"D" ,4,293.665  ,1.159 },
  {"D#",4,311.127  ,1.094 },
  {"E" ,4,329.628  ,1.032 },
  {"F" ,4,349.228  ,0.974 },
  {"F#",4,369.994  ,0.920 },
  {"G" ,4,391.995  ,0.868 },
  {"G#",4,415.305  ,0.819 },
  {"A" ,4,440      ,0.773 }, // A 440
  {"A#",4,466.164  ,0.730 },
  {"B" ,4,493.883  ,0.689 },
  {"C" ,5,523.251  ,0.650 },
  {"C#",5,554.365  ,0.614 },
  {"D" ,5,587.33   ,0.579 },
  {"D#",5,622.254  ,0.547 },
  {"E" ,5,659.255  ,0.516 },
  {"F" ,5,698.456  ,0.487 },
  {"F#",5,739.989  ,0.460 },
  {"G" ,5,783.991  ,0.434 },
  {"G#",5,830.609  ,0.410 },
  {"A" ,5,880      ,0.387 },
  {"A#",5,932.328  ,0.365 },
  {"B" ,5,987.767  ,0.345 },
  {"C" ,6,1046.502 ,0.325 },
  {"C#",6,1108.731 ,0.307 },
  {"D" ,6,1174.659 ,0.290 },
  {"D#",6,1244.508 ,0.273 },
  {"E" ,6,1318.51  ,0.258 },
  {"F" ,6,1396.913 ,0.244 },
  {"F#",6,1479.978 ,0.230 },
  {"G" ,6,1567.982 ,0.217 },
  {"G#",6,1661.219 ,0.205 },
  {"A" ,6,1760     ,0.193 },
  {"A#",6,1864.655 ,0.182 },
  {"B" ,6,1975.533 ,0.172 },
  {"C" ,7,2093.005 ,0.163 },
  {"C#",7,2217.461 ,0.153 },
  {"D" ,7,2349.318 ,0.145 },
  {"D#",7,2489.016 ,0.137 },
  {"E" ,7,2637.021 ,0.129 },
  {"F" ,7,2793.826 ,0.122 },
  {"F#",7,2959.955 ,0.115 },
  {"G" ,7,3135.964 ,0.109 },
  {"G#",7,3322.438 ,0.102 },
  {"A" ,7,3520     ,0.097 },
  {"A#",7,3729.31  ,0.091 },
  {"B" ,7,3951.066 ,0.086 },
  {"C" ,8,4186.009 ,0.081 }, // Piano high
/*{"C#",8,4434.922 ,0.077 },
  {"D" ,8,4698.636 ,0.072 },
  {"D#",8,4978.032 ,0.068 },
  {"E" ,8,5274.042 ,0.065 },
  {"F" ,8,5587.652 ,0.061 },
  {"F#",8,5919.91  ,0.057 },
  {"G" ,8,6271.928 ,0.054 },
  {"G#",8,6644.876 ,0.051 },
  {"A" ,8,7040     ,0.048 },
  {"A#",8,7458.62  ,0.046 },
  {"B" ,8,7902.132 ,0.043 },
  {"C" ,9,8372.018 ,0.041 },
  {"C#",9,8869.844 ,0.038 },
  {"D" ,9,9397.272 ,0.036 },
  {"D#",9,9956.064 ,0.034 },
  {"E" ,9,10548.084,0.032 },
  {"F" ,9,11175.304,0.030 },
  {"F#",9,11839.82 ,0.029 },
  {"G" ,9,12543.856,0.027 },
  {"G#",9,13289.752,0.026 },
  {"A" ,9,14080    ,0.024 },
  {"A#",9,14917.24 ,0.023 },
  {"B" ,9,15804.264,0.022 },*/
};
#define NUM_MUSIC_NOTES (sizeof music_notes / sizeof *music_notes)

static void music_test( int *buf, int len )
{
/*
  static int freq_time = 100;
  if( ++freq_time >= 100 ) {
    freq_time = 0;
*/
  if( rand()%100==0 ) {
    ENVELOPE *e = envs + rand()%NUM_ENVS;
    memset(e,0,sizeof *e);

    int note = rand() % NUM_MUSIC_NOTES;

    e->shape      = rand() % NOISE;
    e->start_freq = music_notes[note].frequency;
    e->volume     = 1.0 + 0.08 * music_notes[note].wavelength;

    e->attack_ratio = (double)(rand()%100) * 0.01 + 1.0;

    double len = music_notes[note].wavelength * 0.01;
    if( len > 0.015 ) len = 0.015;

    e->attack  = (rand()%4==0 ? (double)(rand()%100) * len : 0.0) + 0.05;
    e->decay   = (rand()%4==0 ? (double)(rand()%100) * len : 0.0) + e->attack;
    e->sustain = (rand()%4==0 ? (double)(rand()%300) * len : 0.0) + e->decay;
    e->release = (rand()%4==0 ? (double)(rand()%300) * len : 0.0) + e->sustain;

    if( e->release < 0.01 ) e->release = 1.0;

    e->duty_start   = (double[4]){0.5,0.25,0.125,0.0625}[rand()%4];
    e->duty_end     = e->duty_start;
    if( rand()%4==0 ) {
      e->duty_end   = (double[4]){0.5,0.25,0.125,0.0625}[rand()%4];
      e->duty_speed = (double)(rand()%100) * 0.00001;
      if( rand()%2 )
        e->duty_accel = (double)(rand()%100) * 0.0000001;
    }

    if( rand()%4==0 ) {
      e->tremolo_steps  = rand()%49 - 24;
      e->tremolo_period = (double)(rand()%100) * 0.01;
      if( rand()%3 ) {
        e->tremolo_speed = (double)(rand()%100) * 0.00001;
        if( rand()%2 )
          e->tremolo_accel = (double)(rand()%100) * 0.0000001;
      }
    }

    if( rand()%4==0 ) {
      e->vibrato_steps  = rand()%49 - 24;
      e->vibrato_period = (double)(rand()%100) * 0.01;
      if( rand()%3 ) {
        e->vibrato_speed = (double)(rand()%100) * 0.00001;
        if( rand()%2 )
          e->vibrato_accel = (double)(rand()%100) * 0.0000001;
      }
    }

    if( rand()%4==0 ) {
      e->portamento_steps = rand()%49 - 24;
      e->portamento_len   = (double)(rand()%100) * 0.01;
      if( rand()%3 ) {
        e->portamento_speed = (double)(rand()%100) * 0.00001;
        if( rand()%2 )
          e->portamento_accel = (double)(rand()%100) * 0.0000001;
      }
    }

    if( rand()%4==0 ) {
      e->glissando_steps = rand()%49 - 24;
      e->glissando_len   = (double)(rand()%100) * 0.01;
      if( rand()%3 ) {
        e->glissando_speed = (double)(rand()%100) * 0.00001;
        if( rand()%2 )
          e->glissando_accel = (double)(rand()%100) * 0.0000001;
      }
    }

    /*
    SJC_Write("Note: %2s%d %c ADSR=%2d,%2d,%2d,%2d %10s%c%.f",
        music_notes[note].name,
        music_notes[note].octave,
        vibrato ? 'V' : ' ',
        attack,decay,sustain,release,
        (char[NUM_SHAPES][10]){"SINE","SQUARE","TRIANGLE","SAWTOOTH","NOISE"}[waveshape],
        waveshape==SQUARE?'/':'\0',
        denom);
    */
  }

  int j,n;

  for( j=0; j<len/2; j++ ) {
    static double noiseval  =  0;
    static int    noisectr  =  1;
    static int    noisesign = -1;

    if( noisectr-- <= 0 ) {
      int minimum = 15;
      int range   = 150;
      noisectr  = minimum + rand()%range;
      noisesign = noisesign<0 ? 1 : -1;
      noiseval  = 1.0;
    }

    noiseval *= 0.995;

    for( n=0; n<NUM_ENVS; n++ ) {
      ENVELOPE *e = envs + n;

      if( e->pos >= e->release )
        continue;

      double t = e->pos;
      e->pos += 1.0/44100;

      double veloc = e->volume * 2200;
      if     ( t <= e->attack  ) veloc *= t/e->attack * e->attack_ratio;
      else if( t <= e->decay   ) veloc *= (e->decay-t)/(e->decay-e->attack) * (e->attack_ratio-1.0) + 1.0;
      else if( t <= e->sustain ) ;
      else if( t <= e->release ) veloc *= (e->release-t)/(e->release-e->sustain);

      //double tr_freq = e->start_freq * pow(TWELFTH_ROOT,e->tremolo_steps);
      //double vi_freq = e->start_freq * pow(TWELFTH_ROOT,e->vibrato_steps);

      e->tremolo_period += e->tremolo_speed;
      e->tremolo_speed  += e->tremolo_accel;

      e->vibrato_period += e->vibrato_speed;
      e->vibrato_speed  += e->vibrato_accel;

      double freq = e->start_freq; //FIXME!
      double wl   = 1.0/freq;
      double wl2  = wl/2.0;
      double frac = fmod(e->pos,wl);

      //if( e->shape==SINE ) fprintf(stderr,"%f / %f\n",frac,wl);
      switch( e->shape ) {
        case SINE:     buf[j] += veloc * sin(frac*freq*6.28318531);                  break;
        case SQUARE:   buf[j] += frac>wl2 ? veloc : -veloc;                          break;
        case TRIANGLE: buf[j] += (4*freq*(frac>wl2 ? (wl-frac) : frac) - 1) * veloc; break;
        case SAWTOOTH: buf[j] += (2*freq*frac - 1) * veloc;                          break;
        case NOISE:    buf[j] += (noisesign<0 ? -noiseval : noiseval) * veloc;       break;
        default: break;
      }
    }
  }
}

void mixaudio(void *unused, Uint8 *stream, int len)
{
  int i,j;
  Uint32 amount;
  int buf[len];
  Sint16 *out = (Sint16*)stream;

  memset(buf,0,sizeof *buf * len); // twice as big as it needs to be, if 16bit

  for( i=0; i<NUM_PLAYING; ++i ) {
    amount = playing[i].dlen-playing[i].dpos;
    if( amount > (Uint32)len )
      amount = len;
    for( j=0; j<(int)amount/2; j++ ) {
      Sint16 *in = (Sint16*)(playing[i].data + playing[i].dpos) + j;
      buf[j] += *in;
    }
    playing[i].dpos += amount;
  }

  if( a_musictest ) music_test(buf,len);

  for( j=0; j<len/2; j++ ) {
    if(      buf[j] >  32767 ) out[j] = (Sint16) 32767;
    else if( buf[j] < -32768 ) out[j] = (Sint16)-32768;
    else                       out[j] = (Sint16)buf[j];

    full_waveform[full_waveform_pos] = out[j];
    full_waveform_pos = (full_waveform_pos+1) % full_waveform_len;
    if( full_waveform_pos % a_waveform_len == 0 )
      a_waveform = full_waveform + (full_waveform_pos + (FULL_MULT-1)*a_waveform_len) % full_waveform_len;
  }
}

void audioinit()
{
  if( inited ) audiodestroy();

  size_t i;

  SDL_AudioSpec desired;

  desired.freq = 44100;
  desired.format = AUDIO_S16LSB;
  desired.channels = 1;
  desired.samples = 512;
  desired.callback = mixaudio;
  desired.userdata = NULL;

  if( SDL_OpenAudio(&desired, &spec) < 0 ) {
    SJC_Write("Unable to open audio: %s\n", SDL_GetError());
    return;
  } else {
    SDL_PauseAudio(0);
    inited = 1;
  }

  char *sfmt = NULL;
  switch( spec.format ) {
    #define SFMT(X) case X: sfmt = #X; break;
    SFMT(AUDIO_U8)
    SFMT(AUDIO_S8)
    SFMT(AUDIO_U16LSB)
    SFMT(AUDIO_S16LSB)
    SFMT(AUDIO_U16MSB)
    SFMT(AUDIO_S16MSB)
  }

  SJC_Write("Audio freq: %d  format: %s  channels: %d  silence: %d  samples: %d  size: %d",
            spec.freq, sfmt, spec.channels, spec.silence, spec.samples, spec.size);

  SJGLOB_T *files = SJglob( "game/sounds", "*.wav", SJGLOB_MARK|SJGLOB_NOESCAPE );
  for( i=0; i<files->gl_pathc; i++ )
    make_sure_sound_is_loaded( files->gl_pathv[i] );
  SJglobfree( files );
}

void audiodestroy()
{
  int i;
  for( i=0; i<sound_count; i++ )
    if( sounds[i].cvt.buf )
      free(sounds[i].cvt.buf);

  free(sounds);
  sounds = NULL;
  sound_count = 0;
  sound_alloc = 0;

  SDL_CloseAudio();

  inited = 0;
}

void play(const char *name)
{
  int i;
  SDL_AudioCVT *cvt = NULL;

  // Find sound by name
  for( i=0; i<sound_count; i++ )
    if( 0==strcmp(name,sounds[i].name) ) {
      cvt = &sounds[i].cvt;
      break;
    }

  if( !cvt ) {
    SJC_Write("Could not find sound: %s",name);
    return;
  }

  // Look for an empty (or finished) sound slot
  for( i=0; i<NUM_PLAYING; ++i )
    if( playing[i].dpos == playing[i].dlen )
      break;

  if( i == NUM_PLAYING )
    return;

  SDL_LockAudio();
  playing[i].data = cvt->buf;
  playing[i].dlen = cvt->len_cvt;
  playing[i].dpos = 0;
  SDL_UnlockAudio();
}

void make_sure_sound_is_loaded(const char *file)
{
  // Parse out name w/o dirs or extension
  const char *p = strrchr(file,'/');
  p = p ? p+1 : file;
  char *name = malloc(strlen(p)+1);
  strcpy(name,p);
  char *q;
  for( q=name; *q; q++ )
    if( *q=='.' ) *q = '\0';

  // Check if this sound is already loaded
  int i;
  for( i=0; i<sound_count; i++ )
    if( 0==strcmp(name,sounds[i].name) )
      goto fail;

  // Find an empty slot or make one
  if( sound_count >= sound_alloc ) {
    size_t new_alloc = sound_alloc < 8 ? 8 : sound_alloc*2;
    sounds = realloc( sounds, new_alloc * sizeof *sounds );
    memset( sounds + sound_alloc, 0, (new_alloc - sound_alloc) * sizeof *sounds );
    sound_alloc = new_alloc;
  }

  SOUND_T *s = sounds + sound_count;
  SDL_AudioSpec wave;
  Uint8 *data;
  Uint32 dlen;

  // Load the sound file and convert it to 16-bit stereo at 22kHz
  if( SDL_LoadWAV(file, &wave, &data, &dlen) == NULL ) {
    SJC_Write("Couldn't load %s: %s", file, SDL_GetError());
    goto fail;
  }

  s->name = name;

  SDL_BuildAudioCVT(&s->cvt, wave.format, wave.channels, wave.freq, spec.format, spec.channels, spec.freq);
  s->cvt.buf = malloc(dlen*s->cvt.len_mult);
  memcpy(s->cvt.buf, data, dlen);
  s->cvt.len = dlen;
  SDL_ConvertAudio(&s->cvt);
  SDL_FreeWAV(data);

  sound_count++;
  return;

  fail:
  free(name);
  return;
}
