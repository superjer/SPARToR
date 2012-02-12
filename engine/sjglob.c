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


#include "console.h"
#include "sjglob.h"


SJGLOB_T *SJglob( const char *path, const char *pattern, int flags )
{
  char *str = malloc( strlen(path) + strlen(pattern) + 2 ); // room for a / in the middle and a ^@ at the end
  strcpy( str, path );
  strcat( str, "/" );
  strcat( str, pattern );

#ifdef _WIN32

  SJGLOB_T *files = malloc( sizeof *files );
  files->gl_pathc = 0;
  files->gl_pathv = malloc( 1000 * sizeof *files->gl_pathv );
  files->gl_offs  = 0;

  WIN32_FIND_DATA findata;
  HANDLE handle;

  handle = FindFirstFile( str, &findata );
  if( handle == INVALID_HANDLE_VALUE )
    SJC_Write( "FindFirstFile failed (%d)", GetLastError() );
  else {
    size_t i;

    for( i=0; i<1000; i++ ) {
      files->gl_pathv[i] = malloc( strlen(path) + strlen(findata.cFileName) + 3 ); // room for a / in the middle, at the end, and a ^@
      strcpy( files->gl_pathv[i], path );
      strcat( files->gl_pathv[i], "/" );
      strcat( files->gl_pathv[i], findata.cFileName );
      if( (flags&SJGLOB_MARK) && (findata.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) )
        strcat( files->gl_pathv[i], "/" );
      files->gl_pathc++;

      if( !FindNextFile(handle,&findata) ) {
        DWORD err = GetLastError();
        if( err != ERROR_NO_MORE_FILES )
          SJC_Write( "FindNextFile failed (%d)", err );
        break;
      }
    }
    
    FindClose( handle );
  }

#else

  SJGLOB_T *files = malloc( sizeof *files );
  glob( str, flags, NULL, files );

#endif

  free(str);
  return files;
}


void SJglobfree( SJGLOB_T *files )
{

#ifdef _WIN32

  while( files->gl_pathc-- )
    free( files->gl_pathv[ files->gl_pathc ] );
  free( files );

#else

  globfree( files );

#endif

}


