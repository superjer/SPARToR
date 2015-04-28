#include "video_helpers.h"

void draw_guides(CONTEXT_t *co, int x, int y, int z)
{
  int i;

  glBindTexture( GL_TEXTURE_2D, 0 );

  for( i=0; i<2; i++ )
  {
    if( i )
      glDepthFunc(GL_GREATER);
    int x1 =     x*co->bsx + co->bsx/2;
    int z1 =     z*co->bsz + co->bsz/2;
    int x2 = co->x*co->bsx;
    int z2 = co->z*co->bsz;
    int y1 =     y*co->bsy;
    glBegin(GL_LINE_LOOP);
    glColor4f(0, 0, !i, 1);
    glVertex3i(  0, y1,  0 );
    glVertex3i( x2, y1,  0 );
    glVertex3i( x2, y1, z2 );
    glVertex3i(  0, y1, z2 );
    glEnd();

    glBegin(GL_LINE_LOOP);
    glVertex3i( (x  )*co->bsx, y1, (z  )*co->bsz );
    glVertex3i( (x+1)*co->bsx, y1, (z  )*co->bsz );
    glVertex3i( (x+1)*co->bsx, y1, (z+1)*co->bsz );
    glVertex3i( (x  )*co->bsx, y1, (z+1)*co->bsz );
    glEnd();
    if( x>=0 && z>=0 && x<co->x && z<co->z )
    {
      glBegin(GL_LINES);
      glVertex3i(  0, y1, z1 );
      glVertex3i( x2, y1, z1 );
      glVertex3i( x1, y1,  0 );
      glVertex3i( x1, y1, z2 );
      glColor4f( 0, !i, 0, !i );
      glVertex3i( x1, y1+1000, z1 );
      glVertex3i( x1, y1-1000, z1 );
      glEnd();
    }
  }
}

