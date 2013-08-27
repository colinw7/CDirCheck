#ifndef CFILE_CHECKSUM_H
#define CFILE_CHECKSUM_H

class CFileCheckSum {
 public:
  CFileCheckSum() { }

  void calc(const unsigned char *data, unsigned int len);

  static int diff(const CFileCheckSum &lhs, const CFileCheckSum &rhs);

 private:
  friend bool operator<(const CFileCheckSum &lhs, const CFileCheckSum &rhs) {
    return diff(lhs, rhs) < 0;
  }

 private:
  unsigned char checksum_[16];
};

#endif
