#ifndef CFILE_UTIL_H
#define CFILE_UTIL_H

typedef unsigned int   CIMAGE_INT32;
typedef unsigned short CIMAGE_INT16;
typedef unsigned char  CIMAGE_INT8;

#include <CFileType.h>

#include <string>

class CFileBase;
union CTarHeader;

namespace CFileUtil {
  CFileType getType(const std::string &filename);
  CFileType getType(CFileBase *file);

  CFileType getImageType(const std::string &filename);
  CFileType getImageType(CFileBase *file);
  CFileType getImageTypeFromName(const std::string &filename);

  bool isImageType(CFileType type);

  bool checkBMP(CFileBase *file);
  bool checkGIF(CFileBase *file);
  bool checkIFF(CFileBase *file);
  bool checkJPG(CFileBase *file);
  bool checkPCX(CFileBase *file);
  bool checkPNG(CFileBase *file);
  bool checkPPM(CFileBase *file);
  bool checkPSP(CFileBase *file);
  bool checkPS (CFileBase *file);
  bool checkSGI(CFileBase *file);
  bool checkTGA(CFileBase *file);
  bool checkTIF(CFileBase *file);
  bool checkXBM(CFileBase *file);
  bool checkXPM(CFileBase *file);
  bool checkXWD(CFileBase *file);

  CFileType getSoundType(CFileBase *file);
  CFileType getSoundTypeFromName(const std::string &filename);

  bool checkWAV(CFileBase *file);

  CFileType getTextType(const std::string &filename);
  CFileType getTextType(CFileBase *file);
  CFileType getTextTypeFromName(const std::string &filename);

  CFileType isELF(CFileBase *file);
  CFileType isGZip(CFileBase *file);
  CFileType isScript(CFileBase *file);
  CFileType isHtml(CFileBase *file);
  CFileType isTar(CFileBase *file);
  CFileType isText(CFileBase *file);

  std::string getTypeMime(CFileType type);
  std::string getTypeStr(CFileType type);
  CFileType getStrType(const std::string &str);
  std::string getPrefix(CFileType type);

  int tarCalculateChecksum(CTarHeader *hblock);
  int tarOctalFieldToInt(char *field, int field_size);
  int tarOctalStringToInt(const std::string &str);

  CIMAGE_INT32 swapBytes32(CIMAGE_INT32 i);
  CIMAGE_INT16 swapBytes16(CIMAGE_INT16 i);

  bool isBadFilename(const std::string &name);
}

#endif
