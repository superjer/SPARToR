
#ifndef SPARTOR_SPRITE_H_
#define SPARTOR_SPRITE_H_

#include "basictypes.h"
#include "spr_enum.h"

#define SPR_IDENTIFIER(X) SPR__ ## X
#define SM(X) sprites[spr_map[SPR__ ## X]]

enum { SPRITE_ENUM(SPR_IDENTIFIER), sprite_enum_max };


//tool textures and sprite flags
enum { TOOL_NUL = 1,
       TOOL_SOL,
       TOOL_PLAT,
       TOOL_OPN,
       TOOL_COPY,
       TOOL_PSTE,
       TOOL_OBJ,
       TOOL_ERAS,
       TOOL_VIS,
       TOOL_MASK = 0xFF,
       SPRF_TOP = 0x100,
       SPRF_MID = 0x200,
       SPRF_BOT = 0x400,
       SPRF_ALIGNYMASK = SPRF_TOP | SPRF_MID | SPRF_BOT,
       SPRF_LFT = 0x0800,
       SPRF_CEN = 0x1000,
       SPRF_RGT = 0x2000,
       SPRF_ALIGNXMASK = 0x3800,
       SPRF_ALIGNMASK = 0x3F00,
       SPRF_FLIPX = 0x4000,
       SPRF_FLIPY = 0x8000,
       SPRF_FLOOR = 0x10000 };

typedef struct {
  int      texnum;
  int      bump;
  int      flange;
  char    *name;
  REC      rec;
  int      ancx; // anchor position
  int      ancy;
  unsigned flags;
  struct {
    int gridwide;
    int gridlast;
    int piping;
    int stretch;
    int stretch_t;
    int stretch_r;
    int stretch_b;
    int stretch_l;
  } *more;
} SPRITE_T;


extern SPRITE_T  *sprites;
extern size_t     spr_count;
extern char      *spr_names[];
extern int        spr_map[];

extern SPRITE_T  *old_sprites;
extern size_t     old_spr_count;


void sprblit( SPRITE_T *spr, int x, int y );
void sprblit3d( SPRITE_T *spr, int x, int y, int z );
int load_sprites(int texnum);
void reload_sprites();
void unload_sprites(SPRITE_T *sprites, size_t spr_count);
int find_sprite_by_name(const char *name);


#endif

