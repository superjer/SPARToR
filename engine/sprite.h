
#ifndef SPARTOR_SPRITE_H_
#define SPARTOR_SPRITE_H_


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


int load_sprites(int texnum);
void unload_sprites();
int find_sprite_by_name(const char *name);


#endif

