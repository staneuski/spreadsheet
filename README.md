# spreadsheet
A spreadsheet engine.

## Compilation
Pull [ANTLR](https://www.antlr.org/index.html) sources:
```sh
curl https://www.antlr.org/download/antlr-4.9.2-complete.jar --output src/antlr-4.9.2-complete.jar
curl https://www.antlr.org/download/antlr4-cpp-runtime-4.9.2-source.zip -o src/antlr4_runtime.zip
unzip src/antlr4_runtime.zip -d src/antlr4_runtime && rm src/antlr4_runtime.zip
```