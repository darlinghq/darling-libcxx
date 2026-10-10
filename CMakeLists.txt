project(libcxx)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -nostdinc -std=c++11 -fvisibility=hidden -fvisibility-inlines-hidden")

include_directories(
	BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/include
)

add_definitions(-D_LIBCPP_BUILDING_LIBRARY=1 -DLIBCXX_BUILDING_LIBCXXABI=1)

set(cxx_sources
	src/algorithm.cpp
	src/any.cpp
	src/atomic.cpp
	src/barrier.cpp
	src/bind.cpp
	src/charconv.cpp
	src/chrono.cpp
	src/condition_variable.cpp
	src/condition_variable_destructor.cpp
	src/debug.cpp
	src/exception.cpp
	src/format.cpp
	src/functional.cpp
	src/future.cpp
	src/hash.cpp
	src/ios.cpp
	src/ios.instantiations.cpp
	src/iostream.cpp
	src/locale.cpp
	src/memory.cpp
	src/mutex.cpp
	src/mutex_destructor.cpp
	src/new.cpp
	src/optional.cpp
	src/random.cpp
	src/random_shuffle.cpp
	src/regex.cpp
	src/shared_mutex.cpp
	src/stdexcept.cpp
	src/string.cpp
	src/strstream.cpp
	src/system_error.cpp
	src/thread.cpp
	src/typeinfo.cpp
	src/utility.cpp
	src/valarray.cpp
	src/variant.cpp
	src/vector.cpp
	src/filesystem/directory_iterator.cpp
	src/filesystem/operations.cpp
)

# std::filesystem needs C++17; macOS has shipped it in libc++.1.dylib since 10.15
set_source_files_properties(
	src/filesystem/directory_iterator.cpp
	src/filesystem/operations.cpp
	PROPERTIES COMPILE_OPTIONS "-std=c++17"
)

add_darling_object_library(cxx_obj ${cxx_sources})
make_fat(cxx_obj)

set(DYLIB_INSTALL_NAME "/usr/lib/libc++.1.dylib")
set(DYLIB_COMPAT_VERSION "1.0.0")
set(DYLIB_CURRENT_VERSION "1.0.0")

add_darling_library(cxx SHARED $<TARGET_OBJECTS:cxx_obj>)
make_fat(cxx)
target_link_libraries(cxx PRIVATE system cxxabi)
set_target_properties(cxx PROPERTIES OUTPUT_NAME "c++.1")
set_property(TARGET cxx APPEND_STRING PROPERTY LINK_FLAGS " \
      -Wl,-reexport_library,${CMAKE_BINARY_DIR}/src/external/libcxxabi/libc++abi.dylib \
      -Wl,-unexported_symbols_list,${CMAKE_CURRENT_SOURCE_DIR}/lib/libc++unexp.exp \
      -Wl,-force_symbols_not_weak_list,${CMAKE_CURRENT_SOURCE_DIR}/lib/notweak.exp \
      -Wl,-force_symbols_weak_list,${CMAKE_CURRENT_SOURCE_DIR}/lib/weak.exp ")

install(TARGETS cxx DESTINATION libexec/darling/usr/lib)
InstallSymlink("libc++.1.dylib" "${CMAKE_INSTALL_PREFIX}/libexec/darling/usr/lib/libc++.dylib")

# For dyld only
add_darling_static_library(cxx_static SOURCES ${cxx_sources})
make_fat(cxx_static)

