/**
 **  Dead Kings' Quest
 **  A special game for the SPARToR Network Game Engine
 **  Copyright (c) 2010-2011  Jer Wilson
 **
 **  See COPYING for details.
 **
 **  http://www.superjer.com/
 **  http://www.spartor.com/
 **  http://github.com/superjer/SPARToR
 **/

#include "obj_.h"

void obj_person_draw( int objid, Uint32 vidfr, OBJ_t *o, CONTEXT_t *co )
{
  PERSON_t *pe = o->data;
  int c = POINT2NATIVE_X(pe->pos);
  int d = POINT2NATIVE_Y(pe->pos);
  SJGL_SetTex( sys_tex[TEX_PERSON].num );
  REC rect;
  int flip = 0;

  #define P_REC(x,y,w,h) (rect = (REC){(x)*17,(y)*45,(w)*17,(h)*45})
  switch( (pe->walkcounter/4) % 4 ) { // entangled_walkcounter
    case 0:
    case 2: switch( pe->dir ) {                      // standing
      case NODIR: break;
      case W : P_REC( 4, 0, 1, 1);           break;
      case E : P_REC( 4, 0, 1, 1); flip = 1; break;
      case N : P_REC( 3, 0, 1, 1);           break;
      case S : P_REC( 0, 0, 1, 1);           break;
      case NW: P_REC( 2, 0, 1, 1);           break;
      case NE: P_REC( 2, 0, 1, 1); flip = 1; break;
      case SW: P_REC( 1, 0, 1, 1); flip = 1; break;
      case SE: P_REC( 1, 0, 1, 1);           break;
    } break;

    case 1: switch( pe->dir ) {                      // walking 1
      case NODIR: break;
      case W : P_REC(11, 1, 2, 1);           break;
      case E : P_REC(11, 1, 2, 1); flip = 1; break;
      case N : P_REC( 9, 1, 1, 1);           break;
      case S : P_REC( 0, 1, 1, 1);           break;
      case NW: P_REC( 6, 1, 1, 1);           break;
      case NE: P_REC( 6, 1, 1, 1); flip = 1; break;
      case SW: P_REC( 2, 1, 2, 1); flip = 1; break;
      case SE: P_REC( 2, 1, 2, 1);           break;
    } break;

    case 3: switch( pe->dir ) {                      // walking 2
      case NODIR: break;
      case W : P_REC(13, 1, 2, 1);           break;
      case E : P_REC(13, 1, 2, 1); flip = 1; break;
      case N : P_REC(10, 1, 1, 1);           break;
      case S : P_REC( 1, 1, 1, 1);           break;
      case NW: P_REC( 7, 1, 2, 1);           break;
      case NE: P_REC( 7, 1, 2, 1); flip = 1; break;
      case SW: P_REC( 4, 1, 2, 1); flip = 1; break;
      case SE: P_REC( 4, 1, 2, 1);           break;
    } break;
  }
  #undef P_REC

  int x = c - rect.w/2;
  int y = d - 43;
  int r = d + 12;

  fprintf(stderr,"player: %d\n",r);

  if( flip ) {
    rect.x += rect.w;
    rect.w *= -1;
  }

  SJGL_Blit( &rect, x, y, r );

  // draw shadow
  SJGL_Blit( &(REC){0 ,502,20,10}, c-10, d-5 , r-1 );
}

void obj_person_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob )
{
  PERSON_t  *oldpe = oa->data;
  PERSON_t  *newpe = ob->data;
  CONTEXT_t *co = fr[b].objs[ob->context].data;
  enum DIR8 dir = NODIR;

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
      case CMDT_1SEL:           break;
      case CMDT_1BACK:          break;
    }
  }

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
  if( newx>=0 && newz>=0 && newx<co->x && newz<co->z ) {
    newpe->tilex = newx;
    newpe->tilez = newz;
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

