#include "Voikko.h"
#include "UTF16String.h"

namespace VoikkoNan
{
    using v8::Array;
    using v8::Function;
    using v8::FunctionTemplate;
    using v8::Integer;

    Nan::Persistent<v8::Function> Voikko::constructor;

    void Voikko::Init(Local<Object> exports) {
        Nan::HandleScope scope;

        Local<FunctionTemplate> tpl = Nan::New<v8::FunctionTemplate>(New);
        tpl->SetClassName(Nan::New("Voikko").ToLocalChecked());
        tpl->InstanceTemplate()->SetInternalFieldCount(1);

        Nan::SetPrototypeMethod(tpl, "CheckUTF16", CheckUTF16);
        Nan::SetPrototypeMethod(tpl, "SuggestionsForUTF16", SuggestionsForUTF16);
        Nan::SetPrototypeMethod(tpl, "HyphenateUTF16", HyphenateUTF16);
        Nan::SetPrototypeMethod(tpl, "AnalyseUTF16", AnalyseUTF16);

        // Boolean options
        Nan::SetAccessor(tpl->PrototypeTemplate(),
                         Nan::New("IgnoreDot").ToLocalChecked(),
                         IgnoreDot, IgnoreDot);
        Nan::SetAccessor(tpl->PrototypeTemplate(),
                         Nan::New("IgnoreNumbers").ToLocalChecked(),
                         IgnoreNumbers, IgnoreNumbers);
        Nan::SetAccessor(tpl->PrototypeTemplate(),
                         Nan::New("IgnoreUppercase").ToLocalChecked(),
                         IgnoreUppercase, IgnoreUppercase);
        Nan::SetAccessor(tpl->PrototypeTemplate(),
                         Nan::New("AcceptFirstUppercase").ToLocalChecked(),
                         AcceptFirstUppercase, AcceptFirstUppercase);
        Nan::SetAccessor(tpl->PrototypeTemplate(),
                         Nan::New("AcceptAllUppercase").ToLocalChecked(),
                         AcceptAllUppercase, AcceptAllUppercase);
        Nan::SetAccessor(tpl->PrototypeTemplate(),
                         Nan::New("NoUglyHyphenation").ToLocalChecked(),
                         NoUglyHyphenation, NoUglyHyphenation);
        Nan::SetAccessor(tpl->PrototypeTemplate(),
                         Nan::New("OcrSuggestions").ToLocalChecked(),
                         OcrSuggestions, OcrSuggestions);
        Nan::SetAccessor(tpl->PrototypeTemplate(),
                         Nan::New("IgnoreNonwords").ToLocalChecked(),
                         IgnoreNonwords, IgnoreNonwords);
        Nan::SetAccessor(tpl->PrototypeTemplate(),
                         Nan::New("AcceptExtraHyphens").ToLocalChecked(),
                         AcceptExtraHyphens, AcceptExtraHyphens);
        Nan::SetAccessor(tpl->PrototypeTemplate(),
                         Nan::New("AcceptMissingHyphens").ToLocalChecked(),
                         AcceptMissingHyphens, AcceptMissingHyphens);
        Nan::SetAccessor(tpl->PrototypeTemplate(),
                         Nan::New("AcceptTitlesInGc").ToLocalChecked(),
                         AcceptTitlesInGc, AcceptTitlesInGc);
        Nan::SetAccessor(tpl->PrototypeTemplate(),
                         Nan::New("AcceptUnfinishedParagraphsInGc").ToLocalChecked(),
                         AcceptUnfinishedParagraphsInGc, AcceptUnfinishedParagraphsInGc);
        Nan::SetAccessor(tpl->PrototypeTemplate(),
                         Nan::New("HyphenateUnknownWords").ToLocalChecked(),
                         HyphenateUnknownWords, HyphenateUnknownWords);
        Nan::SetAccessor(tpl->PrototypeTemplate(),
                         Nan::New("AcceptBulletedListsInGc").ToLocalChecked(),
                         AcceptBulletedListsInGc, AcceptBulletedListsInGc);

        // Integer options
        Nan::SetAccessor(tpl->PrototypeTemplate(),
                         Nan::New("MinHyphenatedWordLength").ToLocalChecked(),
                         MinHyphenatedWordLength, MinHyphenatedWordLength);
        Nan::SetAccessor(tpl->PrototypeTemplate(),
                         Nan::New("CacheSize").ToLocalChecked(),
                         CacheSize, CacheSize);

        constructor.Reset(tpl->GetFunction());
        exports->Set(Nan::New("Voikko").ToLocalChecked(), tpl->GetFunction());
    }

    Voikko::Voikko(const char *lang, const char *path)
    {
        booleanValues[VOIKKO_OPT_ACCEPT_FIRST_UPPERCASE] = true;
        booleanValues[VOIKKO_OPT_ACCEPT_ALL_UPPERCASE] = true;
        booleanValues[VOIKKO_OPT_IGNORE_NONWORDS] = true;
        booleanValues[VOIKKO_OPT_HYPHENATE_UNKNOWN_WORDS] = true;
        integerValues[VOIKKO_MIN_HYPHENATED_WORD_LENGTH] = 9;

        const char* error;
        handle = voikkoInit(&error, lang, path);

        if (error) {
            Nan::ThrowError(error);
        }
    }

    Voikko::~Voikko() {
        if (handle) {
            voikkoTerminate(handle);
        }
    }

    void Voikko::New(const FunctionCallbackInfo<Value> &info) {
        if (info.IsConstructCall()) {
            Local<String> langstr = Nan::To<String>(info[0]).ToLocalChecked();
            char* lang_cstr = new char[langstr->Utf8Length() + 1];
            langstr->WriteUtf8(lang_cstr);

            Local<String> pathstr = Nan::To<String>(info[1]).ToLocalChecked();
            char* path_cstr = new char[pathstr->Utf8Length() + 1];
            pathstr->WriteUtf8(path_cstr);

            Voikko *obj = new Voikko(lang_cstr, path_cstr);
            delete[] lang_cstr;
            delete[] path_cstr;

            obj->Wrap(info.This());
            info.GetReturnValue().Set(info.This());
        } else {
            Local<Value> argv[2] = {info[0], info[1]};
            Local<Function> cons = Nan::New<Function>(constructor);
            Local<v8::Context> context = info.GetIsolate()->GetEnteredContext();

            info.GetReturnValue().Set(cons->NewInstance(context, 2, argv).ToLocalChecked());
        }
    }

    /* Un comment if there ever comes a day when voikko supports a language that
     * is outside of the BMP.
     *
    static inline int is_high_surrogate(uint16_t uc) { return (uc & 0xfffffc00) == 0xd800; }
    static inline int is_low_surrogate(uint16_t uc) { return (uc & 0xfffffc00) == 0xdc00; }

    static inline wchar_t surrogate_to_utf32(uint16_t high, uint16_t low) {
        return (high << 10) + low - 0x35fdc00;
    }

    void utf16_to_utf32(const uint16_t *input, wchar_t *output) {
        while (*input != 0) {
            const uint16_t uc = *input++;
            if (uc < 0xD7FF || uc > 0xE000) {
                *output++ = uc;
            } else if (is_high_surrogate(uc) && is_low_surrogate(*input)) {
                *output++ = surrogate_to_utf32(uc, *input++);
            } else {
                *output = 0xFFFD;
            }
        }

        *output = 0;
    }
    */

    /* Voikko doesn't support anything outside BMP and garbage will
     * fail any spell checks just as well as '�' (0xFFFD) would. */
    static inline void utf16_to_utf32(const uint16_t* input, wchar_t* output) {
        size_t i = 0;
        for (; input[i] != '\0'; i++) {
            output[i] = input[i];
        }

        output[i] = '\0';
    }

    static inline wchar_t* v8StringToUTF32(const Local<String>& str) {
        wchar_t* UTF32_buff = new wchar_t[str->Length() + 1];

        if (sizeof(uint16_t) != sizeof(wchar_t)) {
            uint16_t *UTF16_buff = new uint16_t[str->Length() + 1];
            str->Write(UTF16_buff);
            utf16_to_utf32(UTF16_buff, UTF32_buff);
            delete[] UTF16_buff;
        } else {
            str->Write(reinterpret_cast<uint16_t*>(UTF32_buff));
        }

        return UTF32_buff;
    }

    void Voikko::CheckUTF16(const FunctionCallbackInfo<Value> &info) {
        Voikko* obj = ObjectWrap::Unwrap<Voikko>(info.Holder());
        wchar_t* UTF32_buff = v8StringToUTF32(info[0]->ToString());
        int result = voikkoSpellUcs4(obj->handle, UTF32_buff);
        delete[] UTF32_buff;

        info.GetReturnValue().Set(Nan::New((result == VOIKKO_SPELL_OK)));
    }

    void Voikko::SuggestionsForUTF16(const FunctionCallbackInfo<Value> &info) {
        Voikko* obj = ObjectWrap::Unwrap<Voikko>(info.Holder());
        wchar_t* UTF32_buff = v8StringToUTF32(info[0]->ToString());
        wchar_t** results = voikkoSuggestUcs4(obj->handle, UTF32_buff);
        delete[] UTF32_buff;

        Local<Array> suggestions = Nan::New<Array>();
        if (results) {
            for (uint32_t i = 0; results[i] != nullptr; i++) {
                Nan::Set(suggestions, i, (new UTF16String(results[i]))->getLocal(info.GetIsolate()));
            }
            voikko_free_suggest_ucs4(results);
        }

        info.GetReturnValue().Set(suggestions);
    }

    void Voikko::HyphenateUTF16(const FunctionCallbackInfo<Value> &info) {
        Voikko* obj = ObjectWrap::Unwrap<Voikko>(info.Holder());
        wchar_t* UTF32_buff = v8StringToUTF32(info[0]->ToString());
        char* mask = voikkoHyphenateUcs4(obj->handle, UTF32_buff);
        delete[] UTF32_buff;

        if (mask) {
            info.GetReturnValue().Set(Nan::New(mask).ToLocalChecked());
            delete[] mask;
        } else {
            info.GetReturnValue().Set(Nan::EmptyString());
        }
    }

    void Voikko::AnalyseUTF16(const FunctionCallbackInfo<Value> &info) {
        Voikko* obj = ObjectWrap::Unwrap<Voikko>(info.Holder());
        wchar_t* UTF32_buff = v8StringToUTF32(info[0]->ToString());
        voikko_mor_analysis** analyses = voikkoAnalyzeWordUcs4(obj->handle, UTF32_buff);
        delete[] UTF32_buff;

        Local<Array> resultGroups = Nan::New<Array>();
        for (uint32_t i = 0; analyses[i] != nullptr; i++) {
            const char** keys = voikko_mor_analysis_keys(analyses[i]);
            Local<Object> result = Nan::New<Object>();
            Nan::Set(resultGroups, i, result);

            for (size_t key = 0; keys[key] != nullptr; key++) {
                UTF16String* value = new UTF16String(voikko_mor_analysis_value_ucs4(analyses[i], keys[key]));
                printf("%s\n", keys[key]);
                Nan::Set(result, Nan::New(keys[key]).ToLocalChecked(), value->getLocal(info.GetIsolate()));
            }
        }

        voikko_free_mor_analysis(analyses);

        info.GetReturnValue().Set(resultGroups);
    }

    void Voikko::AnalyzeSentance(const FunctionCallbackInfo<Value>& info) {

    }

    inline void Voikko::getBooleanOption(const PropertyCallbackInfo<Value>& info,
                                         const int option) {
        Voikko* obj = ObjectWrap::Unwrap<Voikko>(info.This());
        info.GetReturnValue().Set(Nan::New(obj->booleanValues[option]));
    }

    inline void Voikko::getIntegerOption(const PropertyCallbackInfo<Value>& info,
                                         const int option) {
        Voikko* obj = ObjectWrap::Unwrap<Voikko>(info.This());
        info.GetReturnValue().Set(Nan::New(obj->integerValues[option]));
    }
    
    inline void Voikko::setBooleanOption(const PropertyCallbackInfo<void>& info,
                                         const int option, Local<Value> value) {
        Voikko* obj = ObjectWrap::Unwrap<Voikko>(info.This());
        obj->booleanValues[option] = value->IsTrue();
        voikkoSetBooleanOption(obj->handle, option, value->IsTrue());
    }

    inline void Voikko::setIntegerOption(const PropertyCallbackInfo<void>& info,
                                         const int option, Local<Value> value) {
        Voikko* obj = ObjectWrap::Unwrap<Voikko>(info.This());
        obj->integerValues[option] = Nan::To<Integer>(value).ToLocalChecked()->Value();
        voikkoSetIntegerOption(obj->handle, option, obj->integerValues[option]);
    }

    // Getters
    // Boolean options
    void Voikko::IgnoreDot(Local<String> property, const PropertyCallbackInfo<Value> &info) {
        getBooleanOption(info, VOIKKO_OPT_IGNORE_DOT);
    }

    void Voikko::IgnoreNumbers(Local<String> property, const PropertyCallbackInfo<Value> &info) {
        getBooleanOption(info, VOIKKO_OPT_IGNORE_NUMBERS);
    }

    void Voikko::IgnoreUppercase(Local<String> property, const PropertyCallbackInfo<Value> &info) {
        getBooleanOption(info, VOIKKO_OPT_IGNORE_UPPERCASE);
    }

    void Voikko::AcceptFirstUppercase(Local<String> property, const PropertyCallbackInfo<Value> &info) {
        getBooleanOption(info, VOIKKO_OPT_ACCEPT_FIRST_UPPERCASE);
    }

    void Voikko::AcceptAllUppercase(Local<String> property, const PropertyCallbackInfo<Value> &info) {
        getBooleanOption(info, VOIKKO_OPT_ACCEPT_ALL_UPPERCASE);
    }

    void Voikko::NoUglyHyphenation(Local<String> property, const PropertyCallbackInfo<Value> &info) {
        getBooleanOption(info, VOIKKO_OPT_NO_UGLY_HYPHENATION);
    }

    void Voikko::OcrSuggestions(Local<String> property, const PropertyCallbackInfo<Value> &info) {
        getBooleanOption(info, VOIKKO_OPT_OCR_SUGGESTIONS);
    }

    void Voikko::IgnoreNonwords(Local<String> property, const PropertyCallbackInfo<Value> &info) {
        getBooleanOption(info, VOIKKO_OPT_IGNORE_NONWORDS);
    }

    void Voikko::AcceptExtraHyphens(Local<String> property, const PropertyCallbackInfo<Value> &info) {
        getBooleanOption(info, VOIKKO_OPT_ACCEPT_EXTRA_HYPHENS);
    }

    void Voikko::AcceptMissingHyphens(Local<String> property, const PropertyCallbackInfo<Value> &info) {
        getBooleanOption(info, VOIKKO_OPT_ACCEPT_MISSING_HYPHENS);
    }

    void Voikko::AcceptTitlesInGc(Local<String> property, const PropertyCallbackInfo<Value> &info) {
        getBooleanOption(info, VOIKKO_OPT_ACCEPT_TITLES_IN_GC);
    }

    void Voikko::AcceptUnfinishedParagraphsInGc(Local<String> property, const PropertyCallbackInfo<Value> &info) {
        getBooleanOption(info, VOIKKO_OPT_ACCEPT_UNFINISHED_PARAGRAPHS_IN_GC);
    }

    void Voikko::HyphenateUnknownWords(Local<String> property, const PropertyCallbackInfo<Value> &info) {
        getBooleanOption(info, VOIKKO_OPT_HYPHENATE_UNKNOWN_WORDS);
    }

    void Voikko::AcceptBulletedListsInGc(Local<String> property, const PropertyCallbackInfo<Value> &info) {
        getBooleanOption(info, VOIKKO_OPT_ACCEPT_BULLETED_LISTS_IN_GC);
    }

    // Integer options
    void Voikko::MinHyphenatedWordLength(Local<String> property, const PropertyCallbackInfo<Value> &info) {
        getIntegerOption(info, VOIKKO_MIN_HYPHENATED_WORD_LENGTH);
    }

    void Voikko::CacheSize(Local<String> property, const PropertyCallbackInfo<Value> &info) {
        getIntegerOption(info, VOIKKO_SPELLER_CACHE_SIZE);
    }

    // Setters
    // Boolean options
    void Voikko::IgnoreDot(Local<String> property, Local<Value> value,
                           const PropertyCallbackInfo<void> &info) {
        setBooleanOption(info, VOIKKO_OPT_IGNORE_DOT, value);
    }

    void Voikko::IgnoreNumbers(Local<String> property, Local<Value> value,
                               const PropertyCallbackInfo<void> &info) {
        setBooleanOption(info, VOIKKO_OPT_IGNORE_NUMBERS, value);
    }

    void Voikko::IgnoreUppercase(Local<String> property, Local<Value> value,
                                 const PropertyCallbackInfo<void> &info) {
        setBooleanOption(info, VOIKKO_OPT_IGNORE_UPPERCASE, value);
    }

    void Voikko::AcceptFirstUppercase(Local<String> property, Local<Value> value,
                                      const PropertyCallbackInfo<void> &info) {
        setBooleanOption(info, VOIKKO_OPT_ACCEPT_FIRST_UPPERCASE, value);
    }

    void Voikko::AcceptAllUppercase(Local<String> property, Local<Value> value,
                                    const PropertyCallbackInfo<void> &info) {
        setBooleanOption(info, VOIKKO_OPT_ACCEPT_ALL_UPPERCASE, value);
    }

    void Voikko::NoUglyHyphenation(Local<String> property, Local<Value> value,
                                   const PropertyCallbackInfo<void> &info) {
        setBooleanOption(info, VOIKKO_OPT_NO_UGLY_HYPHENATION, value);
    }

    void Voikko::OcrSuggestions(Local<String> property, Local<Value> value,
                                const PropertyCallbackInfo<void> &info) {
        setBooleanOption(info, VOIKKO_OPT_OCR_SUGGESTIONS, value);
    }

    void Voikko::IgnoreNonwords(Local<String> property, Local<Value> value,
                                const PropertyCallbackInfo<void> &info) {
        setBooleanOption(info, VOIKKO_OPT_IGNORE_NONWORDS, value);
    }

    void Voikko::AcceptExtraHyphens(Local<String> property, Local<Value> value,
                                    const PropertyCallbackInfo<void> &info) {
        setBooleanOption(info, VOIKKO_OPT_ACCEPT_EXTRA_HYPHENS, value);
    }

    void Voikko::AcceptMissingHyphens(Local<String> property, Local<Value> value,
                                      const PropertyCallbackInfo<void> &info) {
        setBooleanOption(info, VOIKKO_OPT_ACCEPT_MISSING_HYPHENS, value);
    }

    void Voikko::AcceptTitlesInGc(Local<String> property, Local<Value> value,
                                  const PropertyCallbackInfo<void> &info) {
        setBooleanOption(info, VOIKKO_OPT_ACCEPT_TITLES_IN_GC, value);
    }

    void Voikko::AcceptUnfinishedParagraphsInGc(Local<String> property, Local<Value> value,
                                                const PropertyCallbackInfo<void> &info) {
        setBooleanOption(info, VOIKKO_OPT_ACCEPT_UNFINISHED_PARAGRAPHS_IN_GC, value);
    }

    void Voikko::HyphenateUnknownWords(Local<String> property, Local<Value> value,
                                       const PropertyCallbackInfo<void> &info) {
        setBooleanOption(info, VOIKKO_OPT_HYPHENATE_UNKNOWN_WORDS, value);
    }

    void Voikko::AcceptBulletedListsInGc(Local<String> property, Local<Value> value,
                                         const PropertyCallbackInfo<void> &info) {
        setBooleanOption(info, VOIKKO_OPT_ACCEPT_BULLETED_LISTS_IN_GC, value);
    }

    // Integer options
    void Voikko::MinHyphenatedWordLength(Local<String> property, Local<Value> value,
                                         const PropertyCallbackInfo<void> &info) {
        setIntegerOption(info, VOIKKO_MIN_HYPHENATED_WORD_LENGTH, value);
    }

    void Voikko::CacheSize(Local<String> property, Local<Value> value,
                           const PropertyCallbackInfo<void> &info) {
        setIntegerOption(info, VOIKKO_SPELLER_CACHE_SIZE, value);
    }
}