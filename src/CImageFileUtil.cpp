#include <CFileUtil.h>
#include <XWDFile.h>
#include <CStrUtil.h>
#include <CFile.h>
#include <CRegExp.h>

#include <cstring>

union IntData {
  CIMAGE_INT32 l;
  CIMAGE_INT16 s;
  CIMAGE_INT8  c[sizeof(CIMAGE_INT32)];
};

CFileType
CFileUtil::
getImageType(const std::string &filename)
{
  CFile file(filename);

  return getImageType(&file);
}

CFileType
CFileUtil::
getImageType(CFileBase *file)
{
  // Order Important

  if      (checkIFF(file))
    return CFILE_TYPE_IMAGE_IFF;
  else if (checkGIF(file))
    return CFILE_TYPE_IMAGE_GIF;
  else if (checkPNG(file))
    return CFILE_TYPE_IMAGE_PNG;
  else if (checkTIF(file))
    return CFILE_TYPE_IMAGE_TIF;
  else if (checkBMP(file))
    return CFILE_TYPE_IMAGE_BMP;
  else if (checkSGI(file))
    return CFILE_TYPE_IMAGE_SGI;
  else if (checkJPG(file))
    return CFILE_TYPE_IMAGE_JPG;
  else if (checkXPM(file))
    return CFILE_TYPE_IMAGE_XPM;
  else if (checkPCX(file))
    return CFILE_TYPE_IMAGE_PCX;
  else if (checkPSP(file))
    return CFILE_TYPE_IMAGE_PSP;
  else if (checkPS(file))
    return CFILE_TYPE_IMAGE_PS;
  else if (checkXBM(file))
    return CFILE_TYPE_IMAGE_XBM;
  else if (checkXWD(file))
    return CFILE_TYPE_IMAGE_XWD;
  else if (checkPPM(file))
    return CFILE_TYPE_IMAGE_PPM;
  else if (checkTGA(file))
    return CFILE_TYPE_IMAGE_TGA;
  else
    return CFILE_TYPE_NONE;
}

CFileType
CFileUtil::
getImageTypeFromName(const std::string &filename)
{
  CFile file(filename);

  std::string suffix = file.getSuffix();

  if      (CStrUtil::casecmp(suffix, "bmp") == 0)
    return CFILE_TYPE_IMAGE_BMP;
  else if (CStrUtil::casecmp(suffix, "eps") == 0)
    return CFILE_TYPE_APP_EPS;
  else if (CStrUtil::casecmp(suffix, "gif") == 0)
    return CFILE_TYPE_IMAGE_GIF;
  else if (CStrUtil::casecmp(suffix, "ico") == 0)
    return CFILE_TYPE_IMAGE_ICO;
  else if (CStrUtil::casecmp(suffix, "iff") == 0)
    return CFILE_TYPE_IMAGE_IFF;
  else if (CStrUtil::casecmp(suffix, "jpg" ) == 0 ||
           CStrUtil::casecmp(suffix, "jpeg") == 0)
    return CFILE_TYPE_IMAGE_JPG;
  else if (CStrUtil::casecmp(suffix, "pcx") == 0)
    return CFILE_TYPE_IMAGE_PCX;
  else if (CStrUtil::casecmp(suffix, "png") == 0)
    return CFILE_TYPE_IMAGE_PNG;
  else if (CStrUtil::casecmp(suffix, "ppm") == 0)
    return CFILE_TYPE_IMAGE_PPM;
  else if (CStrUtil::casecmp(suffix, "psp") == 0)
    return CFILE_TYPE_IMAGE_PSP;
  else if (CStrUtil::casecmp(suffix, "ps") == 0)
    return CFILE_TYPE_IMAGE_PS;
  else if (CStrUtil::casecmp(suffix, "rgb") == 0 ||
           CStrUtil::casecmp(suffix, "sgi") == 0)
    return CFILE_TYPE_IMAGE_SGI;
  else if (CStrUtil::casecmp(suffix, "tga") == 0)
    return CFILE_TYPE_IMAGE_TGA;
  else if (CStrUtil::casecmp(suffix, "tif") == 0)
    return CFILE_TYPE_IMAGE_TIF;
  else if (CStrUtil::casecmp(suffix, "xbm") == 0)
    return CFILE_TYPE_IMAGE_XBM;
  else if (CStrUtil::casecmp(suffix, "xpm") == 0)
    return CFILE_TYPE_IMAGE_XPM;
  else if (CStrUtil::casecmp(suffix, "xwd") == 0)
    return CFILE_TYPE_IMAGE_XWD;
  else
    return CFILE_TYPE_NONE;
}

bool
CFileUtil::
checkBMP(CFileBase *file)
{
  size_t num_read;
  uchar  buffer[2];

  file->rewind();

  if (! file->read(buffer, 2, &num_read))
    return false;

  if (strncmp((char *) buffer, "BM", 2) == 0)
    return true;

  return false;
}

bool
CFileUtil::
checkGIF(CFileBase *file)
{
  size_t num_read;
  uchar  buffer[3];

  file->rewind();

  if (! file->read(buffer, 3, &num_read))
    return false;

  if (strncmp((char *) buffer, "GIF", 3) == 0)
    return true;

  return false;
}

bool
CFileUtil::
checkIFF(CFileBase *file)
{
  size_t num_read;
  uchar  buffer[4];

  file->rewind();

  if (! file->read(buffer, 4, &num_read))
    return false;

  if (strncmp((char *) buffer, "FORM", 4) == 0)
    return true;

  return false;
}

bool
CFileUtil::
checkJPG(CFileBase *file)
{
  size_t num_read;
  uchar  buffer[4];

  file->rewind();

  if (! file->read(buffer, 4, &num_read))
    return false;

  if (((buffer[0] & 0xFF) == 0xFF) &&
      ((buffer[1] & 0xFF) == 0xD8) &&
      ((buffer[2] & 0xFF) == 0xFF) &&
      ((buffer[3] & 0xFF) == 0xE0 ||
       (buffer[3] & 0xFF) == 0xE1 ||
       (buffer[3] & 0xFF) == 0xFE))
    return true;

  return false;
}

bool
CFileUtil::
checkPCX(CFileBase *file)
{
  size_t num_read;
  uchar  buffer[10];

  file->rewind();

  if (! file->read(buffer, 10, &num_read))
    return false;

  if (buffer[0] == 0x0a && buffer[1] <= 0x05 && buffer[2] == 0x01)
    return true;

  return false;
}

bool
CFileUtil::
checkPNG(CFileBase *file)
{
  static uchar signature[8] = {137, 80, 78, 71, 13, 10, 26, 10};

  size_t num_read;
  uchar  buffer[8];

  file->rewind();

  if (! file->read(buffer, 8, &num_read))
    return false;

  if (memcmp(buffer, signature, 8) == 0)
    return true;

  return false;
}

bool
CFileUtil::
checkPPM(CFileBase *file)
{
  size_t num_read;
  uchar  buffer[2];

  file->rewind();

  if (! file->read(buffer, 2, &num_read))
    return false;

  if (buffer[0] == 'P' &&
      (buffer[1] == '3' || buffer[1] == '6'))
    return true;

  return false;
}

bool
CFileUtil::
checkPSP(CFileBase *file)
{
  size_t num_read;
  uchar  buffer[25];

  file->rewind();

  if (! file->read(buffer, 25, &num_read))
    return false;

  if (strncmp((char *) buffer, "Paint Shop Pro Image File", 25) == 0)
    return true;

  return false;
}

bool
CFileUtil::
checkPS(CFileBase *file)
{
  size_t num_read;
  uchar  buffer[4];

  file->rewind();

  if (! file->read(buffer, 4, &num_read))
    return false;

  if (strncmp((char *) buffer, "%!PS", 4) == 0)
    return true;

  return false;
}

bool
CFileUtil::
checkSGI(CFileBase *file)
{
  size_t num_read;
  uchar  buffer[2];

  file->rewind();

  if (! file->read(buffer, 2, &num_read))
    return false;

  short magic = (buffer[0] << 8) + buffer[1];

  if (magic == 474)
    return true;

  return false;
}

bool
CFileUtil::
checkTGA(CFileBase *)
{
  return false;
}

bool
CFileUtil::
checkTIF(CFileBase *file)
{
  size_t num_read;
  uchar  buffer[4];

  file->rewind();

  if (! file->read(buffer, 4, &num_read))
    return false;

  if (buffer[0] == 0x49 && buffer[1] == 0x49 &&
      buffer[2] == 0x2A && buffer[3] == 0x00)
    return true;

  if (buffer[0] == 0x4D && buffer[1] == 0x4D &&
      buffer[2] == 0x00 && buffer[3] == 0x2A)
    return true;

  return false;
}

bool
CFileUtil::
checkXBM(CFileBase *file)
{
  size_t num_read;
  uchar  buffer[257];

  file->rewind();

  memset(buffer, 0, sizeof(buffer));

  if (! file->read(buffer, 256, &num_read))
    return false;

  CRegExp pattern("#define.*[0-9][0-9]*#define.*[0-9][0-9]*char.*\\[.*\\].*0x");

  if (pattern.find((char *) buffer))
    return true;

  return false;
}

bool
CFileUtil::
checkXPM(CFileBase *file)
{
  size_t num_read;
  uchar  buffer[81];

  file->rewind();

  memset(buffer, 0, sizeof(buffer));

  if (! file->read(buffer, 80, &num_read))
    return false;

  buffer[num_read] = '\0';

  if (strstr((char *) buffer, "XPM") != NULL)
    return true;

  return false;
}

bool
CFileUtil::
checkXWD(CFileBase *file)
{
  size_t num_read;
  uchar  buffer[8];

  file->rewind();

  if (! file->read(buffer, 8, &num_read))
    return false;

  uint header_size;
  uint file_version;

  memcpy(&header_size , &buffer[0], sizeof(uint));
  memcpy(&file_version, &buffer[4], sizeof(uint));

  if (file_version != XWD_FILE_VERSION) {
    file_version = CFileUtil::swapBytes32(file_version);

    if (file_version != XWD_FILE_VERSION)
      return false;

    header_size = CFileUtil::swapBytes32(header_size);
  }

  if (header_size < sz_XWDheader)
    return false;

  return true;
}

CIMAGE_INT32
CFileUtil::
swapBytes32(CIMAGE_INT32 bytes)
{
  IntData bytes1;

  bytes1.l = bytes;

  std::swap(bytes1.c[0], bytes1.c[3]);
  std::swap(bytes1.c[1], bytes1.c[2]);

  return bytes1.l;
}

CIMAGE_INT16
CFileUtil::
swapBytes16(CIMAGE_INT16 bytes)
{
  IntData bytes1;

  bytes1.s = bytes;

  std::swap(bytes1.c[0], bytes1.c[1]);

  return bytes1.s;
}
