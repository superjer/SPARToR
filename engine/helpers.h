
#ifndef SPARTOR_HELPERS__
#define SPARTOR_HELPERS__

// macro wrapper for thread-safe strtok-like function tok_
#define tok(buf,sep) tok_(&(buf),(sep))

int pointlis_add( void ***list, void *item );
char *tok_( char **restrict buf, const char *restrict sep );

#endif
