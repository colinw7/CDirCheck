#ifndef CDirWalk_H
#define CDirWalk_H

#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>

#include <CFileType.h>

class CDirWalk {
 public:
  explicit CDirWalk(const std::string &dirname=".");

  virtual ~CDirWalk();

  const std::string &getDirName() const { return dirname_; }

  const std::string &getFileName() const { return filename_; }

  std::string getPath() const;

  const std::vector<std::string> &getDirs() const { return dirs_; }

  bool walk();

  virtual void enter() { }
  virtual void leave() { }

  virtual bool checkDir(const std::string &dirname);

  virtual void process() = 0;

 private:
  bool doWalk();

  bool doEnter();
  void doLeave();

 protected:
  std::string              dirname_;
  std::vector<std::string> dirs_;
  std::string              filename_;
  CFileType                type_;
  struct stat              stat_;
};

#endif
