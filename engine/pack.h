#ifndef SPARTOR_PACK_H_
#define SPARTOR_PACK_H_

// shortcuts for continuing byte packing/unpacking
#define pack(value, width) packbytes(NULL, (value), NULL, (width))
#define unpack(width) unpackbytes(NULL, 0, NULL, (width))

unsigned char *packframe      (unsigned int packfr, size_t *n);
unsigned char *packframecmds  (unsigned int packfr, size_t *n);
int            unpackframe    (unsigned int packfr, unsigned char *data, size_t  len);
int            unpackframecmds(unsigned int packfr, unsigned char *data, size_t  len);
void           packbytes      (unsigned char *data, unsigned long value, size_t *offset, int width);
unsigned long  unpackbytes    (unsigned char *data, size_t len, size_t *offset, int width);
void           inspectbytes   (unsigned char *data, int n);

#endif
