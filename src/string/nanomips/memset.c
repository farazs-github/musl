#include <string.h>

typedef unsigned long reg_t;
typedef struct bits
{
  reg_t B0:8, B1:8, B2:8, B3:8;
} bits_t;

/* This union assumes that small structures can be in registers.  If
   not, then memory accesses will be done - not optimal, but ok.  */
typedef union
{
  reg_t v;
  bits_t b;
} bitfields_t;

/* This code is called when aligning a pointer or there are remaining bytes
   after doing word sets.  */
static inline void * __attribute__ ((always_inline))
do_bytes (void *a, void *retval, unsigned char fill, unsigned long len)
{
  unsigned char *x = (unsigned char *) a;
  unsigned long i;

  for (i = 0; i < len; i++)
    {
      *x++ = fill;
    }
  return retval;
}

/* Pointer is aligned.  */
static void *
do_aligned_words (reg_t * a, void * retval, reg_t fill,
	 unsigned long words, unsigned long bytes)
{
  unsigned long i, words_by_8, words_by_1;

  words_by_1 = words % 8;
  words_by_8 = words - words_by_1;
  /*
   * Note: prefetching the store memory is not beneficial on most
   * cores since the ls/st unit has store buffers that will be filled
   * before the cache line is actually needed.
   *
   * Also, using prepare-for-store cache op is problematic since we
   * don't know the implementation-defined cache line length and we
   * don't want to touch unintended memory.
   */
  for (i = 0; i < words_by_8; i += 8)
    {
      a[0] = fill;
      a[1] = fill;
      a[2] = fill;
      a[3] = fill;
      a[4] = fill;
      a[5] = fill;
      a[6] = fill;
      a[7] = fill;
      a += 8;
    }

  /* do remaining words.  */
  for (i = 0; i < words_by_1; i++)
    {
      *a++ = fill;
    }

  /* mop up any remaining bytes.  */
  return do_bytes (a, retval, fill, bytes);
}

void *
memset (void *a, int ifill, size_t len)
{
  unsigned long bytes, words;
  bitfields_t fill;
  void *retval = (void *) a;

  /* shouldn't hit that often.  */
  if (len < sizeof (reg_t) * 4)
    {
      return do_bytes (a, retval, ifill, len);
    }

  /* Align the pointer to word/dword alignment.
     Note that the pointer is only 32-bits for o32/n32 ABIs. For
     n32, loads are done as 64-bit while address remains 32-bit.   */
  bytes = ((unsigned long) a) % sizeof (reg_t);
  if (bytes)
    {
      bytes = sizeof (reg_t) - bytes;
      if (bytes > len)
	bytes = len;
      do_bytes (a, retval, ifill, bytes);
      if (len == bytes)
	return retval;
      len -= bytes;
      a = (void *) (((unsigned char *) a) + bytes);
    }

  /* Create correct fill value for reg_t sized variable.  */
  fill.v = 0;
  if (ifill != 0)
    {
      fill.b.B0 = (unsigned char) ifill;
      fill.b.B1 = (unsigned char) ifill;
      fill.b.B2 = (unsigned char) ifill;
      fill.b.B3 = (unsigned char) ifill;
    }
  words = len / sizeof (reg_t);
  bytes = len % sizeof (reg_t);
  return do_aligned_words (a, retval, fill.v, words, bytes);
}
