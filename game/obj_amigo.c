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

//FIXME REMOVE! force amigo to flykick
int flykick = 0;
//

void obj_amigo_draw( int objid, Uint32 vidfr, OBJ_t *o, CONTEXT_t *co )
{
  typedef struct { int x, y, w, h, dx, dy; } XSPR;
  XSPR tip = {0,0,0,0,0,0}; // extra sprite for tip of sword
  AMIGO_t *am = o->data;

  int w = 50;
  int h = 50;
  int x = 0,y = 0;
  int z = am->pos.y + am->hull[1].y;
  switch( am->state ) {
    case AMIGO_HELLO:
      x = (am->statetime/30) * 50; //                       x   y  w  h  dx  dy
      if(      am->statetime<30 ) { x =   0; tip = (XSPR){  0,  0, 0, 0,  0,  0}; }
      else if( am->statetime<60 ) { x =  50; tip = (XSPR){  0,  0, 0, 0,  0,  0}; }
      else if( am->statetime<90 ) { x = 100; tip = (XSPR){  0,  0, 0, 0,  0,  0}; }
      else                        { x = 150; tip = (XSPR){220,230,10,10,  8,-10}; }
      y = 150;
      break;
    case AMIGO_COOLDOWN:
      tip = (XSPR){210,230,10,10, 20,-10};
      break;
    case AMIGO_JUMP:
      tip = (XSPR){240,230,10,10, 33,-10};
      x = 0;
      y = 50;
      break;
    case AMIGO_SLASH: //                                    x   y  w  h  dx  dy
      if(      am->statetime<20 ) { x =  50; tip = (XSPR){220,240,30,10,-30, 42}; }
      else if( am->statetime<25 ) { x = 100; tip = (XSPR){220,230,10,10,  6,-10}; }
      else                        { x = 150; tip = (XSPR){230,230,10,10, 32,-10}; }
      break;
    case AMIGO_FLYKICK:
      if( am->statetime > 30 && am->sword_dist.x < 80.0f && am->sword_dist.y < 60.0f )
        // in this state tip is amigo's left arm
        //             x   y  w  h  dx  dy
        tip = (XSPR){170,220,20,20, 35, 20};
      else
        tip = (XSPR){190,220,20,20, 25, 24};
      x = ((am->hatcounter%100)/50 ? 100 : 50);
      y = 50;
      z += 32;
      break;
    case AMIGO_DASH:
      tip = (XSPR){210,250,40, 6,-40, 40};
      x = 150;
      y = 50;
      break;
  }
  SJGL_SetTex( sys_tex[TEX_AMIGO].num );
  int c = POINT2NATIVE_X(&am->pos);
  int d = POINT2NATIVE_Y(&am->pos);
  SJGL_Blit(&(REC){     x,     y,     w,     h }, c-34,        d-32,        z);
  SJGL_Blit(&(REC){ tip.x, tip.y, tip.w, tip.h }, c-34+tip.dx, d-32+tip.dy, z);
}

void obj_amigo_adv( int objid, Uint32 a, Uint32 b, OBJ_t *oa, OBJ_t *ob )
{
  int        slot0;
  AMIGO_t   *am            = ob->data;
  float      amigo_gravity = 0.6f;

//FIXME REMOVE! Wrap amigo since he can mostly only go left
CONTEXT_t *co = fr[b].objs[ob->context].data;
float cowidth = co->x*co->bsx;
if( am->pos.x <        -20.0f ) am->pos.x += cowidth+39.0f;
if( am->pos.x > cowidth+20.0f ) am->pos.x -= cowidth+39.0f;
//

  spatt(hotfr); // FIXME: probably a BUG
  switch( am->state ) {
    case AMIGO_HELLO:
      if( am->statetime>120 ) {
        am->state = AMIGO_COOLDOWN;
        am->statetime = 0;
      }
      break;
    case AMIGO_COOLDOWN:
      if( am->vel.y!=0 )
        break;
      am->vel.x = 0.0f;
      if( am->statetime>30 ) { // decide which attack to do!
        am->statetime = 0;
        switch( patt()%8 ) {
          case 0: am->state = AMIGO_JUMP;    am->vel.y -= 10.0f;                                        break;
          case 1: am->state = AMIGO_JUMP;    am->vel.y -= 10.0f; am->vel.x =  4.0f;                     break;
          case 2: am->state = AMIGO_JUMP;    am->vel.y -= 10.0f; am->vel.x = -4.0f;                     break;
          case 3: am->state = AMIGO_JUMP;    am->vel.y -=  8.0f; am->vel.x =  4.0f;                     break;
          case 4: am->state = AMIGO_JUMP;    am->vel.y -=  8.0f; am->vel.x = -4.0f;                     break;
          case 5: am->state = AMIGO_SLASH;                       am->vel.x = -0.1f;                     break;
          case 6: am->state = AMIGO_DASH;                        am->vel.x = -7.6f;                     break;
          case 7: am->state = AMIGO_FLYKICK; am->vel.y  = -3.0f; am->vel.x = -7.5f; am->hatcounter = 0; break;
        }
      }
      break;
    case AMIGO_JUMP:
      if( am->statetime>20 ) {
        am->state = AMIGO_COOLDOWN;
        am->statetime = 0;
      }
      break;
    case AMIGO_SLASH:
      am->vel.x += 0.05f;
      if( am->vel.x > 0.0f )
        am->vel.x = 0.0f;
      if( am->statetime>30 ) {
        am->state = AMIGO_COOLDOWN;
        am->statetime = 0;
      }
      break;
    case AMIGO_FLYKICK: {
      amigo_gravity = 0.0f;
      am->vel.y = 0.0f;
      am->hatcounter += fabsf(am->vel.x)*10;
      am->vel.x += am->vel.x < -2.0f ? 0.1f : 0.05;
      if( am->vel.x > 0.0f )
        am->vel.x = 0.0f;
      if( am->statetime==1 ) {
        MKOBJ( sw, AMIGOSWORD, ob->context, OBJF_POS|OBJF_VEL|OBJF_VIS );
        sw->pos = am->pos;
        sw->vel = (V){1.5f,-2.5f,0.0f};
        sw->hull[0] = (V){0,0,0};
        sw->hull[1] = (V){0,0,0};
        sw->model = 0;
        sw->owner = objid;
        sw->spincounter = 0;
        am->sword = slot0;
      }
      GETOBJ( sw, AMIGOSWORD, am->sword );
      am->sword_dist = (V){ fabsf(sw->pos.x - am->pos.x), fabsf(sw->pos.y - am->pos.y), 0 };
      if( am->statetime>90 ) {
        if( am->sword_dist.x < 41.0f && am->sword_dist.y < 11.0f ) {
          am->state = AMIGO_COOLDOWN;
          am->statetime = 0;
          fr[b].objs[am->sword].flags |= OBJF_DEL;
        }
      }
      break;
    }
    case AMIGO_DASH:
      am->vel.x += 0.01f;
      if( am->vel.y==0.0f && fabsf(am->vel.x)>2.0f && !(patt()%60) )
        am->pos.y -= (patt()%2+2)*2.3f; // turbulence on the ground
      if( am->statetime>50 ) {
        am->state = AMIGO_COOLDOWN;
        am->statetime = 0;
      }
      break;
  }

  am->statetime++;
  am->vel.y += amigo_gravity; //gravity
}

