#ifndef SPARTOR_PACK_H_
#define SPARTOR_PACK_H_

// shortcuts for continuing byte packing/unpacking
#define pack(value, width) packbytes(NULL, (value), NULL, (width))
#define unpack(width) unpackbytes(NULL, 0, NULL, (width))

Uint8 *packframe(      Uint32 packfr, size_t *n);
Uint8 *packframecmds(  Uint32 packfr, size_t *n);
int    unpackframe(    Uint32 packfr, Uint8  *data,  size_t  len);
int    unpackframecmds(Uint32 packfr, Uint8  *data,  size_t  len);
void   packbytes(      Uint8 *data,   Uint64  value, size_t *offset, int width);
Uint64 unpackbytes(    Uint8 *data,   size_t  len,   size_t *offset, int width);
void   inspectbytes(   Uint8 *data,   int     n);

#endif
