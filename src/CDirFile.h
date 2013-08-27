#ifndef CDIR_FILE_H
#define CDIR_FILE_H

#include <CFile.h>
#include <CFileCheckSum.h>

class CDirFile {
 public:
  typedef std::list<CDirFile *> FileList;

 public:
  CDirFile(const std::string &filename);

  CFile &getFile() { return file_; }

  const CFile &getFile() const { return file_; }

  uint getIDev() const { return idev_; }

  uint getINode() const { return inode_; }

  uint getLen() const { return len_; }

  int getTime() const { return time_; }

  bool isDir() const { return is_dir_; }

  void setIsDir() { is_dir_ = true; }

  bool isLink() const { return is_link_; }

  bool isLinkExists() const { return link_exists_; }

  std::string getLinkName() const;

  const CFileCheckSum &getCheckSum() const { return checksum_; }

  const FileList &getChildren() const { return children_; }

  void addChild(CDirFile *file);

  static bool same(const CDirFile *src, const CDirFile *dst);

  static bool equal(const CDirFile *src, const CDirFile *dst);

  static bool dataEqual(const CDirFile *src, const CDirFile *dst);

  void print() const;

 protected:
  CFile         file_;
  uint          idev_;
  uint          inode_;
  uint          len_;
  int           time_;
  bool          is_dir_;
  bool          is_link_;
  bool          link_exists_;
  CFileCheckSum checksum_;
  FileList      children_;
};

#endif
