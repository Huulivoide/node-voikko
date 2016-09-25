#include <nan.h>

#include "Voikko.h"
#include "Dictionary.h"

void InitAll(v8::Local<v8::Object> exports) {
    VoikkoNan::Voikko::Init(exports);
    VoikkoNan::Dictionary::Init(exports);
}

NODE_MODULE(voikko, InitAll)
