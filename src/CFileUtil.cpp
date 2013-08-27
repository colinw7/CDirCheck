#include <CFileUtil.h>
#include <CFile.h>
#include <CStrUtil.h>

#include <cstring>

CFileType
CFileUtil::
getType(const std::string &filename)
{
  CFile file(filename);

  return getType(&file);
}

CFileType
CFileUtil::
getType(CFileBase *file)
{
  CFileType type = file->getType();

  if (type != CFILE_TYPE_INODE_REG)
    return type;

  type = getImageType(file);

  if (type != CFILE_TYPE_NONE)
    return type;

  type = getSoundType(file);

  if (type != CFILE_TYPE_NONE)
    return type;

  type = getTextType(file);

  if (type != CFILE_TYPE_NONE)
    return type;

  //------

  std::string filename = file->getName();

  type = getImageTypeFromName(filename);

  if (type != CFILE_TYPE_NONE)
    return type;

  type = getSoundTypeFromName(filename);

  if (type != CFILE_TYPE_NONE)
    return type;

  type = getTextTypeFromName(filename);

  if (type != CFILE_TYPE_NONE)
    return type;

  //------

  type = isText(file);

  if (type != CFILE_TYPE_NONE)
    return type;

  return CFILE_TYPE_TEXT_BINARY;
}

bool
CFileUtil::
isImageType(CFileType type)
{
  return (type & CFILE_TYPE_IMAGE);
}

std::string
CFileUtil::
getTypeMime(CFileType type)
{
  std::string type_str = "??";

  switch (type) {
    case CFILE_TYPE_INODE_FIFO : type_str = "inode/fifo"               ; break;
    case CFILE_TYPE_INODE_CHR  : type_str = "inode/chardevice"         ; break;
    case CFILE_TYPE_INODE_DIR  : type_str = "inode/directory"          ; break;
    case CFILE_TYPE_INODE_BLK  : type_str = "inode/blockdevice"        ; break;
    case CFILE_TYPE_INODE_REG  : type_str = "text/plain"               ; break;
    case CFILE_TYPE_INODE_LNK  : type_str = "inode/link"               ; break;
    case CFILE_TYPE_INODE_SOCK : type_str = "inode/socket"             ; break;

    case CFILE_TYPE_TEXT_PLAIN : type_str = "text/plain"               ; break;
    case CFILE_TYPE_TEXT_BINARY: type_str = "application/octet-stream" ; break;
    case CFILE_TYPE_TEXT_HTML  : type_str = "text/html"                ; break;
    case CFILE_TYPE_TEXT_XML   : type_str = "text/xml"                 ; break;
    case CFILE_TYPE_TEXT_TAR   : type_str = "application/x-tar"        ; break;
    case CFILE_TYPE_TEXT_TGZ   : type_str = "application/x-tar-gz"     ; break;
    case CFILE_TYPE_TEXT_GZIP  : type_str = "application/x-compressed" ; break;

    case CFILE_TYPE_IMAGE_BMP  : type_str = "image/x-ms-bmp"           ; break;
    case CFILE_TYPE_IMAGE_EPS  : type_str = "application/postscript"   ; break;
    case CFILE_TYPE_IMAGE_GIF  : type_str = "image/gif"                ; break;
    case CFILE_TYPE_IMAGE_ICO  : type_str = "image/x-icon"             ; break;
    case CFILE_TYPE_IMAGE_IFF  : type_str = "image/iff"                ; break;
    case CFILE_TYPE_IMAGE_JPG  : type_str = "image/jpeg"               ; break;
    case CFILE_TYPE_IMAGE_PCX  : type_str = "image/pcx"                ; break;
    case CFILE_TYPE_IMAGE_PNG  : type_str = "image/png"                ; break;
    case CFILE_TYPE_IMAGE_PPM  : type_str = "image/x-portable-pixmap"  ; break;
    case CFILE_TYPE_IMAGE_PS   : type_str = "application/postscript"   ; break;
    case CFILE_TYPE_IMAGE_PSP  : type_str = "image/psp"                ; break;
    case CFILE_TYPE_IMAGE_SGI  : type_str = "image/x-rgb"              ; break;
    case CFILE_TYPE_IMAGE_SVG  : type_str = "image/svg+html"           ; break;
    case CFILE_TYPE_IMAGE_TGA  : type_str = "image/tga"                ; break;
    case CFILE_TYPE_IMAGE_TIF  : type_str = "image/tiff"               ; break;
    case CFILE_TYPE_IMAGE_XBM  : type_str = "image/x-xbitmap"          ; break;
    case CFILE_TYPE_IMAGE_XPM  : type_str = "image/x-xpixmap"          ; break;
    case CFILE_TYPE_IMAGE_XWD  : type_str = "image/x-xwindowdump"      ; break;

    case CFILE_TYPE_APP_EXEC   : type_str = "application/octet-stream" ; break;
    case CFILE_TYPE_APP_CORE   : type_str = "application/x-coredump"   ; break;
    case CFILE_TYPE_APP_SH     : type_str = "application/x-shellscript"; break;
    case CFILE_TYPE_APP_CSH    : type_str = "application/x-shellscript"; break;
    case CFILE_TYPE_APP_KSH    : type_str = "application/x-shellscript"; break;
    case CFILE_TYPE_APP_TCL    : type_str = "text/x-tcl"               ; break;

    // TODO: Python, ...

    case CFILE_TYPE_SOUND_WAV  : type_str = "audio/x-wav"              ; break;

    default: break;
  }

  return type_str;
}

std::string
CFileUtil::
getTypeStr(CFileType type)
{
  std::string type_str = "??";

  switch (type) {
    case CFILE_TYPE_INODE_FIFO : type_str = "inode/fifo"  ; break;
    case CFILE_TYPE_INODE_CHR  : type_str = "inode/cdev"  ; break;
    case CFILE_TYPE_INODE_DIR  : type_str = "inode/dir"   ; break;
    case CFILE_TYPE_INODE_BLK  : type_str = "inode/bdev"  ; break;
    case CFILE_TYPE_INODE_REG  : type_str = "text/txt"    ; break;
    case CFILE_TYPE_INODE_LNK  : type_str = "inode/lnk"   ; break;
    case CFILE_TYPE_INODE_SOCK : type_str = "inode/socket"; break;

    case CFILE_TYPE_TEXT_PLAIN : type_str = "text/txt"    ; break;
    case CFILE_TYPE_TEXT_BINARY: type_str = "text/bin"    ; break;
    case CFILE_TYPE_TEXT_HTML  : type_str = "text/html"   ; break;
    case CFILE_TYPE_TEXT_XML   : type_str = "text/xml"    ; break;
    case CFILE_TYPE_TEXT_TAR   : type_str = "text/tar"    ; break;
    case CFILE_TYPE_TEXT_TGZ   : type_str = "text/tgz"    ; break;
    case CFILE_TYPE_TEXT_GZIP  : type_str = "text/gz"     ; break;

    case CFILE_TYPE_IMAGE_BMP  : type_str = "image/bmp"   ; break;
    case CFILE_TYPE_IMAGE_EPS  : type_str = "app/eps"     ; break;
    case CFILE_TYPE_IMAGE_GIF  : type_str = "image/gif"   ; break;
    case CFILE_TYPE_IMAGE_ICO  : type_str = "image/ico"   ; break;
    case CFILE_TYPE_IMAGE_IFF  : type_str = "image/iff"   ; break;
    case CFILE_TYPE_IMAGE_JPG  : type_str = "image/jpg"   ; break;
    case CFILE_TYPE_IMAGE_PCX  : type_str = "image/pcx"   ; break;
    case CFILE_TYPE_IMAGE_PNG  : type_str = "image/png"   ; break;
    case CFILE_TYPE_IMAGE_PPM  : type_str = "image/ppm"   ; break;
    case CFILE_TYPE_IMAGE_PS   : type_str = "app/ps"      ; break;
    case CFILE_TYPE_IMAGE_PSP  : type_str = "image/psp"   ; break;
    case CFILE_TYPE_IMAGE_SGI  : type_str = "image/rgb"   ; break;
    case CFILE_TYPE_IMAGE_SVG  : type_str = "text/svg"    ; break;
    case CFILE_TYPE_IMAGE_TGA  : type_str = "image/tga"   ; break;
    case CFILE_TYPE_IMAGE_TIF  : type_str = "image/tif"   ; break;
    case CFILE_TYPE_IMAGE_XBM  : type_str = "image/xbm"   ; break;
    case CFILE_TYPE_IMAGE_XPM  : type_str = "image/xpm"   ; break;
    case CFILE_TYPE_IMAGE_XWD  : type_str = "image/xwd"   ; break;

    case CFILE_TYPE_APP_EXEC   : type_str = "app/exe"     ; break;
    case CFILE_TYPE_APP_CORE   : type_str = "app/core"    ; break;
    case CFILE_TYPE_APP_SH     : type_str = "app/sh"      ; break;
    case CFILE_TYPE_APP_CSH    : type_str = "app/csh"     ; break;
    case CFILE_TYPE_APP_KSH    : type_str = "app/ksh"     ; break;

    // TODO: Python, ...

    case CFILE_TYPE_SOUND_WAV  : type_str = "audio/wav"   ; break;

    default:                                                break;
  }

  return type_str;
}

CFileType
CFileUtil::
getStrType(const std::string &type_str)
{
  CFileType type = CFILE_TYPE_NONE;

  if      (type_str == "inode/fifo"  ) type = CFILE_TYPE_INODE_FIFO ;
  else if (type_str == "inode/cdev"  ) type = CFILE_TYPE_INODE_CHR  ;
  else if (type_str == "inode/dir"   ) type = CFILE_TYPE_INODE_DIR  ;
  else if (type_str == "inode/bdev"  ) type = CFILE_TYPE_INODE_BLK  ;
  else if (type_str == "inode/lnk"   ) type = CFILE_TYPE_INODE_LNK  ;
  else if (type_str == "inode/socket") type = CFILE_TYPE_INODE_SOCK ;

  else if (type_str == "text/txt"    ) type = CFILE_TYPE_TEXT_PLAIN ;
  else if (type_str == "text/bin"    ) type = CFILE_TYPE_TEXT_BINARY;
  else if (type_str == "text/html"   ) type = CFILE_TYPE_TEXT_HTML  ;
  else if (type_str == "text/xml"    ) type = CFILE_TYPE_TEXT_XML   ;
  else if (type_str == "text/tar"    ) type = CFILE_TYPE_TEXT_TAR   ;
  else if (type_str == "text/tgz"    ) type = CFILE_TYPE_TEXT_TGZ   ;
  else if (type_str == "text/gz"     ) type = CFILE_TYPE_TEXT_GZIP  ;

  else if (type_str == "image/bmp"   ) type = CFILE_TYPE_IMAGE_BMP  ;
  else if (type_str == "app/eps"     ) type = CFILE_TYPE_IMAGE_EPS  ;
  else if (type_str == "image/gif"   ) type = CFILE_TYPE_IMAGE_GIF  ;
  else if (type_str == "image/ico"   ) type = CFILE_TYPE_IMAGE_ICO  ;
  else if (type_str == "image/iff"   ) type = CFILE_TYPE_IMAGE_IFF  ;
  else if (type_str == "image/jpg"   ) type = CFILE_TYPE_IMAGE_JPG  ;
  else if (type_str == "image/pcx"   ) type = CFILE_TYPE_IMAGE_PCX  ;
  else if (type_str == "image/png"   ) type = CFILE_TYPE_IMAGE_PNG  ;
  else if (type_str == "image/ppm"   ) type = CFILE_TYPE_IMAGE_PPM  ;
  else if (type_str == "app/ps"      ) type = CFILE_TYPE_IMAGE_PS   ;
  else if (type_str == "image/psp"   ) type = CFILE_TYPE_IMAGE_PSP  ;
  else if (type_str == "image/rgb"   ) type = CFILE_TYPE_IMAGE_SGI  ;
  else if (type_str == "text/svg"    ) type = CFILE_TYPE_IMAGE_SVG  ;
  else if (type_str == "image/tga"   ) type = CFILE_TYPE_IMAGE_TGA  ;
  else if (type_str == "image/tif"   ) type = CFILE_TYPE_IMAGE_TIF  ;
  else if (type_str == "image/xbm"   ) type = CFILE_TYPE_IMAGE_XBM  ;
  else if (type_str == "image/xpm"   ) type = CFILE_TYPE_IMAGE_XPM  ;
  else if (type_str == "image/xwd"   ) type = CFILE_TYPE_IMAGE_XWD  ;

  else if (type_str == "app/exe"     ) type = CFILE_TYPE_APP_EXEC   ;
  else if (type_str == "app/core"    ) type = CFILE_TYPE_APP_CORE   ;
  else if (type_str == "app/sh"      ) type = CFILE_TYPE_APP_SH     ;
  else if (type_str == "app/csh"     ) type = CFILE_TYPE_APP_CSH    ;
  else if (type_str == "app/ksh"     ) type = CFILE_TYPE_APP_KSH    ;

  // TODO: Python, ...

  else if (type_str == "audio/wav"   ) type = CFILE_TYPE_SOUND_WAV  ;

  return type;
}

std::string
CFileUtil::
getPrefix(CFileType type)
{
  std::string prefix = "??";

  switch (type) {
    case CFILE_TYPE_INODE_FIFO : prefix = "fifo"  ; break;
    case CFILE_TYPE_INODE_CHR  : prefix = "cdev"  ; break;
    case CFILE_TYPE_INODE_DIR  : prefix = ""      ; break;
    case CFILE_TYPE_INODE_BLK  : prefix = "bdev"  ; break;
    case CFILE_TYPE_INODE_REG  : prefix = "txt"   ; break;
    case CFILE_TYPE_INODE_LNK  : prefix = "lnk"   ; break;
    case CFILE_TYPE_INODE_SOCK : prefix = "socket"; break;

    case CFILE_TYPE_TEXT_PLAIN : prefix = "txt"   ; break;
    case CFILE_TYPE_TEXT_BINARY: prefix = "bin"   ; break;
    case CFILE_TYPE_TEXT_HTML  : prefix = "html"  ; break;
    case CFILE_TYPE_TEXT_XML   : prefix = "xml"   ; break;
    case CFILE_TYPE_TEXT_TAR   : prefix = "tar"   ; break;
    case CFILE_TYPE_TEXT_TGZ   : prefix = "tgz"   ; break;
    case CFILE_TYPE_TEXT_GZIP  : prefix = "gz"    ; break;

    case CFILE_TYPE_IMAGE_BMP  : prefix = "bmp"   ; break;
    case CFILE_TYPE_IMAGE_EPS  : prefix = "eps"   ; break;
    case CFILE_TYPE_IMAGE_GIF  : prefix = "gif"   ; break;
    case CFILE_TYPE_IMAGE_ICO  : prefix = "ico"   ; break;
    case CFILE_TYPE_IMAGE_IFF  : prefix = "iff"   ; break;
    case CFILE_TYPE_IMAGE_JPG  : prefix = "jpg"   ; break;
    case CFILE_TYPE_IMAGE_PCX  : prefix = "pcx"   ; break;
    case CFILE_TYPE_IMAGE_PNG  : prefix = "png"   ; break;
    case CFILE_TYPE_IMAGE_PPM  : prefix = "ppm"   ; break;
    case CFILE_TYPE_IMAGE_PS   : prefix = "ps"    ; break;
    case CFILE_TYPE_IMAGE_PSP  : prefix = "psp"   ; break;
    case CFILE_TYPE_IMAGE_SGI  : prefix = "rgb"   ; break;
    case CFILE_TYPE_IMAGE_SVG  : prefix = "svg"   ; break;
    case CFILE_TYPE_IMAGE_TGA  : prefix = "tga"   ; break;
    case CFILE_TYPE_IMAGE_TIF  : prefix = "tif"   ; break;
    case CFILE_TYPE_IMAGE_XBM  : prefix = "xbm"   ; break;
    case CFILE_TYPE_IMAGE_XPM  : prefix = "xpm"   ; break;
    case CFILE_TYPE_IMAGE_XWD  : prefix = "xwd"   ; break;

    case CFILE_TYPE_APP_EXEC   : prefix = "exe"   ; break;
    case CFILE_TYPE_APP_CORE   : prefix = ""      ; break;
    case CFILE_TYPE_APP_SH     : prefix = "sh"    ; break;
    case CFILE_TYPE_APP_CSH    : prefix = "csh"   ; break;
    case CFILE_TYPE_APP_KSH    : prefix = "ksh"   ; break;

    // TODO: Python, ...

    case CFILE_TYPE_SOUND_WAV  : prefix = "wav"   ; break;

    default:                                        break;
  }

  return prefix;
}

bool
CFileUtil::
isBadFilename(const std::string &name)
{
//static char file65_chars[] = "abcdefghijklmnopqrstuvwxyz"
//                             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
//                             "1234567890"
//                             "_.-";

  static char file_chars[] = "abcdefghijklmnopqrstuvwxyz"
                             "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                             "1234567890"
                             "_.-+:;~@%";

  uint len = name.size();

  if (len == 0) return true;

  if (name[0] == '-')
    return true;

  for (uint i = 0; i < len; ++i)
    if (strchr(file_chars, name[i]) == NULL)
      return true;

  return false;
}
