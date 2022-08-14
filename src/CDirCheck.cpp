#include <CDirCheck.h>
#include <CDir.h>
#include <CDirWalk.h>
#include <CFileUtil.h>
#include <CRegExp.h>

#include <set>
#include <iostream>

class CDirTreeWalk : public CDirWalk {
 public:
  CDirTreeWalk(CDirCheck *check);

 private:
  bool checkDir(const std::string &dirname);

  void enter();
  void leave();

  void process();

 private:
  CDirCheck*        check_ { 0 };
  std::string       dirName_;
  std::vector<uint> dirCount_;
};

//----------

CDirCheck::
CDirCheck(const std::string &dirName) :
 dirName_(dirName)
{
  CFile dir(dirName_);

  dirPath_ = dir.getDir();
}

void
CDirCheck::
addTest(TestType type)
{
  if      (type == ZERO_LENGTH) tests.zero_length = true;
  else if (type == DUPLICATE  ) tests.duplicate   = true;
  else if (type == DUP_NAME   ) tests.dup_name    = true;
  else if (type == BAD_NAME   ) tests.bad_name    = true;
  else if (type == BAD_LINK   ) tests.bad_link    = true;
  else if (type == BIGGER     ) tests.bigger      = true;
  else if (type == SMALLER    ) tests.smaller     = true;
  else if (type == EMPTY      ) tests.empty       = true;
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
setRemove(bool remove)
{
  remove_ = remove;
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
addEmptyDir(const std::string &path)
{
  emptyDirList_.push_back(path);
}

void
CDirCheck::
addFile(const std::string &path)
{
  auto *file = new CDirCheckFile(path);

  fileList_.push_back(file);
}

void
CDirCheck::
process()
{
  if (tests.duplicate) {
    CDirCheckFileMap *map = CDirCheckFileMapInst;

    for (const auto &file : fileList_)
      map->addFile(file);
  }

  if (tests.dup_name) {
    CDirDupNameMap *map = CDirDupNameMapInst;

    for (const auto &file : fileList_)
      map->addFile(file);
  }

  for (const auto &file : fileList_) {
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
    for (const auto &file : fileList_) {
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

  if (tests.empty) {
    for (const auto &dir : emptyDirList_) {
      if (remove_)
        std::cout << "rmdir " << dir << std::endl;
      else
        std::cerr << dir << " : EmptyDir " << std::endl;
    }
  }

  for (const auto &file : fileList_) {
    uint fail = file->getFail();

    if (! fail) continue;

    std::string filename = simplifyPath(file->getFile().getPath());

    if (remove_) {
      std::cout << "rm " << filename << std::endl;
    }
    else {
      if (fail != DUPLICATE)
        std::cerr << filename << " :";

      if (fail & ZERO_LENGTH) std::cerr << " ZeroLength";
      if (fail & BIGGER     ) std::cerr << " Bigger";
      if (fail & SMALLER    ) std::cerr << " Smaller";
      if (fail & BAD_LINK   ) std::cerr << " BadLink";
      if (fail & BAD_NAME   ) std::cerr << " BadName";

      if (fail & DUPLICATE) {
        const CDirCheckFile::FileList &dupFiles = file->getDuplicateFiles();

        std::set<std::string> names;

        if (fail == DUPLICATE) {
          names.insert(file->getFile().getName());

          for (const auto &dupFile : dupFiles)
            names.insert(dupFile->getFile().getName());
        }

        if (names.size() == 1) {
          std::cerr << "Duplicates for " << file->getFile().getName() << " :" << std::endl;

          std::cerr << "   " << simplifyPath(file->getFile().getPath());

          for (const auto &dupFile : dupFiles)
            std::cerr << " " << simplifyPath(dupFile->getFile().getPath());
        }
        else {
          if (fail == DUPLICATE)
            std::cerr << "Duplicates for " << filename << " :" << std::endl << "  ";
          else
            std::cerr << " Duplicate (";

          int i = 0;

          for (const auto &dupFile : dupFiles) {
            if (i > 0) std::cerr << " ";

            std::cerr << simplifyPath(dupFile->getFile().getPath());

            ++i;
          }

          std::cerr << ")";
        }
      }

      if (fail & DUP_NAME) {
        bool first = true;

        std::cerr << " Dup Name (";

        const auto &files = CDirDupNameMapInst->getDupNames(file);

        for (const auto &dfile : files) {
          if (! first) std::cerr << " ";

          std::cerr << simplifyPath(dfile->getFile().getPath());

          first = false;
        }

        std::cerr << ")";
      }

      std::cerr << std::endl;
    }
  }
}

std::string
CDirCheck::
simplifyPath(const std::string &path) const
{
  auto dirSize = dirPath_.size();

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
  dirCount_.push_back(0);
}

bool
CDirTreeWalk::
checkDir(const std::string &dirname)
{
  return check_->checkDir(dirname);
}

void
CDirTreeWalk::
enter()
{
  dirCount_.push_back(0);

  dirName_ = getDirPath();
}

void
CDirTreeWalk::
leave()
{
  auto dirCount = dirCount_.back();

  dirCount_.pop_back();

  if (dirCount == 0)
    check_->addEmptyDir(dirName_);
}

void
CDirTreeWalk::
process()
{
  ++dirCount_.back();

  const std::string &fileName = getFileName();

  check_->addFile(fileName);
}

//-----------

CDirCheckFile::
CDirCheckFile(const std::string &fileName) :
 CDirFile(fileName)
{
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

  for (const auto &file1 : files) {
    if (file1->is_link_) continue;

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

  auto p = checkSumMap_.find(checksum);

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

  auto p = nameMap_.find(file->getFile().getName());

  if (p == nameMap_.end())
    return files;

  const FileList &files1 = (*p).second;

  for (auto &file1 : files1) {
    if (file1 != file)
      files.push_back(file1);
  }

  return files;
}

bool
CDirDupNameMap::
isDupName(const CDirCheckFile *file) const
{
  const auto p = nameMap_.find(file->getFile().getName());

  if (p == nameMap_.end() || (*p).second.size() < 2) return false;

  return (file == (*p).second.front());
}
