#pragma once
#include "UtilityHeaders.h"
#include "utility.h"
#include <iostream>

class ILogger {
public:
  ILogger() {}
  virtual bool Log(const std::string &val) = 0;

  virtual bool Log(const std::wstring &val) final {
    return Log(Utility::string_helpers::wstring_to_string(val));
  };
  virtual ILogger &operator<<(const std::string &val) = 0;
  virtual ILogger &operator<<(const std::wstring &val) final {
    *this << Utility::string_helpers::wstring_to_string(val);
    return *this;
  };

protected:
  virtual ~ILogger() noexcept {};
};
