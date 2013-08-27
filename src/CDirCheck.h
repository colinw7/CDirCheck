#ifndef CDIR_CHECK_H
#define CDIR_CHECK_H

#include <CFile.h>
#include <CDirFile.h>
#include <CFileCheckSum.h>

#include <map>

class CRegExp;
class CDirCheckFile;

#define CDirCheckFileMapInst CDirCheckFileMap::getInstance()

class CDirCheckFileMap {
 public:
  typedef std::list<CDirCheckFile *> FileList;

  typedef std::map<uint         ,FileList> LengthMap;
  typedef std::map<CFileCheckSum,FileList> CheckSumMap;

 private:
  CDirCheckFileMap() { }

 public:
  static CDirCheckFileMap *getInstance();

  void addFile(CDirCheckFile *file);

  const FileList &getCheckSumFiles(const CFileCheckSum &checksum) const;

 private:
  LengthMap   lengthMap_;
  CheckSumMap checkSumMap_;
};

#define CDirDupNameMapInst CDirDupNameMap::getInstance()

class CDirDupNameMap {
 public:
  typedef std::list<CDirCheckFile *> FileList;

  typedef std::map<std::string,FileList> NameMap;

 private:
  CDirDupNameMap() { }

 public:
  static CDirDupNameMap *getInstance();

  void addFile(CDirCheckFile *file);

  CDirFile::FileList getDupNames(CDirCheckFile *file) const;

  bool isDupName(const CDirCheckFile *file) const;

 private:
  NameMap nameMap_;
};

class CDirCheckFile : public CDirFile {
 public:
  CDirCheckFile(const std::string &filename);

  void setFail(uint flag);

  uint getFail() const { return fail_; }

  bool isDuplicate(bool *seen) const;

  bool isDupName() const;

  const FileList &getDuplicateFiles() const { return duplicateFiles_; }

  void close();

  //----

 private:
  uint     fail_;
  FileList duplicateFiles_;
  FileList dupNameFiles_;
};

class CDirCheck {
 public:
  typedef std::list<CDirCheckFile *> FileList;

  enum TestType {
    BAD_LINK    = (1<<0),
    BAD_NAME    = (1<<1),
    NO_EXTENSION= (1<<2),
    ZERO_LENGTH = (1<<3),
    DUPLICATE   = (1<<4),
    DUP_NAME    = (1<<5),
    BIGGER      = (1<<6),
    SMALLER     = (1<<7)
  };

  struct TestSet {
    bool bad_link;
    bool bad_name;
    bool no_extension;
    bool zero_length;
    bool duplicate;
    bool dup_name;
    bool bigger;
    uint bigger_value;
    bool smaller;
    uint smaller_value;
    // large/small larger/smaller
    // old/new older/newer

    TestSet() {
      bad_link      = false;
      bad_name      = false;
      no_extension  = false;
      zero_length   = false;
      duplicate     = false;
      dup_name      = false;
      bigger        = false;
      bigger_value  = 0;
      smaller       = false;
      smaller_value = 0;
    }
  };

 public:
  CDirCheck(const std::string &dirName);

  const std::string &getDirName() const { return dirName_; }

  void addTest(TestType type);

  void setBigger (uint bigger);
  void setSmaller(uint smaller);

  void setMatchDir (const std::string &pattern);
  void setMatchFile(const std::string &pattern);

  void setIgnoreDir (const std::string &pattern);
  void setIgnoreFile(const std::string &pattern);

  void exec();

  void addFile(const std::string &fileName);

  void process();

  bool checkDir(const std::string &dirname);

 private:
  std::string simplifyPath(const std::string &path) const;

 private:
  std::string  dirName_;
  std::string  dirPath_;
  FileList     fileList_;
  TestSet      tests;
  CRegExp     *matchDir_;
  CRegExp     *matchFile_;
  CRegExp     *ignoreDir_;
  CRegExp     *ignoreFile_;
};

#endif