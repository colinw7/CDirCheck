#include <CDirScan.h>
#include <COSFile.h>

CDirScan::
CDirScan(const std::string &dirname) :
 dirname_(dirname), dir_scope_(dirname), inited_(false)
{
  if (init())
    inited_ = true;
}

CDirScan::
~CDirScan()
{
  if (inited_)
    term();
}

bool
CDirScan::
init()
{
  if (! dir_scope_.valid())
    return false;

  handle_ = COSFile::openDir(".");

  return true;
}

bool
CDirScan::
next(std::string &filename)
{
  if (! dir_scope_.valid())
    return false;

  if (! COSFile::readDir(handle_, filename))
    return false;

  return true;
}

bool
CDirScan::
term()
{
  if (! dir_scope_.valid())
    return false;

  COSFile::closeDir(handle_);

  return true;
}
