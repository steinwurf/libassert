cmake_minimum_required(VERSION 3.10)
project(libassert)

find_package(Python COMPONENTS Interpreter)

# Use waf to resolve dependencies
if(NOT DEFINED STEINWURF_RESOLVE)
    message(STATUS "Resolving dependencies...")
    execute_process(
    COMMAND ${Python_EXECUTABLE} waf resolve ${STEINWURF_RESOLVE_OPTIONS}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    RESULT_VARIABLE STATUS)

    if(STATUS AND NOT STATUS EQUAL 0)
        message(FATAL_ERROR "Failed: ${STATUS}")
    endif()

    set(STEINWURF_RESOLVE "${CMAKE_CURRENT_SOURCE_DIR}/resolve_symlinks")
    set(STEINWURF_TOP_NAME ${PROJECT_NAME})
endif()

if (NOT CMAKE_BUILD_TYPE)
    message(STATUS "CMAKE_BUILD_TYPE not defined, using Debug")
    set (CMAKE_BUILD_TYPE "Debug")
else ()
    message(STATUS "CMAKE_BUILD_TYPE is defined, using ${CMAKE_BUILD_TYPE}")
    set (BUILD_TYPE ${CMAKE_BUILD_TYPE})
endif()

if (UNIX AND NOT APPLE)
    # Linux-specific code
    set(LIB_PREFIX "lib")
    set(LIB_SUFFIX ".a")
elseif (APPLE)
    # Apple-Darwin-specific code
    set(LIB_PREFIX "lib")
    set(LIB_SUFFIX ".a")
elseif (WIN32)
    # Windows-specific code
    set(LIB_PREFIX "")
    set(LIB_SUFFIX ".lib")
endif ()


# include(ExternalProject)
# ExternalProject_add(project_zstd
#   SOURCE_DIR ${STEINWURF_RESOLVE}/zstd-source
#   BINARY_DIR ${STEINWURF_RESOLVE}/zstd-source/cmake_build
#   INSTALL_DIR ${STEINWURF_RESOLVE}/zstd-source/install
#   CONFIGURE_COMMAND ${CMAKE_COMMAND} ${STEINWURF_RESOLVE}/zstd-source/ -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
#   BUILD_COMMAND ${CMAKE_COMMAND} --build ${STEINWURF_RESOLVE}/zstd-source/cmake_build --config ${CMAKE_BUILD_TYPE}
#   EXCLUDE_FROM_ALL TRUE
#   STEP_TARGETS build
#   INSTALL_COMMAND ${CMAKE_COMMAND} --install ${STEINWURF_RESOLVE}/zstd-source/cmake_build --prefix ${CMAKE_BINARY_DIR}/zstd-source/install --config ${CMAKE_BUILD_TYPE}
#   BUILD_BYPRODUCTS ${STEINWURF_RESOLVE}/zstd-source/install/lib/${LIB_NAME}
# )
# add_library(zstd STATIC IMPORTED GLOBAL)
# add_dependencies(zstd project_zstd)
# add_library(steinwurf::zstd ALIAS zstd)

# # Make the include directory before we set the target properties otherwise cmake will complain
# # as this is resovled at configure time rather than build time where the header files are generated
# file(MAKE_DIRECTORY ${STEINWURF_RESOLVE}/zstd-source/install/include)
# set(ZSTD_GENERATED_INCLUDE_DIR ${STEINWURF_RESOLVE}/zstd-source/install/include)
# set_target_properties(zstd PROPERTIES
#    IMPORTED_LOCATION ${STEINWURF_RESOLVE}/zstd-source/install/lib/${LIB_NAME}
#    INTERFACE_INCLUDE_DIRECTORIES ${ZSTD_GENERATED_INCLUDE_DIR}
# )

# target_include_directories(zstd INTERFACE ${ZSTD_GENERATED_INCLUDE_DIR})

# install(TARGETS zstd)

include(GNUInstallDirs) # For `lib/lib64` detection using `CMAKE_INSTALL_LIBDIR`.
include(ExternalProject)
ExternalProject_add(project_libassert
    SOURCE_DIR ${STEINWURF_RESOLVE}/libassert-source
    BINARY_DIR ${STEINWURF_RESOLVE}/libassert-source/cmake_build
    INSTALL_DIR ${STEINWURF_RESOLVE}/libassert-source/install
    CONFIGURE_COMMAND ${CMAKE_COMMAND} ${STEINWURF_RESOLVE}/libassert-source/ -DLIBASSERT_STATIC_DEFINE=${IS_STATIC_DEFINE} -DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
    BUILD_COMMAND ${CMAKE_COMMAND} --build ${STEINWURF_RESOLVE}/libassert-source/cmake_build --config ${CMAKE_BUILD_TYPE}
    EXCLUDE_FROM_ALL TRUE
    STEP_TARGETS build
    INSTALL_COMMAND ${CMAKE_COMMAND} --install ${STEINWURF_RESOLVE}/libassert-source/cmake_build --prefix ${STEINWURF_RESOLVE}/libassert-source/install --config ${CMAKE_BUILD_TYPE}
    BUILD_BYPRODUCTS ${STEINWURF_RESOLVE}/libassert-source/install/${CMAKE_INSTALL_LIBDIR}/${LIB_PREFIX}assert${LIB_SUFFIX} ${STEINWURF_RESOLVE}/libassert-source/install/${CMAKE_INSTALL_LIBDIR}/${LIB_PREFIX}dwarf${LIB_SUFFIX} ${STEINWURF_RESOLVE}/libassert-source/install/${CMAKE_INSTALL_LIBDIR}/${LIB_PREFIX}cpptrace${LIB_SUFFIX} ${STEINWURF_RESOLVE}/libassert-source/install/${CMAKE_INSTALL_LIBDIR}/${LIB_PREFIX}zstd${LIB_SUFFIX}
)

find_package(ZLIB)

add_library(libassert STATIC IMPORTED GLOBAL)
add_library(cpptrace STATIC IMPORTED GLOBAL)


add_dependencies(libassert project_libassert)
add_library(steinwurf::libassert ALIAS libassert)
add_library(steinwurf::cpptrace ALIAS cpptrace)





file(MAKE_DIRECTORY ${STEINWURF_RESOLVE}/libassert-source/install/include)
set(LIBASSERT_GENERATED_INCLUDE_DIR ${STEINWURF_RESOLVE}/libassert-source/install/include)

set_target_properties(libassert PROPERTIES
   IMPORTED_LOCATION ${STEINWURF_RESOLVE}/libassert-source/install/${CMAKE_INSTALL_LIBDIR}/${LIB_PREFIX}assert${LIB_SUFFIX}
   INTERFACE_INCLUDE_DIRECTORIES ${LIBASSERT_GENERATED_INCLUDE_DIR}
   CXX_STANDARD 17
)

set_target_properties(cpptrace PROPERTIES
   IMPORTED_LOCATION ${STEINWURF_RESOLVE}/libassert-source/install/${CMAKE_INSTALL_LIBDIR}/${LIB_PREFIX}cpptrace${LIB_SUFFIX}
   INTERFACE_INCLUDE_DIRECTORIES ${LIBASSERT_GENERATED_INCLUDE_DIR}
   CXX_STANDARD 17
)

target_include_directories(libassert INTERFACE ${LIBASSERT_GENERATED_INCLUDE_DIR})
target_include_directories(cpptrace INTERFACE ${LIBASSERT_GENERATED_INCLUDE_DIR})

if (CMAKE_C_COMPILER_ID MATCHES "MSVC")
    target_link_libraries(cpptrace INTERFACE dbghelp)
endif ()

target_link_libraries(libassert INTERFACE cpptrace)

# MSVC does not build libdwarf and zstd
if (NOT CMAKE_C_COMPILER_ID MATCHES "MSVC")
    add_library(libdwarf STATIC IMPORTED GLOBAL)
    add_library(zstd STATIC IMPORTED GLOBAL)
    add_library(steinwurf::libdwarf ALIAS libdwarf)
    add_library(steinwurf::zstd ALIAS zstd)

    set_target_properties(libdwarf PROPERTIES
    IMPORTED_LOCATION ${STEINWURF_RESOLVE}/libassert-source/install/${CMAKE_INSTALL_LIBDIR}/${LIB_PREFIX}dwarf${LIB_SUFFIX}
    INTERFACE_INCLUDE_DIRECTORIES ${LIBASSERT_GENERATED_INCLUDE_DIR}
    CXX_STANDARD 17
    )

    set_target_properties(zstd PROPERTIES
    IMPORTED_LOCATION ${STEINWURF_RESOLVE}/libassert-source/install/${CMAKE_INSTALL_LIBDIR}/${LIB_PREFIX}zstd${LIB_SUFFIX}
    INTERFACE_INCLUDE_DIRECTORIES ${LIBASSERT_GENERATED_INCLUDE_DIR}
    CXX_STANDARD 17
    )
    target_include_directories(libdwarf INTERFACE ${LIBASSERT_GENERATED_INCLUDE_DIR})
    target_include_directories(zstd INTERFACE ${LIBASSERT_GENERATED_INCLUDE_DIR})

    target_link_libraries(libassert INTERFACE libdwarf)
    target_link_libraries(libassert INTERFACE zstd)
    target_link_libraries(libassert INTERFACE ${ZLIB_LIBRARIES})
endif()





#message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")
#add_library(libassert STATIC IMPORTED GLOBAL)
#add_dependencies(libassert project_libassert)

# Make the include directory before we set the target properties otherwise cmake will complain
# as this is resovled at configure time rather than build time where the header files are generated
#file(MAKE_DIRECTORY ${STEINWURF_RESOLVE}/libassert-source/install/include)
#set(LIBASSERT_GENERATED_INCLUDE_DIR ${STEINWURF_RESOLVE}/libassert-source/install/include)
#set_target_properties(libassert PROPERTIES
#   IMPORTED_LOCATION ${STEINWURF_RESOLVE}/libassert-source/install/lib/${LIB_NAME}
#   INTERFACE_INCLUDE_DIRECTORIES ${LIBASSERT_GENERATED_INCLUDE_DIR}
#)
#
#target_include_directories(libassert INTERFACE ${LIBASSERT_GENERATED_INCLUDE_DIR})




# Is top level project?
if (${PROJECT_NAME} STREQUAL ${STEINWURF_TOP_NAME})
    enable_testing()

    if (CMAKE_C_COMPILER_ID MATCHES "MSVC")
        # For Windows: Prevent overriding the parent project's compiler/linker
        # settings
        set(gtest_force_shared_crt
                ON
                CACHE BOOL "" FORCE)
    endif ()

    # Google Test dependency
    add_subdirectory("${STEINWURF_RESOLVE}/gtest-source" gtest_main
            EXCLUDE_FROM_ALL)
    # Define library
    file(GLOB_RECURSE libassert_test_sources test/**/*.cpp)

    # Build test executable
    add_executable(libassert_tests ${libassert_test_sources})
    add_dependencies(libassert_tests steinwurf::libassert)

    target_link_libraries(libassert_tests steinwurf::libassert)
    if (CMAKE_C_COMPILER_ID MATCHES "MSVC")
        target_link_libraries(libassert_tests dbghelp)
    endif ()
    target_link_libraries(libassert_tests gtest_main)


    # Make sure we compile with C++17 and do not use compiler specific extensions
    set_property(TARGET libassert_tests PROPERTY CXX_STANDARD 17)
    set_property(TARGET libassert_tests PROPERTY CXX_EXTENSIONS OFF)

    # Enable pedantic errors
    #if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    #   # GCC is throwing ISO errors from abseil that we can't fix
    #else ()
    #   target_compile_options(libassert_tests PUBLIC -pedantic-errors)
    #endif ()

    add_test(libassert_tests libassert_tests)

endif ()
