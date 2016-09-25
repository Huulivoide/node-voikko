#include <libvoikko/voikko.h>

#include <cstring>

#include "Dictionary.h"
#include "Voikko.h"

namespace VoikkoNan {
    using v8::Array;
    using v8::FunctionTemplate;

    Nan::Persistent<v8::Function> Dictionary::constructor;

    void Dictionary::Init(Local<Object> exports) {
        Local<FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
        tpl->SetClassName(Nan::New("Dictionary").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        Nan::SetMethod(tpl, "FindDictionaries", FindDictionaries);

        Nan::SetAccessor(tpl->PrototypeTemplate(), Nan::New("language").ToLocalChecked(), Language);
        Nan::SetAccessor(tpl->PrototypeTemplate(), Nan::New("variant").ToLocalChecked(), Variant);
        Nan::SetAccessor(tpl->PrototypeTemplate(), Nan::New("script").ToLocalChecked(), Script);
        Nan::SetAccessor(tpl->PrototypeTemplate(), Nan::New("description").ToLocalChecked(), Description);
        Nan::SetAccessor(tpl->PrototypeTemplate(), Nan::New("path").ToLocalChecked(), Path);

        Nan::SetAccessor(tpl->PrototypeTemplate(), Nan::New("isSpellChecker").ToLocalChecked(), IsSpellChecker);
        Nan::SetAccessor(tpl->PrototypeTemplate(), Nan::New("isHyphenator").ToLocalChecked(), IsHyphenator);
        Nan::SetAccessor(tpl->PrototypeTemplate(), Nan::New("isGrammarChecker").ToLocalChecked(), IsGrammarChecker);

        constructor.Reset(tpl->GetFunction());
        exports->Set(Nan::New("Dictionary").ToLocalChecked(), tpl->GetFunction());
    }

    Dictionary::Dictionary(
            const Local<String> language, const Local<String> variant,
            const Local<String> script, const Local<String> description,
            const Local<String> path, const Local<Boolean> speller,
            const Local<Boolean> hyphenator, const Local<Boolean> gcor)
            : mLanguage(language), mVariant(variant), mScript(script),
              mDescription(description), mPath(path), mIsSpellChecker(speller),
              mIsHyphenator(hyphenator), mIsGrammarChecker(gcor) {}

    Dictionary::~Dictionary() {
        mLanguage.Reset();
        mVariant.Reset();
        mScript.Reset();
        mDescription.Reset();
        mPath.Reset();
    }

    void Dictionary::New(const FunctionCallbackInfo<Value> &info) {

        if (info.IsConstructCall()) {
            Dictionary* dict = new Dictionary(
                    Nan::To<String>(info[0]).ToLocalChecked(),
                    Nan::To<String>(info[1]).ToLocalChecked(),
                    Nan::To<String>(info[2]).ToLocalChecked(),
                    Nan::To<String>(info[3]).ToLocalChecked(),
                    Nan::To<String>(info[4]).ToLocalChecked(),
                    Nan::To<Boolean>(info[5]).ToLocalChecked(),
                    Nan::To<Boolean>(info[6]).ToLocalChecked(),
                    Nan::To<Boolean>(info[7]).ToLocalChecked()
            );

            dict->Wrap(info.This());
            info.GetReturnValue().Set(info.This());
        } else {
            Local<v8::Context> context = info.GetIsolate()->GetCurrentContext();
            Local<Function> cons = Nan::New<Function>(constructor);
            Local<Value> argv[5] = {info[0], info[1], info[2], info[3], info[4]};

            info.GetReturnValue().Set(cons->NewInstance(context, 5, argv).ToLocalChecked());
        }
    }

    static bool listContains(char const* const* list, const char* str) {
        for (size_t i = 0; list[i] != nullptr; i++) {
            if (strcmp(list[i], str) == 0) {
                return true;
            }
        }

        return false;
    }

    void Dictionary::FindDictionaries(const FunctionCallbackInfo<Value> &info) {
        Local<String> path = Nan::EmptyString();

        if ((info[0]->IsString() || info[0]->IsStringObject())) {
            path = Nan::To<String>(info[0]).ToLocalChecked();
        }

        char* path_cstr = new char[path->Utf8Length() + 1];
        path->WriteUtf8(path_cstr);
        voikko_dict** voikkoDicts = voikko_list_dicts(path_cstr);

        char** spellers = voikkoListSupportedSpellingLanguages(path_cstr);
        char** hyphenators = voikkoListSupportedHyphenationLanguages(path_cstr);
        char** grammarCheckers = voikkoListSupportedGrammarCheckingLanguages(path_cstr);

        delete[] path_cstr;

        Local<Array> dicts = Nan::New<Array>();
        for (uint32_t i = 0; voikkoDicts[i] != nullptr; i++) {
            const char* langTag = voikko_dict_language(voikkoDicts[i]);

            Local<Array> argv = Nan::New<Array>();
            Nan::Set(argv, 0, Nan::New(langTag).ToLocalChecked());
            Nan::Set(argv, 1, Nan::New(voikko_dict_variant(voikkoDicts[i])).ToLocalChecked());
            Nan::Set(argv, 2, Nan::New(voikko_dict_script(voikkoDicts[i])).ToLocalChecked());
            Nan::Set(argv, 3, Nan::New(voikko_dict_description(voikkoDicts[i])).ToLocalChecked());
            Nan::Set(argv, 4, path);
            Nan::Set(argv, 5, Nan::New(listContains(spellers, langTag)));
            Nan::Set(argv, 6, Nan::New(listContains(hyphenators, langTag)));
            Nan::Set(argv, 7, Nan::New(listContains(grammarCheckers, langTag)));

            Nan::Set(dicts, i, argv);
        }

        voikko_free_dicts(voikkoDicts);
        voikkoFreeCstrArray(spellers);
        voikkoFreeCstrArray(hyphenators);
        voikkoFreeCstrArray(grammarCheckers);

        info.GetReturnValue().Set(dicts);
    }

    void Dictionary::Language(Local<String> property, const PropertyCallbackInfo<Value> &info) {
        Dictionary* dict = ObjectWrap::Unwrap<Dictionary>(info.This());
        info.GetReturnValue().Set(dict->mLanguage.Get(info.GetIsolate()));
    }

    void Dictionary::Variant(Local<String> property, const PropertyCallbackInfo<Value> &info) {
        Dictionary* dict = ObjectWrap::Unwrap<Dictionary>(info.This());
        info.GetReturnValue().Set(dict->mVariant.Get(info.GetIsolate()));
    }

    void Dictionary::Script(Local<String> property, const PropertyCallbackInfo<Value> &info) {
        Dictionary* dict = ObjectWrap::Unwrap<Dictionary>(info.This());
        info.GetReturnValue().Set(dict->mScript.Get(info.GetIsolate()));
    }

    void Dictionary::Description(Local<String> property, const PropertyCallbackInfo<Value> &info) {
        Dictionary* dict = ObjectWrap::Unwrap<Dictionary>(info.This());
        info.GetReturnValue().Set(dict->mDescription.Get(info.GetIsolate()));
    }

    void Dictionary::Path(Local<String> property, const PropertyCallbackInfo<Value> &info) {
        Dictionary* dict = ObjectWrap::Unwrap<Dictionary>(info.This());
        info.GetReturnValue().Set(dict->mPath.Get(info.GetIsolate()));
    }

    void Dictionary::IsSpellChecker(Local<String> property, const PropertyCallbackInfo<Value> &info) {
        Dictionary* dict = ObjectWrap::Unwrap<Dictionary>(info.This());
        info.GetReturnValue().Set(dict->mIsSpellChecker.Get(info.GetIsolate()));
    }

    void Dictionary::IsHyphenator(Local<String> property, const PropertyCallbackInfo<Value> &info) {
        Dictionary* dict = ObjectWrap::Unwrap<Dictionary>(info.This());
        info.GetReturnValue().Set(dict->mIsHyphenator.Get(info.GetIsolate()));
    }

    void Dictionary::IsGrammarChecker(Local<String> property, const PropertyCallbackInfo<Value> &info) {
        Dictionary* dict = ObjectWrap::Unwrap<Dictionary>(info.This());
        info.GetReturnValue().Set(dict->mIsGrammarChecker.Get(info.GetIsolate()));
    }
};
