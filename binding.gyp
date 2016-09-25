{
  "targets": [
    {
      "target_name": "voikko",
      "sources": ["addon.cpp", "Voikko.cpp", "UTF16String.cpp", "Dictionary.cpp"],
      "include_dirs": [
        "<!(node -e \"require('nan')\")"
      ],
      "link_settings": {
        "libraries": ["-lvoikko"]
      }
    }
  ]
}
