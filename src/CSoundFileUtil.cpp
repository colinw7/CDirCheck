#include <CFileUtil.h>
#include <CFile.h>
#include <CStrUtil.h>

#include <cstring>

CFileType
CFileUtil::
getSoundType(CFileBase *file)
{
  // Order Important

  if      (CFileUtil::checkWAV(file))
    return CFILE_TYPE_SOUND_WAV;
  else
    return CFILE_TYPE_NONE;
}

CFileType
CFileUtil::
getSoundTypeFromName(const std::string &filename)
{
  CFile file(filename);

  std::string suffix = file.getSuffix();

  if      (CStrUtil::casecmp(suffix, "wav") == 0)
    return CFILE_TYPE_SOUND_WAV;
  else
    return CFILE_TYPE_NONE;
}

bool
CFileUtil::
checkWAV(CFileBase *file)
{
  struct Header {
    char riff[4];
    uint size;
    char wave[4];
  };

  Header header;

  file->rewind();

  if (! file->read((uchar *) &header, sizeof(header)))
    return false;

  if (strncmp(header.riff, "RIFF", 4) != 0)
    return false;

  if (strncmp(header.wave, "WAVE", 4) != 0)
    return false;

  return true;
}
