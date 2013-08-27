#include <CFileUtil.h>

#include <std_os.h>

#include <CStrUtil.h>
#include <CRegExp.h>
#include <CTarDef.h>
#include <CFile.h>

#ifdef OS_LINUX
#include <elf.h>
#endif

#include <cstdio>
#include <cstring>

CFileType
CFileUtil::
getTextType(const std::string &filename)
{
  CFile file(filename);

  return getTextType(&file);
}

CFileType
CFileUtil::
getTextType(CFileBase *file)
{
  CFileType type;

  type = isELF(file);

  if (type != CFILE_TYPE_NONE)
    return type;

  type = isGZip(file);

  if (type != CFILE_TYPE_NONE)
    return type;

  type = isScript(file);

  if (type != CFILE_TYPE_NONE)
    return type;

  type = isHtml(file);

  if (type != CFILE_TYPE_NONE)
    return type;

  type = isTar(file);

  if (type != CFILE_TYPE_NONE)
    return type;

  return CFILE_TYPE_NONE;
}

CFileType
CFileUtil::
getTextTypeFromName(const std::string &filename)
{
  CFile file(filename);

  std::string suffix = file.getSuffix();

  if      (CStrUtil::casecmp(suffix, "exe") == 0)
    return CFILE_TYPE_APP_EXEC;
  else if (CStrUtil::casecmp(suffix, "txt") == 0)
    return CFILE_TYPE_TEXT_PLAIN;
  else if (CStrUtil::casecmp(suffix, "sh") == 0)
    return CFILE_TYPE_APP_SH;
  else if (CStrUtil::casecmp(suffix, "csh") == 0)
    return CFILE_TYPE_APP_CSH;
  else if (CStrUtil::casecmp(suffix, "ksh") == 0)
    return CFILE_TYPE_APP_KSH;
  else if (CStrUtil::casecmp(suffix, "htm" ) == 0 ||
           CStrUtil::casecmp(suffix, "html") == 0)
    return CFILE_TYPE_TEXT_HTML;
  else if (CStrUtil::casecmp(suffix, "svg") == 0)
    return CFILE_TYPE_IMAGE_SVG;
  else if (CStrUtil::casecmp(suffix, "xml") == 0)
    return CFILE_TYPE_TEXT_XML;
  else if (CStrUtil::casecmp(suffix, "tcl") == 0 ||
           CStrUtil::casecmp(suffix, "tk" ) == 0)
    return CFILE_TYPE_APP_TCL;
  else if (CStrUtil::casecmp(suffix, "php") == 0)
    return CFILE_TYPE_APP_PHP;
  else if (CStrUtil::casecmp(suffix, "java") == 0)
    return CFILE_TYPE_APP_JAVA;
  else if (CStrUtil::casecmp(suffix, "tar") == 0)
    return CFILE_TYPE_TEXT_TAR;
  else if (CStrUtil::casecmp(suffix, "tgz") == 0)
    return CFILE_TYPE_TEXT_TGZ;
  else
    return CFILE_TYPE_NONE;
}

CFileType
CFileUtil::
isELF(CFileBase *file)
{
  file->rewind();

  size_t num;
  uchar  buffer[17];

  if (! file->read(buffer, 16, &num))
    return CFILE_TYPE_NONE;

  buffer[16] = '\0';

  if (buffer[0] != 0x7f || strncmp((char *) &buffer[1], "ELF", 3) != 0)
    return CFILE_TYPE_NONE;

  short type;

  if (! file->read((uchar *) &type, sizeof(type), NULL))
    return CFILE_TYPE_NONE;

#ifdef OS_LINUX
  if (type == ET_CORE)
    return CFILE_TYPE_APP_CORE;
#endif

  return CFILE_TYPE_APP_EXEC;
}

CFileType
CFileUtil::
isGZip(CFileBase *file)
{
  file->rewind();

  size_t num;
  uchar  buffer[17];

  if (! file->read(buffer, 16, &num))
    return CFILE_TYPE_NONE;

  buffer[16] = '\0';

  if (buffer[0] != 0x1f || buffer[1] != 0x8b)
    return CFILE_TYPE_NONE;

  return CFILE_TYPE_TEXT_GZIP;
}

CFileType
CFileUtil::
isScript(CFileBase *file)
{
  file->rewind();

  std::string line;

  while (true) {
    if (! file->readLine(line))
      return CFILE_TYPE_NONE;

    line = CStrUtil::stripSpaces(line);

    if (line != "")
      break;
  }

  if (CRegExp::find("#! *.*/csh", line) ||
      CRegExp::find("#! *.*/tcsh", line))
    return CFILE_TYPE_APP_CSH;

  if (CRegExp::find("#! *.*/ksh", line))
    return CFILE_TYPE_APP_KSH;

  if (CRegExp::find("#! *.*/sh", line) ||
      CRegExp::find("#! *.*/bash", line))
    return CFILE_TYPE_APP_SH;

  return CFILE_TYPE_NONE;
}

CFileType
CFileUtil::
isHtml(CFileBase *file)
{
  file->rewind();

  std::string line;

  while (true) {
    if (! file->readLine(line))
      return CFILE_TYPE_NONE;

    line = CStrUtil::stripSpaces(line);

    if (line != "")
      break;
  }

  if (CRegExp::find(" *< *html *>", line))
    return CFILE_TYPE_TEXT_HTML;

  if (CRegExp::find(" *<!.*DOCTYPE.*svg", line))
    return CFILE_TYPE_IMAGE_SVG;

  if (CRegExp::find(" *< *svg.*>", line))
    return CFILE_TYPE_IMAGE_SVG;

  if (CRegExp::find(" *<.*>", line))
    return CFILE_TYPE_TEXT_XML;

  return CFILE_TYPE_NONE;
}

CFileType
CFileUtil::
isTar(CFileBase *file)
{
  file->rewind();

  size_t     num;
  CTarHeader block;

  if (! file->read((uchar *) block.dummy, sizeof(block), &num))
    return CFILE_TYPE_NONE;

  if (num != sizeof(block))
    return CFILE_TYPE_NONE;

  if (strncmp(block.pdbuf.magic, CTAR_MAGIC, CTAR_MAGLEN - 1) != 0)
    return CFILE_TYPE_NONE;

  int chksum = tarOctalFieldToInt(block.pdbuf.chksum, 8);

  if (chksum != tarCalculateChecksum(&block))
    return CFILE_TYPE_NONE;

  return CFILE_TYPE_TEXT_TAR;
}

int
CFileUtil::
tarCalculateChecksum(CTarHeader *hblock)
{
  strncpy(hblock->pdbuf.chksum, "        ", 8);

  int total = 0;

  for (int i = 0; i < CTAR_HEADER_SIZE; i++)
    total += (int) hblock->dummy[i];

  return total;
}

int
CFileUtil::
tarOctalFieldToInt(char *field, int field_size)
{
  static char temp_string[32];

  strncpy(temp_string, field, field_size);

  temp_string[field_size] = '\0';

  return tarOctalStringToInt(temp_string);
}

int
CFileUtil::
tarOctalStringToInt(const std::string &str)
{
  std::string str1 = CStrUtil::stripSpaces(str);

  int integer = 0;

  sscanf(str1.c_str(), "%o", &integer);

  return integer;
}

CFileType
CFileUtil::
isText(CFileBase *file)
{
  file->rewind();

  int num = 0;

  int num_ctrl  = 0;
  int num_print = 0;

  int c;

  while ((c = file->getC()) != EOF && num < 4096) {
    if (c == '\0')
      break;

    if (! isspace(c) && (iscntrl(c) || ! isascii(c)))
      num_ctrl++;
    else
      num_print++;

    num++;
  }

  if (c == '\0')
    return CFILE_TYPE_TEXT_BINARY;

  if (num == 0)
    return CFILE_TYPE_NONE;

  if (num_ctrl*10 >= num_print)
    return CFILE_TYPE_TEXT_BINARY;

  return CFILE_TYPE_TEXT_PLAIN;
}
