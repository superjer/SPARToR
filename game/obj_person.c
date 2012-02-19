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

SPRITE_T *get_azma_sprite(PERSON_t *pe);
SPRITE_T *get_slug_sprite(PERSON_t *pe);

void obj_person_draw( int objid, Uint32 vidfr, OBJ_t *o, CONTEXT_t *co )
{
  PERSON_t *pe = o->data;
  int c = POINT2NATIVE_X(&pe->pos);
  int d = POINT2NATIVE_Y(&pe->pos);
  SPRITE_T *spr;

  switch( pe->character ) {
    case CHR_AZMA: spr = get_azma_sprite(pe); break;
    case CHR_SLUG: spr = get_slug_sprite(pe); break;
  }

  sprblit( spr, c, d, d+12 );
  sprblit( &SM(shadow), c, d, d+11 );
}

void obj_person_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob )
{
  PERSON_t  *oldpe = oa->data;
  PERSON_t  *newpe = ob->data;
  CONTEXT_t *co = fr[b].objs[ob->context].data;
  MOTHER_t  *mo = fr[b].objs[0].data;

  enum DIR8 dir = NODIR;  // direction we want to move
  int stop = 0;           // whether to end the current turn

  if( mo->active==objid ) {
    if( mo->turnstart == hotfr ) {
      newpe->ap += 100;
      if( newpe->ap > newpe->max_ap )
        newpe->ap = newpe->max_ap;
    }

    // check for input
    if( newpe->ghost ) {
      GHOST_t *gh = fr[b].objs[newpe->ghost].data;

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
    }

    if( stop ) mo->active = 0;
  }

  if( mo->intervalstart == hotfr )
    newpe->pos.y -= 50;

  if( !oldpe ) { //FIXME why's this null?
    SJC_Write("Warning: oldpe is NULL!");
    return;
  }

  // determine desired movement
  int newx = newpe->tilex;
  int newz = newpe->tilez;
  switch( dir ) {
    case NODIR: break;
    case W : newx = newpe->tilex-1; newz = newpe->tilez+1; break;
    case E : newx = newpe->tilex+1; newz = newpe->tilez-1; break;
    case N : newx = newpe->tilex-1; newz = newpe->tilez-1; break;
    case S : newx = newpe->tilex+1; newz = newpe->tilez+1; break;
    case NW: newx = newpe->tilex-1;                        break;
    case NE:                        newz = newpe->tilez-1; break;
    case SW:                        newz = newpe->tilez+1; break;
    case SE: newx = newpe->tilex+1;                        break;
  }

  if( dir ) newpe->dir = dir;

  // move only if in-bounds
  if( dir && newx>=0 && newz>=0 && newx<co->x && newz<co->z ) {
    int required_ap;
    
    switch( (newpe->tilex==newx ? 0 : 1) + (newpe->tilez==newz ? 0 : 1) ) {
      case 2: required_ap = 14; break;
      case 1: required_ap = 10; break;
      default: SJC_Write("How many directions do you really need to move at one time, jeeez!");
    }

    if( newpe->ap >= required_ap ) {
      newpe->tilex = newx;
      newpe->tilez = newz;
      newpe->ap -= required_ap;
    } else {
      SJC_Write("I can't let you do that, StarDave");
    }
  }

  float posx = newpe->tilex*co->bsx + co->bsx/2;
  float posz = newpe->tilez*co->bsz + co->bsz/2;

  float velx = (posx - newpe->pos.x)/4;
  float velz = (posz - newpe->pos.z)/4;
  float mag = sqrtf(velx*velx + velz*velz);

  if( mag>0.1 || (newpe->walkcounter/4) % 2 ) // entangled_walkcounter
    newpe->walkcounter++;
  else
    newpe->walkcounter = 0;

  // just snap if close
  if( fabsf(velx)<0.5 && fabsf(velz)<0.5 ) {
    newpe->vel.x = 0;
    newpe->vel.z = 0;
    newpe->pos.x = posx;
    newpe->pos.z = posz;
  } else if( mag<2 ) {
    newpe->vel.x = velx/mag*2;
    newpe->vel.z = velz/mag*2;
  } else {
    newpe->vel.x = velx;
    newpe->vel.z = velz;
  }

  newpe->vel.y += 0.6f;      //gravity
}

//////////////////////////////////////////
// Different persons' drawing routines! //
//////////////////////////////////////////

SPRITE_T* get_azma_sprite(PERSON_t *pe)
{
  SPRITE_T *defspr = &SM(azma_stand_s);
  switch( (pe->walkcounter/4) % 4 ) { // entangled_walkcounter
    case 0:
    case 2: switch( pe->dir ) {                      // standing
      case W : return &SM(azma_stand_w);
      case E : return &SM(azma_stand_e);
      case N : return &SM(azma_stand_n);
      case S : return &SM(azma_stand_s);
      case NW: return &SM(azma_stand_nw);
      case NE: return &SM(azma_stand_ne);
      case SW: return &SM(azma_stand_sw);
      case SE: return &SM(azma_stand_se);
      default: return defspr;
    } break;

    case 1: switch( pe->dir ) {                      // walking 1
      case W : return &SM(azma_walk1_w);
      case E : return &SM(azma_walk1_e);
      case N : return &SM(azma_walk1_n);
      case S : return &SM(azma_walk1_s);
      case NW: return &SM(azma_walk1_nw);
      case NE: return &SM(azma_walk1_ne);
      case SW: return &SM(azma_walk1_sw);
      case SE: return &SM(azma_walk1_se);
      default: return defspr;
    } break;

    case 3: switch( pe->dir ) {                      // walking 2
      case W : return &SM(azma_walk2_w);
      case E : return &SM(azma_walk2_e);
      case N : return &SM(azma_walk2_n);
      case S : return &SM(azma_walk2_s);
      case NW: return &SM(azma_walk2_nw);
      case NE: return &SM(azma_walk2_ne);
      case SW: return &SM(azma_walk2_sw);
      case SE: return &SM(azma_walk2_se);
      default: return defspr;
    } break;
  }
  return defspr;
}

SPRITE_T* get_slug_sprite(PERSON_t *pe)
{
  return &SM(slug_r);
}
