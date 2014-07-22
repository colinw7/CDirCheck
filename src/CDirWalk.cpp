#include <CDirWalk.h>

#include <CDir.h>
#include <CDirScan.h>
#include <COSFile.h>

CDirWalk::
CDirWalk(const std::string &dirname) :
 dirname_(dirname)
{
}

CDirWalk::
~CDirWalk()
{
}

void
CDirWalk::
setDirName(const std::string &dirname)
{
  dirname_ = dirname;
}

std::string
CDirWalk::
getFilePath() const
{
  uint num_dirs = dirs_.size();

  if (num_dirs == 0)
    return filename_;

  return getDirPath() + "/" + filename_;
}

std::string
CDirWalk::
getDirPath() const
{
  uint num_dirs = dirs_.size();

  if (num_dirs == 0)
    return "";

  std::string path = dirs_[num_dirs - 1];

  for (int i = num_dirs - 2; i >= 0; --i)
    path = dirs_[i] + "/" + path;

  return path;
}

bool
CDirWalk::
walk()
{
  dirs_.clear();

  bool entered = false;

  if (dirname_ != ".") {
    dirs_.push_back(dirname_);

    if (! CDir::enter(dirname_))
      return false;

    entered = true;
  }

  bool rc = doWalk();

  if (entered)
    CDir::leave();

  return rc;
}

bool
CDirWalk::
doWalk()
{
  CDirScan scan;

  while (scan.next(filename_)) {
    if (lstat(filename_.c_str(), &stat_) < 0)
      continue;

    if (COSFile::stat_is_dir(&stat_)) {
      type_ = CFILE_TYPE_INODE_DIR;

      if (doEnter()) {
        doWalk();

        doLeave();
      }
    }
    else {
      type_ = CFILE_TYPE_INODE_REG;

      process();
    }
  }

  return true;
}

bool
CDirWalk::
doEnter()
{
  if (! CDir::enter(filename_))
    return false;

  if (! checkDir(filename_)) {
    CDir::leave();
    return false;
  }

  dirs_.push_back(filename_);

  enter();

  return true;
}

void
CDirWalk::
doLeave()
{
  leave();

  CDir::leave();

  dirs_.pop_back();
}

bool
CDirWalk::
checkDir(const std::string &)
{
  return true;
}
