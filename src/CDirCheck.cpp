#include <CDirCheck.h>
#include <CDir.h>
#include <CDirWalk.h>
#include <CFileUtil.h>
#include <CRegExp.h>

#include <iostream>

class CDirTreeWalk : public CDirWalk {
 public:
  CDirTreeWalk(CDirCheck *check);

 private:
  bool checkDir(const std::string &dirname);

  void process();

 private:
  CDirCheck *check_;
};

//----------

CDirCheck::
CDirCheck(const std::string &dirName) :
 dirName_(dirName), matchDir_(0), matchFile_(0), ignoreDir_(0), ignoreFile_(0)
{
  CFile dir(dirName_);

  dirPath_ = dir.getDir();
}

void
CDirCheck::
addTest(TestType type)
{
  if      (type == ZERO_LENGTH)  tests.zero_length = true;
  else if (type == DUPLICATE  )  tests.duplicate   = true;
  else if (type == DUP_NAME   )  tests.dup_name    = true;
  else if (type == BAD_NAME   )  tests.bad_name    = true;
  else if (type == BAD_LINK   )  tests.bad_link    = true;
  else if (type == BIGGER     )  tests.bigger      = true;
  else if (type == SMALLER    )  tests.smaller     = true;
}

void
CDirCheck::
setBigger(uint value)
{
  tests.bigger_value = value;
}

void
CDirCheck::
setSmaller(uint value)
{
  tests.smaller_value = value;
}

void
CDirCheck::
setMatchDir(const std::string &pattern)
{
  delete matchDir_;

  matchDir_ = new CRegExp(pattern);

  matchDir_->setExtended(true);
  matchDir_->setMatchBOL(false);
  matchDir_->setMatchEOL(false);
}

void
CDirCheck::
setMatchFile(const std::string &pattern)
{
  delete matchFile_;

  matchFile_ = new CRegExp(pattern);

  matchFile_->setExtended(true);
  matchFile_->setMatchBOL(false);
  matchFile_->setMatchEOL(false);
}

void
CDirCheck::
setIgnoreDir(const std::string &pattern)
{
  delete ignoreDir_;

  ignoreDir_ = new CRegExp(pattern);

  ignoreDir_->setExtended(true);
  ignoreDir_->setMatchBOL(false);
  ignoreDir_->setMatchEOL(false);
}

void
CDirCheck::
setIgnoreFile(const std::string &pattern)
{
  delete ignoreFile_;

  ignoreFile_ = new CRegExp(pattern);

  ignoreFile_->setExtended(true);
  ignoreFile_->setMatchBOL(false);
  ignoreFile_->setMatchEOL(false);
}

void
CDirCheck::
exec()
{
  CFileBase::setUseLStat(true);

  CDirTreeWalk walk(this);

  walk.walk();

  process();
}

void
CDirCheck::
addFile(const std::string &path)
{
  CDirCheckFile *file = new CDirCheckFile(path);

  fileList_.push_back(file);
}

void
CDirCheck::
process()
{
  FileList::const_iterator p1, p2;

  if (tests.duplicate) {
    CDirCheckFileMap *map = CDirCheckFileMapInst;

    for (p1 = fileList_.begin(), p2 = fileList_.end(); p1 != p2; ++p1) {
      CDirCheckFile *file = *p1;

      map->addFile(file);
    }
  }

  if (tests.dup_name) {
    CDirDupNameMap *map = CDirDupNameMapInst;

    for (p1 = fileList_.begin(), p2 = fileList_.end(); p1 != p2; ++p1) {
      CDirCheckFile *file = *p1;

      map->addFile(file);
    }
  }

  for (p1 = fileList_.begin(), p2 = fileList_.end(); p1 != p2; ++p1) {
    CDirCheckFile *file = *p1;

    if (matchFile_ && ! matchFile_->find(file->getFile().getName()))
      continue;

    if (ignoreFile_ && ignoreFile_->find(file->getFile().getName()))
      continue;

    if (tests.bad_name && CFileUtil::isBadFilename(file->getFile().getName()))
      file->setFail(CDirCheck::BAD_NAME);

    if (tests.bad_link && file->isLink() && ! file->isLinkExists())
      file->setFail(CDirCheck::BAD_LINK);

    if (tests.zero_length && ! file->isLink() && file->getLen() == 0)
      file->setFail(CDirCheck::ZERO_LENGTH);

    bool seen = false;

    if (tests.duplicate && file->isDuplicate(&seen) && ! seen)
      file->setFail(DUPLICATE);

    if (tests.bigger && file->getLen() > tests.bigger_value)
      file->setFail(CDirCheck::BIGGER);

    if (tests.smaller && file->getLen() < tests.smaller_value)
      file->setFail(CDirCheck::SMALLER);
  }

  if (tests.dup_name) {
    for (p1 = fileList_.begin(), p2 = fileList_.end(); p1 != p2; ++p1) {
      CDirCheckFile *file = *p1;

      if (matchFile_ && ! matchFile_->find(file->getFile().getName()))
        continue;

      if (ignoreFile_ && ignoreFile_->find(file->getFile().getName()))
        continue;

      bool seen = false;

      if (tests.duplicate && file->isDuplicate(&seen)) continue;

      if (file->isDupName())
        file->setFail(DUP_NAME);
    }
  }

  for (p1 = fileList_.begin(), p2 = fileList_.end(); p1 != p2; ++p1) {
    CDirCheckFile *file = *p1;

    uint fail = file->getFail();

    if (! fail) continue;

    std::cerr << simplifyPath(file->getFile().getPath()) << ":";

    if (fail & ZERO_LENGTH)
      std::cerr << " ZeroLength";

    if (fail & BIGGER)
      std::cerr << " Bigger";

    if (fail & SMALLER)
      std::cerr << " Smaller";

    if (fail & BAD_LINK)
      std::cerr << " BadLink";

    if (fail & BAD_NAME)
      std::cerr << " BadName";

    if (fail & DUPLICATE) {
      std::cerr << " Duplicate (";

      const CDirCheckFile::FileList &files = file->getDuplicateFiles();

      CDirCheckFile::FileList::const_iterator pd1, pd2;

      for (pd1 = files.begin(), pd2 = files.end(); pd1 != pd2; ++pd1) {
        if (pd1 != files.begin()) std::cerr << " ";

        std::cerr << simplifyPath((*pd1)->getFile().getPath());
      }

      std::cerr << ")";
    }

    if (fail & DUP_NAME) {
      std::cerr << " Dup Name (";

      const CDirCheckFile::FileList &files = CDirDupNameMapInst->getDupNames(file);

      CDirCheckFile::FileList::const_iterator pd1, pd2;

      for (pd1 = files.begin(), pd2 = files.end(); pd1 != pd2; ++pd1) {
        if (pd1 != files.begin()) std::cerr << " ";

        std::cerr << simplifyPath((*pd1)->getFile().getPath());
      }

      std::cerr << ")";
    }

    std::cerr << std::endl;
  }
}

std::string
CDirCheck::
simplifyPath(const std::string &path) const
{
  uint dirSize = dirPath_.size();

  std::string path1 = path;

  if (path1.size() > dirSize && path1.substr(0, dirSize) == dirPath_ && path1[dirSize] == '/')
    path1 = path1.substr(dirSize + 1);

  return path1;
}

bool
CDirCheck::
checkDir(const std::string &dirname)
{
  if (matchDir_)
    return matchDir_->find(dirname);

  if (ignoreDir_)
    return ! ignoreDir_->find(dirname);

  return true;
}

//----------

CDirTreeWalk::
CDirTreeWalk(CDirCheck *check) :
 CDirWalk(check->getDirName()), check_(check)
{
}

bool
CDirTreeWalk::
checkDir(const std::string &dirname)
{
  return check_->checkDir(dirname);
}

void
CDirTreeWalk::
process()
{
  const std::string &fileName = getFileName();

  check_->addFile(fileName);
}

//-----------

CDirCheckFile::
CDirCheckFile(const std::string &fileName) :
 CDirFile(fileName)
{
  fail_ = 0;
}

void
CDirCheckFile::
setFail(uint flag)
{
  fail_ |= flag;
}

bool
CDirCheckFile::
isDuplicate(bool *seen) const
{
  if (is_link_) return false;

  if (! duplicateFiles_.empty()) {
    *seen = true;

    return true;
  }

  CDirCheckFile *th = const_cast<CDirCheckFile *>(this);

  //th->duplicateFiles_.clear();

  if (getLen() == 0) return false;

  const CDirCheckFileMap::FileList &files = CDirCheckFileMapInst->getCheckSumFiles(getCheckSum());

  CDirCheckFileMap::FileList::const_iterator p1, p2;

  for (p1 = files.begin(), p2 = files.end(); p1 != p2; ++p1) {
    CDirCheckFile *file1 = *p1;

    if (file1 < this) continue;

    if (getLen() != file1->getLen()) continue;

    if (dataEqual(this, file1)) {
      th->duplicateFiles_.push_back(file1);

      file1->duplicateFiles_.push_back(th);
    }

    file1->close();
  }

  return ! duplicateFiles_.empty();
}

bool
CDirCheckFile::
isDupName() const
{
  if (is_link_) return false;

  return CDirDupNameMapInst->isDupName(this);
}

void
CDirCheckFile::
close()
{
  getFile().close();
}

//----------

CDirCheckFileMap *
CDirCheckFileMap::
getInstance()
{
  static CDirCheckFileMap *instance;

  if (! instance)
    instance = new CDirCheckFileMap;

  return instance;
}

void
CDirCheckFileMap::
addFile(CDirCheckFile *file)
{
  lengthMap_  [file->getLen()     ].push_back(file);
  checkSumMap_[file->getCheckSum()].push_back(file);
}

const CDirCheckFileMap::FileList &
CDirCheckFileMap::
getCheckSumFiles(const CFileCheckSum &checksum) const
{
  static FileList null_files;

  CheckSumMap::const_iterator p = checkSumMap_.find(checksum);

  if (p != checkSumMap_.end())
    return (*p).second;

  return null_files;
}

//----------

CDirDupNameMap *
CDirDupNameMap::
getInstance()
{
  static CDirDupNameMap *instance;

  if (! instance)
    instance = new CDirDupNameMap;

  return instance;
}

void
CDirDupNameMap::
addFile(CDirCheckFile *file)
{
  nameMap_[file->getFile().getName()].push_back(file);
}

CDirFile::FileList
CDirDupNameMap::
getDupNames(CDirCheckFile *file) const
{
  CDirFile::FileList files;

  const NameMap::const_iterator p = nameMap_.find(file->getFile().getName());

  if (p == nameMap_.end())
    return files;

  const FileList &files1 = (*p).second;

  for (FileList::const_iterator p = files1.begin(); p != files1.end(); ++p) {
    if (*p != file)
      files.push_back(*p);
  }

  return files;
}

bool
CDirDupNameMap::
isDupName(const CDirCheckFile *file) const
{
  const NameMap::const_iterator p = nameMap_.find(file->getFile().getName());

  if (p == nameMap_.end() || (*p).second.size() < 2) return false;

  return (file == (*p).second.front());
}
