
#ifndef SPARTOR_SJGLOB_H_
#define SPARTOR_SJGLOB_H_


#ifdef _WIN32

#include <windows.h>

#define SJGLOB_MARK     1
#define SJGLOB_NOESCAPE 2

typedef struct {
        size_t   gl_pathc;
        char   **gl_pathv;
        size_t   gl_offs;
} SJGLOB_T;

#else //ifdef _WIN32

#include <glob.h>

#define SJGLOB_MARK     GLOB_MARK
#define SJGLOB_NOESCAPE GLOB_NOESCAPE

typedef glob_t SJGLOB_T;

#endif //ifdef _WIN32


SJGLOB_T *SJglob( const char *path, const char *pattern, int flags );
void SJglobfree( SJGLOB_T *files );


#endif


