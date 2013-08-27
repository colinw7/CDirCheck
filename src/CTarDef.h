#ifndef CTAR_DEF_H
#define CTAR_DEF_H

#include <sys/types.h>

#define CTAR_HEADER_SIZE 512

#define CTAR_FCHR 0020000
#define CTAR_FBLK 0060000
#define CTAR_FIFO 0010000

#define CTAR_NAMSPACE 100
#define CTAR_NAMESIZ  (NAMSPACE - 1)

#define CTAR_PREFIXSPACE  156
#define CTAR_PREFIXSIZ    (PREFIXSPACE - 1)

union CTarHeader {
  char dummy[CTAR_HEADER_SIZE];

  struct header {
    char name[CTAR_NAMSPACE];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char linkflag;
    char linkname[CTAR_NAMSPACE];
    char rdev[12];
  } dbuf;

  struct pheader {
    char name[CTAR_NAMSPACE];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag;
    char linkname[CTAR_NAMSPACE];
    char magic[6];
    char version[2];
    char uname[32];
    char gname[32];
    char devmajor[8];
    char devminor[8];
    char prefix[CTAR_PREFIXSPACE];
  } pdbuf;
};

/* #define linkflag typeflag */

struct CTarLikBuf {
  struct CTarLikBuf *nextp;
  ino_t              inum;
  dev_t              devnum;
  short              count;
  char               pathname[1];  /* actually alloced larger */
};

#define CTAR_MAGIC   "ustar"  /* ustar and null */
#define CTAR_MAGLEN  6
#define CTAR_VERSION "00"     /* 00 and null */
#define CTAR_VERSLEN 2

/* Values in typeflag field */

#define CTAR_REGTYPE  '0'   /* Regular File */
#define CTAR_AREGTYPE '\0'  /* Regular File */
#define CTAR_LNKTYPE  '1'   /* Link */
#define CTAR_SYMTYPE  '2'   /* Reserved */
#define CTAR_CHRTYPE  '3'   /* Character Special */
#define CTAR_BLKTYPE  '4'   /* Block Special */
#define CTAR_DIRTYPE  '5'   /* Directory */
#define CTAR_FIFOTYPE '6'   /* FIFO special */
#define CTAR_CONTTYPE '7'   /* Reserved */

/* Bits used in the mode field - values in octal */

#define CTAR_SUID   04000  /* Set UID on execution */
#define CTAR_SGID   02000  /* Set GID on execution */
#define CTAR_SVTX   01000  /* Reserved */
#define CTAR_UREAD  00400  /* Read by Owner */
#define CTAR_UWRITE 00200  /* Write by Owner */
#define CTAR_UEXEC  00100  /* Execute/Search by Owner */
#define CTAR_GREAD  00040  /* Read by Group */
#define CTAR_GWRITE 00020  /* Write by Group */
#define CTAR_GEXEC  00010  /* Execute/Search by Group */
#define CTAR_OREAD  00004  /* Read by Other */
#define CTAR_OWRITE 00002  /* Write by Other */
#define CTAR_OEXEC  00001  /* Execute/Search by Other */

#endif
