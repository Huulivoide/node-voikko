'use strict';

let native = require('bindings')('voikko');

function ensureString(param) {
    if (typeof param !== 'string') {
        throw new TypeError(`Expected a string parameter, but 1st parameter is of type ${typeof param}.`);
    }
}

class Voikko {
    constructor(language, path) {
        if (language instanceof native.Dictionary) {
            this.native = new native.Voikko(language.language, language.path);
        } else {
            this.native = new native.Voikko(language, path);
        }
    }

    static listDictionaries(paths) {
        paths = paths ? paths : [''];

        if (typeof paths === 'string') {
            paths = [paths];
        } else if (!Array.isArray(paths)) {
            throw new TypeError(
                `Expected a string or an array of strings as parameter, but 1st parameter is of type ${typeof paths}.`
            );
        }

        return constructDictionaries([].concat(...initDictionarySearches(paths)));
    }

    static nativeInterfaces() { return native; }

    check(word) {
        ensureString(word);
        return this.native.CheckUTF16(word);
    }

    suggest(word) {
        ensureString(word);
        return this.native.SuggestionsForUTF16(word);
    }

    hyphenate(word) {
        ensureString(word);
        return this.native.HyphenateUTF16(word);
    }

    analyze(word) {
        ensureString(word);
        return this.native.AnalyseUTF16(word);
    }
}

function constructDictionaries(paramLists) {
    let dictionaries = new Map();
    for (let params of paramLists) {
        if (!dictionaries.has(params[0])) {
            dictionaries.set(params[0], new native.Dictionary(...params));
        }
    }

    return Array.from(dictionaries.values());
}

function initDictionarySearches(paths) {
    let paramLists = [];

    for (let path of paths) {
        if (typeof path !== 'string') {
            throw new TypeError(`Expected an array of strings but the array contained a non-string object ${path}.`);
        }

        paramLists.push(native.Dictionary.FindDictionaries(path));
    }

    return paramLists;
}

module.exports = Voikko;
