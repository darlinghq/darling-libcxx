project(libcxx)

# DARLING: libcxx 19.1.7, the libc++ generation of macOS 15/26. The sources
# are C++23 (as upstream builds them) and use libcxx/src for their private
# headers ("include/ryu/ryu.h", "include/atomic_support.h").
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -nostdinc -std=c++23 -fvisibility=hidden -fvisibility-inlines-hidden")

include_directories(
	BEFORE ${CMAKE_CURRENT_SOURCE_DIR}/include
	${CMAKE_CURRENT_SOURCE_DIR}/src
)

# The source list and definitions follow libcxx/src/CMakeLists.txt for an Apple build
# (cmake/caches/Apple.cmake): threads, random_device, localization and filesystem on,
# libc++abi as the ABI library and the provider of operator new/delete, no time zone database.
add_definitions(-D_LIBCPP_BUILDING_LIBRARY=1 -DLIBCXX_BUILDING_LIBCXXABI=1 -D_LIBCPP_REMOVE_TRANSITIVE_INCLUDES=1)

set(cxx_sources
	src/algorithm.cpp
	src/any.cpp
	src/bind.cpp
	src/call_once.cpp
	src/charconv.cpp
	src/chrono.cpp
	src/error_category.cpp
	src/exception.cpp
	src/expected.cpp
	src/filesystem/filesystem_clock.cpp
	src/filesystem/filesystem_error.cpp
	src/filesystem/path.cpp
	src/functional.cpp
	src/hash.cpp
	src/legacy_pointer_safety.cpp
	src/memory.cpp
	src/memory_resource.cpp
	src/new_handler.cpp
	src/new_helpers.cpp
	src/optional.cpp
	src/print.cpp
	src/random_shuffle.cpp
	src/ryu/d2fixed.cpp
	src/ryu/d2s.cpp
	src/ryu/f2s.cpp
	src/stdexcept.cpp
	src/string.cpp
	src/system_error.cpp
	src/typeinfo.cpp
	src/valarray.cpp
	src/variant.cpp
	src/vector.cpp
	src/verbose_abort.cpp

	# threads
	src/atomic.cpp
	src/barrier.cpp
	src/condition_variable_destructor.cpp
	src/condition_variable.cpp
	src/future.cpp
	src/mutex_destructor.cpp
	src/mutex.cpp
	src/shared_mutex.cpp
	src/thread.cpp

	# random_device
	src/random.cpp

	# localization
	src/fstream.cpp
	src/ios.cpp
	src/ios.instantiations.cpp
	src/iostream.cpp
	src/locale.cpp
	src/ostream.cpp
	src/regex.cpp
	src/strstream.cpp

	# filesystem
	src/filesystem/directory_entry.cpp
	src/filesystem/directory_iterator.cpp
	src/filesystem/operations.cpp

	# DARLING: symbols that older releases of the dylib exported
	src/darling/legacy_debug.cpp
	src/darling/legacy_exports.cpp
	src/darling/legacy_format_error.cpp
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

# For dyld only. dyld runs without libSystem: as for cxxabi_static, no dlopen() for the check of libstdc++'s
# empty string, and no library aligned allocation.
add_darling_static_library(cxx_static SOURCES ${cxx_sources})
target_compile_definitions(cxx_static PRIVATE
	_LIBCPP_HAS_NO_LIBRARY_ALIGNED_ALLOCATION=1
	_LIBCPP_DARLING_NO_GCC_EMPTY_STRING_CHECK=1
)
make_fat(cxx_static)

