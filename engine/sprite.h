
#ifndef SPARTOR_SPRITE_H_
#define SPARTOR_SPRITE_H_


typedef struct {
  int    texnum;
  char  *name;
  REC    rec;
  int    ancx,ancy; // anchor position
} SPRITE_T;


extern SPRITE_T  *sprites;
extern size_t     spr_count;


int load_sprites(int texnum);
void unload_sprites();


#endif

