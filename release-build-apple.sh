# VCPKG settings
export VCPKG_ROOT="$HOME/.vcpkg"
export CMAKE_TOOLCHAIN_FILE="$HOME/.vcpkg/scripts/buildsystems/vcpkg.cmake"

# Delete the old build artifact directory
rm -rf build-apple

# Generate build files
cmake -B build-apple -S . \
	-DCMAKE_BUILD_TYPE=Release \
	-DVCPKG_TARGET_TRIPLET=arm64-osx \
	-DVCPKG_LIBRARY_LINKAGE=static \
	-DCMAKE_TOOLCHAIN_FILE=$CMAKE_TOOLCHAIN_FILE

# Compile
cmake --build build-apple --config Release
