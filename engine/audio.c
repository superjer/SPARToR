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

struct sample {
    Uint8 *data;
    Uint32 dpos;
    Uint32 dlen;
} playing[NUM_PLAYING];

typedef struct {
  char *name;
  SDL_AudioCVT cvt;
} SOUND_T;


static int sound_count = 0;
static int sound_alloc = 0;
static SOUND_T *sounds = NULL;
static int inited = 0;
static SDL_AudioSpec spec;


void mixaudio(void *unused, Uint8 *stream, int len)
{
  int i,j;
  Uint32 amount;
  int buf[len];
  static int fase;
  static int freq = 200;
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

  /*
  unsigned int bytesperperiod = spec.freq / freq;
  for( j=0; j<len/2; j++ ) {
    buf[j] += (int)(70 * sin(fase*6.28/bytesperperiod));
    fase = (fase+1) % bytesperperiod;
  }

  freq += 10;
  if( freq > 1000 ) freq = 200;
  */

  for( j=0; j<len/2; j++ ) {
    if(      buf[j] >  65535 ) out[j] = (Sint16) 65535;
    else if( buf[j] < -65536 ) out[j] = (Sint16)-65536;
    else                       out[j] = (Sint16)buf[j];
  }
}

void audioinit()
{
  if( inited ) audiodestroy();

  size_t i;

  SDL_AudioSpec desired;

  desired.freq = 22050;
  desired.format = AUDIO_S16LSB;
  desired.channels = 1;
  desired.samples = 512;
  desired.callback = mixaudio;
  desired.userdata = NULL;

  fprintf(stderr,"Initing audio\n");
  if( SDL_OpenAudio(&desired, &spec) < 0 ) {
    fprintf(stderr,"Init audio error\n");
    SJC_Write("Unable to open audio: %s\n", SDL_GetError());
    return;
  } else {
    fprintf(stderr,"Init audio OK\n");
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
  fprintf(stderr,"Audio freq: %d  format: %s  channels: %d  silence: %d  samples: %d  size: %d\n",
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


