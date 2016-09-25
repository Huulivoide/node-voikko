#ifndef VOIKKO_NAN_H
#define VOIKKO_NAN_H

#include <map>
#include <nan.h>
#include <v8.h>
#include <libvoikko/voikko.h>

namespace VoikkoNan
{
    using Nan::FunctionCallbackInfo;
    using Nan::ObjectWrap;
    using Nan::PropertyCallbackInfo;
    using v8::Local;
    using v8::Object;
    using v8::String;
    using v8::Value;

    class Voikko : public ObjectWrap {
    public:
        static Nan::Persistent<v8::Function> constructor;
        static void Init(Local<Object> exports);

    private:
        VoikkoHandle* handle;
        std::map<int, bool> booleanValues;
        std::map<int, int> integerValues;

        Voikko(const char* lang, const char* path = nullptr);
        ~Voikko();

        static void New(const FunctionCallbackInfo<Value> &info);

        static void CheckUTF16(const FunctionCallbackInfo<Value>& info);
        static void SuggestionsForUTF16(const FunctionCallbackInfo<Value>& info);
        static void HyphenateUTF16(const FunctionCallbackInfo<Value>& info);
        static void AnalyseUTF16(const FunctionCallbackInfo<Value>& info);
        static void AnalyzeSentance(const FunctionCallbackInfo<Value>& info);

        static void getBooleanOption(const PropertyCallbackInfo<Value>& info, const int option);
        static void getIntegerOption(const PropertyCallbackInfo<Value>& info, const int option);

        static void setBooleanOption(const PropertyCallbackInfo<void>& info, const int option, Local<Value> value);
        static void setIntegerOption(const PropertyCallbackInfo<void>& info, const int option, Local<Value> value);
        
        // Getters
        // Boolean options
        static void IgnoreDot(Local<String> property, const PropertyCallbackInfo<Value>& info);
        static void IgnoreNumbers(Local<String> property, const PropertyCallbackInfo<Value>& info);
        static void IgnoreUppercase(Local<String> property, const PropertyCallbackInfo<Value>& info);
        static void AcceptFirstUppercase(Local<String> property, const PropertyCallbackInfo<Value>& info);
        static void AcceptAllUppercase(Local<String> property, const PropertyCallbackInfo<Value>& info);
        static void NoUglyHyphenation(Local<String> property, const PropertyCallbackInfo<Value>& info);
        static void OcrSuggestions(Local<String> property, const PropertyCallbackInfo<Value>& info);
        static void IgnoreNonwords(Local<String> property, const PropertyCallbackInfo<Value>& info);
        static void AcceptExtraHyphens(Local<String> property, const PropertyCallbackInfo<Value>& info);
        static void AcceptMissingHyphens(Local<String> property, const PropertyCallbackInfo<Value>& info);
        static void AcceptTitlesInGc(Local<String> property, const PropertyCallbackInfo<Value>& info);
        static void AcceptUnfinishedParagraphsInGc(Local<String> property, const PropertyCallbackInfo<Value>& info);
        static void HyphenateUnknownWords(Local<String> property, const PropertyCallbackInfo<Value>& info);
        static void AcceptBulletedListsInGc(Local<String> property, const PropertyCallbackInfo<Value>& info);

        // Integer options
        static void MinHyphenatedWordLength(Local<String> property, const PropertyCallbackInfo<Value>& info);
        static void CacheSize(Local<String> property, const PropertyCallbackInfo<Value>& info);
        
        // Setters
        // Boolean options
        static void IgnoreDot(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void>& info);
        static void IgnoreNumbers(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void>& info);
        static void IgnoreUppercase(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void>& info);
        static void AcceptFirstUppercase(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void>& info);
        static void AcceptAllUppercase(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void>& info);
        static void NoUglyHyphenation(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void>& info);
        static void OcrSuggestions(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void>& info);
        static void IgnoreNonwords(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void>& info);
        static void AcceptExtraHyphens(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void>& info);
        static void AcceptMissingHyphens(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void>& info);
        static void AcceptTitlesInGc(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void>& info);
        static void AcceptUnfinishedParagraphsInGc(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void>& info);
        static void HyphenateUnknownWords(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void>& info);
        static void AcceptBulletedListsInGc(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void>& info);

        // Integer options
        static void MinHyphenatedWordLength(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void>& info);
        static void CacheSize(Local<String> property, Local<Value> value, const PropertyCallbackInfo<void>& info);
    };
}

#endif //VOIKKO_NAN_H
