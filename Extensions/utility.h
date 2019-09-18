#pragma once
#include <codecvt>
#include <string>

namespace Utility {
    namespace string_helpers {
        std::string wstring_to_string(const std::wstring& val)
        {
            std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> cv;

            return cv.to_bytes(val);
        };
}
} // namespace Utility
