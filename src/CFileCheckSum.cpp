#include <CFileCheckSum.h>

extern "C" {
#include "md5.h"
}

void
CFileCheckSum::
calc(const unsigned char *data, unsigned int len)
{
  struct MD5Context context;

  MD5Init  (&context);
  MD5Update(&context, data, len);
  MD5Final (checksum_, &context);
}

int
CFileCheckSum::
diff(const CFileCheckSum &lhs, const CFileCheckSum &rhs)
{
  for (int i = 0; i < 16; ++i) {
    int d = (lhs.checksum_[i] - rhs.checksum_[i]);

    if (d != 0) return d;
  }

  return 0;
}
