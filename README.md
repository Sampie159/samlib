# Default Definitions
This is a personal C library containing some default goodness.

## Usage with CMake
You can simply add the following to your CMakeLists.txt:
```cmake
include(FetchContent)
FetchContent_Declare(
    defs
    GIT_REPOSITORY "https://github.com/Sampie159/defs.git"
    GIT_TAG "master"
    GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(defs)

target_link_libraries(
    YOUR_TARGET_HERE
    ...
    defs
    ...
)
```
