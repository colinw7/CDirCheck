#ifndef CDirScan_H
#define CDirScan_H

#include <string>

#include <CDirScope.h>

class CDirScan {
 public:
  explicit CDirScan(const std::string &dirname=".");

 ~CDirScan();

  bool next(std::string &filename);

  const std::string &getErrorMsg() const { return error_msg_; }

 private:
  bool init();
  bool term();

 private:
  std::string dirname_;
  CDirScope   dir_scope_;
  bool        inited_;
  void *      handle_;
  std::string error_msg_;
};

#endif
