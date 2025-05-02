## Modules
A feature introduced in C++20, aims to bring an alternative to using header files

## Clang Examples
`clang++ -std=c++23  module/werelib.cppm --precompile -o ./tests/werelib.pcm`
`clang++ -std=c++23 -fprebuilt-module-path=tests tests/test.cpp -o test.exe`

Old Example that sometimes worked: `clang++ -std=c++23 ./tests/test.cpp -fprebuilt-module-path=./tests/werelib.pcm`

from side ./tests folder
`clang++ -std=c++23 ./test.cpp -fprebuilt-module-path=./werelib.pcm`