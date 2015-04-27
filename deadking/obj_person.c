/**
 **  Dead Kings' Quest
 **  A special game for the SPARToR Network Game Engine
 **  Copyright (c) 2010-2012  Jer Wilson
 **
 **  See COPYING for details.
 **
 **  http://www.superjer.com/
 **  http://www.spartor.com/
 **  http://github.com/superjer/SPARToR
 **/

#include "obj_.h"

#define SPRITECOUNT 10

static void get_azma_sprites(   SPRITE_T **sprs, PERSON_t *pe);
static void get_gyllioc_sprites(SPRITE_T **sprs, PERSON_t *pe);
static void get_slug_sprites(   SPRITE_T **sprs, PERSON_t *pe);


void obj_person_draw( int objid, Uint32 vidfr, OBJ_t *o, CONTEXT_t *co )
{
  PERSON_t *pe = o->data;
  int i;
  SPRITE_T *sprs[SPRITECOUNT] = {NULL};

  switch( pe->character ) {
    case CHR_AZMA:    get_azma_sprites(   sprs,pe); break;
    case CHR_GYLLIOC: get_gyllioc_sprites(sprs,pe); break;
    case CHR_SLUG:    get_slug_sprites(   sprs,pe); break;
  }

  unsigned char amt = 255 - pe->hitcounter*9;
  if( pe->hitcounter ) glColor3ub(255,amt,amt);
  else                 glColor3ub(255,255,255);

  for( i=0; i<SPRITECOUNT; i++ )
    if( sprs[i] )
      sprblit3d( sprs[i], pe->pos.x, pe->pos.y, pe->pos.z );

  glColor3ub(255,255,255);

  sprblit3d( &SM(shadow), pe->pos.x, pe->pos.y, pe->pos.z );
}

void obj_person_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob )
{
  PERSON_t  *oldpe = oa->data;
  PERSON_t  *newpe = ob->data;
  CONTEXT_t *co = fr[b].objs[ob->context].data;
  MOTHER_t  *mo = fr[b].objs[0].data;

  enum DIR8 dir = NODIR;  // direction we want to move
  int stop = 0;           // whether to end the current turn

  spatt(hotfr);

  if( mo->intervalstart == hotfr )
    newpe->to += 10;    // TODO: use speed instead of "10"

  if( mo->active==objid ) {
    if( mo->turnstart == hotfr ) {
      newpe->to = 0;
      newpe->ap = newpe->max_ap;

      if( mo->pc )
        newpe->gait = RUNNING;

      #define PERSON_CLAMP(pe,attr,amt)       \
        (pe)->attr += (amt);                  \
        if( (pe)->attr > (pe)->max_ ## attr ) \
          (pe)->attr = (pe)->max_ ## attr;

      PERSON_CLAMP(newpe,st,10) // TODO: use 10 + 1/3 of might
      PERSON_CLAMP(newpe,mp, 0) // TODO: use f( lucidity, intrepidity )
      PERSON_CLAMP(newpe,pn, 1) // TODO: 1 + 1/3 of intrepidity
      PERSON_CLAMP(newpe,ml, 0) // TODO: 1/3 of intrepidity + 1/3 lucidity

      #undef PERSON_CLAMP
    }

    // check for input if player controlled
    if( mo->pc ) {
      GHOST_t *gh = fr[b].objs[mo->ghost].data;

      switch( fr[b].cmds[gh->client].cmd ) {
        case CMDT_1LEFT:  dir=W;  break;
        case CMDT_1RIGHT: dir=E;  break;
        case CMDT_1UP:    dir=N;  break;
        case CMDT_1DOWN:  dir=S;  break;
        case CMDT_1NW:    dir=NW; break;
        case CMDT_1NE:    dir=NE; break;
        case CMDT_1SW:    dir=SW; break;
        case CMDT_1SE:    dir=SE; break;
        case CMDT_1SEL:   stop=1; break;
        case CMDT_1BACK:          break;
      }
    } else { // npc
      if( newpe->ap >= 10 )
        dir = patt()%8 + 1;
      else
        stop = 1;
    }

    if( stop ) {
      mo->active = 0;
      newpe->pos.y -= 10; // hup!
    }
  }

  if( !oldpe ) { //FIXME why's this null?
    echo("Warning: oldpe is NULL!");
    return;
  }

  // determine desired movement
  int newx = newpe->tilex;
  int newz = newpe->tilez;
  switch( dir ) {
    case NODIR: break;
    case E : newx = newpe->tilex-1; newz = newpe->tilez+1; break;
    case W : newx = newpe->tilex+1; newz = newpe->tilez-1; break;
    case N : newx = newpe->tilex-1; newz = newpe->tilez-1; break;
    case S : newx = newpe->tilex+1; newz = newpe->tilez+1; break;
    case NE: newx = newpe->tilex-1;                        break;
    case NW:                        newz = newpe->tilez-1; break;
    case SE:                        newz = newpe->tilez+1; break;
    case SW: newx = newpe->tilex+1;                        break;
  }

  if( dir ) newpe->dir = dir;

  // move only if in-bounds
  if( dir && newx>=0 && newz>=0 && newx<co->x && newz<co->z ) {
    int i;
    int required_ap;
    int required_st;
    PERSON_t *obstructor = NULL;

    // FIXME make it easier to check for obstructions
    for( i=0; i<maxobjs; i++ ) {
      if( fr[b].objs[i].type==OBJT_PERSON ) {
        PERSON_t *pe = fr[b].objs[i].data;
        if( pe->tilex!=newx || pe->tilez!=newz )
          continue;
        obstructor = pe;
        break;
      }
    }

    // TODO: go to somewhere else to handle obstructor

    switch( (newpe->tilex==newx ? 0 : 1) + (newpe->tilez==newz ? 0 : 1) ) {
      case 2: required_ap = 14; break;
      case 1: required_ap = 10; break;
    }

    required_st = 1;

    // walking, running, or sprinting?
    if( newpe->gait == WALKING ) {
      required_ap += required_ap / 2;
      required_st = 0;
    } else if( newpe->gait == SPRINTING ) {
      required_ap /= 2;
      required_st = 2;
    }

    if( newpe->ap < required_ap ) {
      // not enough Action Points
    } else if( newpe->st < required_st ) {
      // not enough Stamina Points
    } else {
      if( obstructor ) {
        obstructor->hp -= 10;
        obstructor->hitcounter = 20;
      } else {
        newpe->tilex = newx;
        newpe->tilez = newz;
      }

      newpe->ap -= required_ap;
      newpe->st -= required_st;
    }
  }

  float posx = newpe->tilex*co->bsx + co->bsx/2;
  float posz = newpe->tilez*co->bsz + co->bsz/2;

  float velx = (posx - newpe->pos.x)/4;
  float velz = (posz - newpe->pos.z)/4;
  float mag = sqrtf(velx*velx + velz*velz);

  if( mag>0.1 || (newpe->walkcounter/4) % 2 ) { // entangled_walkcounter
    if( newpe->stopcounter > 0 )
      newpe->walkcounter = 6;
    newpe->walkcounter++;
    newpe->stopcounter = 0;
  } else {
    newpe->walkcounter = 0;
    newpe->stopcounter++;
  }

  if( newpe->hp > 0 ) newpe->incapcounter = 0;
  else                newpe->incapcounter++;

  if( --newpe->hitcounter < 0 )
    newpe->hitcounter = 0;

  // just snap if close
  if( fabsf(velx)<0.5 && fabsf(velz)<0.5 )
  {
    newpe->vel.x = 0;
    newpe->vel.z = 0;
    newpe->pos.x = posx;
    newpe->pos.z = posz;
  }
  else if( mag<2 )
  {
    newpe->vel.x = velx/mag*2;
    newpe->vel.z = velz/mag*2;
  }
  else
  {
    newpe->vel.x = velx;
    newpe->vel.z = velz;
  }

  newpe->vel.y += 0.6f;      //gravity
}

//////////////////////////////////////////
// Different persons' drawing routines! //
//////////////////////////////////////////

static void get_azma_sprites(SPRITE_T **sprs, PERSON_t *pe)
{
  SPRITE_T *defspr = &SM(azma_c_idle_s);
  if( pe->armed )
  {
    switch( (pe->walkcounter/4) % 4 ) {
      case 0:
      case 2: switch( pe->dir ) {                      // standing
        case W : sprs[0] = &SM(azma_c_idle_w);   sprs[1] = &SM(azma_w_idle_w);   break;
        case E : sprs[0] = &SM(azma_c_idle_e);   sprs[1] = &SM(azma_w_idle_e);   break;
        case N : sprs[0] = &SM(azma_c_idle_n);   sprs[1] = &SM(azma_w_idle_n);   break;
        case S : sprs[0] = &SM(azma_c_idle_s);   sprs[1] = &SM(azma_w_idle_s);   break;
        case NW: sprs[0] = &SM(azma_c_idle_nw);  sprs[1] = &SM(azma_w_idle_nw);  break;
        case NE: sprs[0] = &SM(azma_c_idle_ne);  sprs[1] = &SM(azma_w_idle_ne);  break;
        case SW: sprs[0] = &SM(azma_c_idle_sw);  sprs[1] = &SM(azma_w_idle_sw);  break;
        case SE: sprs[0] = &SM(azma_c_idle_se);  sprs[1] = &SM(azma_w_idle_se);  break;
        default: sprs[0] = defspr;                                               break;
      } break;

      case 1: switch( pe->dir ) {                      // walking 1
        case W : sprs[0] = &SM(azma_c_walk1_w);  sprs[1] = &SM(azma_w_walk1_w);  break;
        case E : sprs[0] = &SM(azma_c_walk1_e);  sprs[1] = &SM(azma_w_walk1_e);  break;
        case N : sprs[0] = &SM(azma_c_walk1_n);  sprs[1] = &SM(azma_w_walk1_n);  break;
        case S : sprs[0] = &SM(azma_c_walk1_s);  sprs[1] = &SM(azma_w_walk1_s);  break;
        case NW: sprs[0] = &SM(azma_c_walk1_nw); sprs[1] = &SM(azma_w_walk1_nw); break;
        case NE: sprs[0] = &SM(azma_c_walk1_ne); sprs[1] = &SM(azma_w_walk1_ne); break;
        case SW: sprs[0] = &SM(azma_c_walk1_sw); sprs[1] = &SM(azma_w_walk1_sw); break;
        case SE: sprs[0] = &SM(azma_c_walk1_se); sprs[1] = &SM(azma_w_walk1_se); break;
        default: sprs[0] = defspr;                                               break;
      } break;

      case 3: switch( pe->dir ) {                      // walking 2
        case W : sprs[0] = &SM(azma_c_walk2_w);  sprs[1] = &SM(azma_w_walk2_w);  break;
        case E : sprs[0] = &SM(azma_c_walk2_e);  sprs[1] = &SM(azma_w_walk2_e);  break;
        case N : sprs[0] = &SM(azma_c_walk2_n);  sprs[1] = &SM(azma_w_walk2_n);  break;
        case S : sprs[0] = &SM(azma_c_walk2_s);  sprs[1] = &SM(azma_w_walk2_s);  break;
        case NW: sprs[0] = &SM(azma_c_walk2_nw); sprs[1] = &SM(azma_w_walk2_nw); break;
        case NE: sprs[0] = &SM(azma_c_walk2_ne); sprs[1] = &SM(azma_w_walk2_ne); break;
        case SW: sprs[0] = &SM(azma_c_walk2_sw); sprs[1] = &SM(azma_w_walk2_sw); break;
        case SE: sprs[0] = &SM(azma_c_walk2_se); sprs[1] = &SM(azma_w_walk2_se); break;
        default: sprs[0] = defspr;                                               break;
      } break;
    }
  } else { // not armed
    switch( (pe->walkcounter/4) % 4 ) {
      case 0:
      case 2: switch( pe->dir ) {                      // standing
        case W : sprs[0] = &SM(azma_idle_w);                                     break;
        case E : sprs[0] = &SM(azma_idle_e);                                     break;
        case N : sprs[0] = &SM(azma_idle_n);                                     break;
        case S : sprs[0] = &SM(azma_idle_s);                                     break;
        case NW: sprs[0] = &SM(azma_idle_nw);                                    break;
        case NE: sprs[0] = &SM(azma_idle_ne);                                    break;
        case SW: sprs[0] = &SM(azma_idle_sw);                                    break;
        case SE: sprs[0] = &SM(azma_idle_se);                                    break;
        default: sprs[0] = defspr;                                               break;
      } break;

      case 1: switch( pe->dir ) {                      // walking 1
        case W : sprs[0] = &SM(azma_walk1_w);                                    break;
        case E : sprs[0] = &SM(azma_walk1_e);                                    break;
        case N : sprs[0] = &SM(azma_walk1_n);                                    break;
        case S : sprs[0] = &SM(azma_walk1_s);                                    break;
        case NW: sprs[0] = &SM(azma_walk1_nw);                                   break;
        case NE: sprs[0] = &SM(azma_walk1_ne);                                   break;
        case SW: sprs[0] = &SM(azma_walk1_sw);                                   break;
        case SE: sprs[0] = &SM(azma_walk1_se);                                   break;
        default: sprs[0] = defspr;                                               break;
      } break;

      case 3: switch( pe->dir ) {                      // walking 2
        case W : sprs[0] = &SM(azma_walk2_w);                                    break;
        case E : sprs[0] = &SM(azma_walk2_e);                                    break;
        case N : sprs[0] = &SM(azma_walk2_n);                                    break;
        case S : sprs[0] = &SM(azma_walk2_s);                                    break;
        case NW: sprs[0] = &SM(azma_walk2_nw);                                   break;
        case NE: sprs[0] = &SM(azma_walk2_ne);                                   break;
        case SW: sprs[0] = &SM(azma_walk2_sw);                                   break;
        case SE: sprs[0] = &SM(azma_walk2_se);                                   break;
        default: sprs[0] = defspr;                                               break;
      } break;
    }
  }
}

static void get_gyllioc_sprites(SPRITE_T **sprs, PERSON_t *pe)
{
  SPRITE_T *defspr = &SM(gyllioc_idle_s);
  
  if( pe->hp <= 0 ) {
    if( pe->incapcounter < 10 ) sprs[0] = &SM(gyllioc_fall);
    else                        sprs[0] = &SM(gyllioc_incap);
  } else if( pe->stopcounter > 10 ) {
    switch( pe->dir ) {
      case W : sprs[0] = &SM(gyllioc_idle_w);  break;
      case E : sprs[0] = &SM(gyllioc_idle_e);  break;
      case N : sprs[0] = &SM(gyllioc_idle_n);  break;
      case S : sprs[0] = &SM(gyllioc_idle_s);  break;
      case NW: sprs[0] = &SM(gyllioc_idle_nw); break;
      case NE: sprs[0] = &SM(gyllioc_idle_ne); break;
      case SW: sprs[0] = &SM(gyllioc_idle_sw); break;
      case SE: sprs[0] = &SM(gyllioc_idle_se); break;
      default: sprs[0] = defspr;               break;
    }
  } else {
    switch( (pe->walkcounter/4) % 4 ) {
      case 0:
      case 2: switch( pe->dir ) {                      // stopping
        case W : sprs[0] = &SM(gyllioc_stop_w);  break;
        case E : sprs[0] = &SM(gyllioc_stop_e);  break;
        case N : sprs[0] = &SM(gyllioc_stop_n);  break;
        case S : sprs[0] = &SM(gyllioc_stop_s);  break;
        case NW: sprs[0] = &SM(gyllioc_stop_nw); break;
        case NE: sprs[0] = &SM(gyllioc_stop_ne); break;
        case SW: sprs[0] = &SM(gyllioc_stop_sw); break;
        case SE: sprs[0] = &SM(gyllioc_stop_se); break;
        default: sprs[0] = defspr;               break;
      } break;

      case 1: switch( pe->dir ) {                      // walking 1
        case W : sprs[0] = &SM(gyllioc_walk1_w);                                    break;
        case E : sprs[0] = &SM(gyllioc_walk1_e);                                    break;
        case N : sprs[0] = &SM(gyllioc_walk1_n);                                    break;
        case S : sprs[0] = &SM(gyllioc_walk1_s);                                    break;
        case NW: sprs[0] = &SM(gyllioc_walk1_nw);                                   break;
        case NE: sprs[0] = &SM(gyllioc_walk1_ne);                                   break;
        case SW: sprs[0] = &SM(gyllioc_walk1_sw);                                   break;
        case SE: sprs[0] = &SM(gyllioc_walk1_se);                                   break;
        default: sprs[0] = defspr;                                                  break;
      } break;

      case 3: switch( pe->dir ) {                      // walking 2
        case W : sprs[0] = &SM(gyllioc_walk2_w);                                    break;
        case E : sprs[0] = &SM(gyllioc_walk2_e);                                    break;
        case N : sprs[0] = &SM(gyllioc_walk2_n);                                    break;
        case S : sprs[0] = &SM(gyllioc_walk2_s);                                    break;
        case NW: sprs[0] = &SM(gyllioc_walk2_nw);                                   break;
        case NE: sprs[0] = &SM(gyllioc_walk2_ne);                                   break;
        case SW: sprs[0] = &SM(gyllioc_walk2_sw);                                   break;
        case SE: sprs[0] = &SM(gyllioc_walk2_se);                                   break;
        default: sprs[0] = defspr;                                                  break;
      } break;
    }
  }
}

static void get_slug_sprites(SPRITE_T **sprs, PERSON_t *pe)
{
  sprs[0] = &SM(slug_r);
}
