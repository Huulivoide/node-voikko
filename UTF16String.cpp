#include "UTF16String.h"

#include <cwchar>
#include <cstring>

namespace VoikkoNan
{
    UTF16String::UTF16String(const wchar_t *str) : String::ExternalStringResource() {
        _length = wcslen(str);
        _data = new uint16_t[_length + 1];

        if (sizeof(uint16_t) != sizeof(wchar_t)) {
            size_t i = 0;
            for (; i < _length; i++) {
                _data[i] = str[i];
            }
            _data[i] = '\0';
        } else {
            memcpy(_data, str, sizeof(wchar_t) * (_length + 1));
        }
    }

    UTF16String::UTF16String(uint16_t *str) : String::ExternalStringResource(), _data(str) {
        _length = 0;
        for (; str[_length] != 0; _length++);
    }

    UTF16String::UTF16String(uint16_t *str, const size_t len)
            : String::ExternalStringResource(), _data(str), _length(len) {}

    UTF16String::~UTF16String() {
        delete[] _data;
    }

    const uint16_t* UTF16String::data() const { return _data; }

    size_t UTF16String::length() const { return _length; }
}
