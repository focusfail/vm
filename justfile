default: run

build:
    cmake -B build -S . -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Debug
    ninja -C build

release:
    cmake -B build -S . -G Ninja -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Release
    ninja -C build

run: build
    build/vm
