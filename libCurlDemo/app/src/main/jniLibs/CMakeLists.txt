# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.
# -DENCODE_DATABASE
cmake_minimum_required(VERSION 3.4.1)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSQLITE_HAS_CODEC")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DSQLITE_HAS_CODEC")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

set(sdk_path ${PROJECT_SOURCE_DIR}/src/main/cpp/sdk)
set(source_path ${PROJECT_SOURCE_DIR}/src/main/cpp/sdk/source)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/src/main/jniLibs/${ANDROID_ABI})

#file(GLOB_RECURSE src_list "*.cpp")
file(GLOB jni_src_list "src/main/cpp/jni/*.cpp")
file(GLOB http_src_list "${source_path}/HTTP/*.cpp")
file(GLOB tmtp_src_list "${source_path}/TMTP/src/*.cpp")
file(GLOB tnimdb_src_list "${source_path}/TNIMSDK/db/*.cpp")
file(GLOB sqlite_src_list "${source_path}/TNIMSDK/db/sqlite/*.cpp")
file(GLOB imsdk_src_list "${source_path}/TNIMSDK/CHAT/*.cpp")
file(GLOB imsdk_gc_src_list "${source_path}/TNIMSDK/GrouPChat/*.cpp")

 add_library( curl STATIC IMPORTED )
 add_library( ssl STATIC IMPORTED )
 add_library( crypto STATIC IMPORTED )

 set_target_properties( curl
                       PROPERTIES IMPORTED_LOCATION
                       ${sdk_path}/lib/${ANDROID_ABI}/libcurl.a )
 set_target_properties( ssl
                       PROPERTIES IMPORTED_LOCATION
                       ${sdk_path}/lib/${ANDROID_ABI}/libssl.a )
 set_target_properties( crypto
                       PROPERTIES IMPORTED_LOCATION
                        ${sdk_path}/lib/${ANDROID_ABI}/libcrypto.a )


add_library( tmtpSdk SHARED
  ${jni_src_list}
  ${http_src_list}
  ${tmtp_src_list}
  ${tnimdb_src_list}
  ${imsdk_src_list}
  ${sqlite_src_list}
  ${imsdk_gc_src_list}
  ${source_path}/TNIMSDK/db/sqlite/src/sqlite3.c)

target_include_directories(tmtpSdk PRIVATE
${source_path}/3rd/rapidjson/
${source_path}/3rd/OpenSSL/include/
${source_path}/HTTP/curl/
${source_path}/HTTP/
${source_path}/TMTP/include/
${source_path}/TNIMSDK/API/
${source_path}/TNIMSDK/db/
${source_path}/TNIMSDK/db/sqlite/
${source_path}/TNIMSDK/db/sqlite/src/
)

find_library( log-lib log )
find_library( lib-z z )

target_link_libraries( tmtpSdk
                       curl
                       ssl
                       crypto
                        ${log-lib}
                        ${lib-z} )