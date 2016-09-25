#ifndef NODE_VOIKKO_DICTIONARY_H
#define NODE_VOIKKO_DICTIONARY_H

#include <nan.h>
#include <v8.h>

namespace VoikkoNan {
    using Nan::FunctionCallbackInfo;
    using Nan::ObjectWrap;
    using Nan::PropertyCallbackInfo;
    using v8::Boolean;
    using v8::Function;
    using v8::Local;
    using v8::Object;
    using v8::String;
    using v8::Value;

    class Dictionary : public ObjectWrap
    {
    public:
        static void Init(Local<Object> exports);

    private:
        Nan::Persistent<String> mLanguage;
        Nan::Persistent<String> mVariant;
        Nan::Persistent<String> mScript;
        Nan::Persistent<String> mDescription;
        Nan::Persistent<String> mPath;
        Nan::Persistent<Boolean> mIsSpellChecker;
        Nan::Persistent<Boolean> mIsHyphenator;
        Nan::Persistent<Boolean> mIsGrammarChecker;

        static Nan::Persistent<Function> constructor;

        Dictionary(const Local<String> language, const Local<String> variant,
                   const Local<String> script, const Local<String> description,
                   const Local<String> path, const Local<Boolean> speller,
                   const Local<Boolean> hyphenator, const Local<Boolean> gcor);

        ~Dictionary();

        static void New(const FunctionCallbackInfo<Value> &info);

        static void FindDictionaries(const FunctionCallbackInfo<Value> &info);

        static void Language(Local<String> property, const PropertyCallbackInfo<Value> &info);
        static void Variant(Local<String> property, const PropertyCallbackInfo<Value> &info);
        static void Script(Local<String> property, const PropertyCallbackInfo<Value> &info);
        static void Description(Local<String> property, const PropertyCallbackInfo<Value> &info);
        static void Path(Local<String> property, const PropertyCallbackInfo<Value> &info);

        static void IsSpellChecker(Local<String> property, const PropertyCallbackInfo<Value> &info);
        static void IsHyphenator(Local<String> property, const PropertyCallbackInfo<Value> &info);
        static void IsGrammarChecker(Local<String> property, const PropertyCallbackInfo<Value> &info);
    };
};

#endif //NODE_VOIKKO_DICTIONARY_H
