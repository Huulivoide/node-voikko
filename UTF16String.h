#ifndef NODE_VOIKKO_UTF16STRING_H
#define NODE_VOIKKO_UTF16STRING_H

#include <v8.h>

namespace VoikkoNan
{
    using v8::Isolate;
    using v8::Local;
    using v8::String;

    class UTF16String : public String::ExternalStringResource
    {
    public:
        explicit UTF16String(const wchar_t* str);
        explicit UTF16String(uint16_t* str);
        explicit UTF16String(uint16_t* str, const size_t len);

        virtual ~UTF16String();

        virtual const uint16_t* data() const;
        virtual size_t length() const;

        inline Local<String> getLocal(Isolate* isolate) {
            return String::NewExternalTwoByte(isolate, this).ToLocalChecked();
        }

    private:
        uint16_t* _data;
        size_t _length;
    };
}
#endif //NODE_VOIKKO_UTF16STRING_H
