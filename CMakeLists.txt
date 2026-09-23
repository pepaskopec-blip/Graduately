cmake_minimum_required(VERSION 3.16)
project(graduately C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# GTK4 is found through pkg-config everywhere: Linux/macOS packages, MSYS2,
# vcpkg and the native MSVC (gvsbuild) package all ship .pc files.
if(MSVC)
    find_program(PKG_CONFIG_EXECUTABLE
        NAMES pkg-config pkg-config.exe pkgconf pkgconf.exe)
    if(NOT PKG_CONFIG_EXECUTABLE)
        message(FATAL_ERROR
            "pkg-config was not found. A native GTK4 install for MSVC "
            "(e.g. gvsbuild) provides it; set PKG_CONFIG_PATH to "
            "<GTK>/lib/pkgconfig. See README.md.")
    endif()

    execute_process(
        COMMAND "${PKG_CONFIG_EXECUTABLE}" --cflags gtk4 --msvc-syntax
        OUTPUT_VARIABLE GTK4_CFLAGS_RAW
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE GTK4_CFLAGS_RESULT)
    execute_process(
        COMMAND "${PKG_CONFIG_EXECUTABLE}" --libs gtk4 --msvc-syntax
        OUTPUT_VARIABLE GTK4_LIBS_RAW
        OUTPUT_STRIP_TRAILING_WHITESPACE
        RESULT_VARIABLE GTK4_LIBS_RESULT)

    if(NOT GTK4_CFLAGS_RESULT EQUAL 0 OR NOT GTK4_LIBS_RESULT EQUAL 0)
        message(FATAL_ERROR
            "pkg-config could not find GTK4. Set PKG_CONFIG_PATH to "
            "<GTK>/lib/pkgconfig (for example C:\\gtk\\lib\\pkgconfig).")
    endif()

    separate_arguments(GTK4_CFLAGS WINDOWS_COMMAND "${GTK4_CFLAGS_RAW}")
    separate_arguments(GTK4_LIBS WINDOWS_COMMAND "${GTK4_LIBS_RAW}")
else()
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(GTK4 REQUIRED IMPORTED_TARGET gtk4)
endif()

file(GLOB GRADUATELY_SOURCES CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
if(WIN32)
    add_executable(graduately WIN32 ${GRADUATELY_SOURCES}
                   "${CMAKE_CURRENT_SOURCE_DIR}/data/windows/maturita.rc")
else()
    add_executable(graduately ${GRADUATELY_SOURCES})
endif()

target_include_directories(graduately PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src")

if(DEFINED APP_COMMIT AND NOT APP_COMMIT STREQUAL "")
    target_compile_definitions(graduately PRIVATE APP_COMMIT="${APP_COMMIT}")
endif()

if(MSVC)
    target_compile_options(graduately PRIVATE ${GTK4_CFLAGS})
    target_link_options(graduately PRIVATE ${GTK4_LIBS})
    target_link_options(graduately PRIVATE /ENTRY:mainCRTStartup)
    target_compile_definitions(graduately PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
    target_link_libraries(graduately PRIVATE PkgConfig::GTK4 m)
    target_compile_options(graduately PRIVATE
        -Wall -Wextra -Wno-deprecated-declarations)
    if(APPLE)
        target_link_libraries(graduately PRIVATE
            "-framework AppKit" "-framework Foundation")
    endif()
endif()

# Runtime assets sit next to the binary after install / cmake --build.
set(ICON_512
    "${CMAKE_CURRENT_SOURCE_DIR}/data/share/icons/hicolor/512x512/apps/maturita.png")
add_custom_command(TARGET graduately POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/data/style.css
            $<TARGET_FILE_DIR:graduately>/style.css
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/data/changelog.txt
            $<TARGET_FILE_DIR:graduately>/changelog.txt
    COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:graduately>/icons/hicolor/512x512/apps
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${ICON_512}
            $<TARGET_FILE_DIR:graduately>/icons/hicolor/512x512/apps/maturita.png
    COMMAND ${CMAKE_COMMAND} -E copy_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/data/share
            $<TARGET_FILE_DIR:graduately>/share)
