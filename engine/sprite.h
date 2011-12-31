
#ifndef SPARTOR_SPRITE_H_
#define SPARTOR_SPRITE_H_

#include "spr_enum.h"

#define SPR_IDENTIFIER(X) SPR__ ## X
#define SM(X) sprites[spr_map[SPR__ ## X]]

enum { SPRITE_ENUM(SPR_IDENTIFIER), sprite_enum_max };


//tool textures
enum { TOOL_NUL = 1,
       TOOL_SOL,
       TOOL_PLAT,
       TOOL_OPN,
       TOOL_COPY,
       TOOL_PSTE,
       TOOL_OBJ,
       TOOL_ERAS,
       TOOL_VIS,
       TOOL_MASK = 255 };


typedef struct {
  int    texnum;
  char  *name;
  short  flags;
  REC    rec;
  int    ancx,ancy; // anchor position
} SPRITE_T;



extern SPRITE_T  *sprites;
extern size_t     spr_count;
extern char      *spr_names[];
extern int        spr_map[];


int load_sprites(int texnum);
void unload_sprites();
int find_sprite_by_name(const char *name);


#endif

