# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)


set(jniLib_path ${PROJECT_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI})
set(cpp_path ${PROJECT_SOURCE_DIR}/src/main/cpp)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/libs/${ANDROID_ABI})

file(GLOB jni_common_list "src/main/cpp/jni/*.cpp")
file(GLOB src_list "src/main/cpp/*.cpp")
add_library( networkLib
             SHARED
             ${src_list}
             ${jni_common_list})


add_library( curl STATIC IMPORTED )
add_library( ssl STATIC IMPORTED )
add_library( crypto STATIC IMPORTED )

set_target_properties( curl
                       PROPERTIES IMPORTED_LOCATION
                       ${jniLib_path}/libcurl.a )
set_target_properties( ssl
                       PROPERTIES IMPORTED_LOCATION
                       ${jniLib_path}/libssl.a )
set_target_properties( crypto
                       PROPERTIES IMPORTED_LOCATION
                        ${jniLib_path}/libcrypto.a )


target_include_directories(networkLib PRIVATE
${cpp_path}/curl/
)

find_library( log-lib log )
find_library( lib-z z )

target_link_libraries( networkLib
                        curl
                        ssl
                        crypto
                       ${log-lib}
                        ${lib-z})