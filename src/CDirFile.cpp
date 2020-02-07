#include <CDirFile.h>
#include <iostream>

CDirFile::
CDirFile(const std::string &fileName) :
 file_(fileName), is_dir_(false), link_exists_(true)
{
  CFileBase::setUseLStat(true);

  is_link_ = file_.isLink();

  if (is_link_) {
    std::string linkName;

    file_.getLinkName(linkName);

    link_exists_ = CFile::exists(linkName);
  }

  if (! is_link_) {
    file_.open(CFileBase::Mode::READ);

    idev_  = file_.getIDev();
    inode_ = file_.getINode();
    len_   = file_.getSize();
    time_  = file_.getMTime();

    size_t num_read;
    uchar  buffer[32];

    file_.read(buffer, 32, &num_read);

    checksum_.calc(buffer, num_read);

    file_.close();
  }
}

std::string
CDirFile::
getLinkName() const
{
  std::string linkName;

  if (is_link_)
    file_.getLinkName(linkName);

  return linkName;
}

void
CDirFile::
addChild(CDirFile *child)
{
  children_.push_back(child);
}

bool
CDirFile::
same(const CDirFile *src, const CDirFile *dst)
{
  return ((src->getIDev() == dst->getIDev()) && (src->getINode() == dst->getINode()));
}

bool
CDirFile::
equal(const CDirFile *src, const CDirFile *dst)
{
  if (src->getLen() != dst->getLen()) return false;

  if (CFileCheckSum::diff(src->getCheckSum(), dst->getCheckSum()) != 0) return false;

  return dataEqual(src, dst);
}

bool
CDirFile::
dataEqual(const CDirFile *src, const CDirFile *dst)
{
  bool equal = true;

  uint len = src->getLen();

  for (uint i = 0; i < len; ++i) {
    char c1 = const_cast<CDirFile *>(src)->getFile().getC();
    char c2 = const_cast<CDirFile *>(dst)->getFile().getC();

    if (c1 != c2) {
      equal = false;
      break;
    }
  }

  const_cast<CDirFile *>(src)->getFile().close();

  return equal;
}

void
CDirFile::
print() const
{
  std::cout << file_.getPath() << std::endl;

  FileList::const_iterator p1, p2;

  for (p1 = children_.begin(), p2 = children_.end(); p1 != p2; ++p1)
    (*p1)->print();
}
