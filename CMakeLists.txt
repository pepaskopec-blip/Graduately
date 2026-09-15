cmake_minimum_required(VERSION 3.16)
project(maturita C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

# GTK4 is found through pkg-config everywhere: Linux/macOS packages, MSYS2,
# vcpkg and the native MSVC (gvsbuild) package all ship .pc files.
if(MSVC)
    # On MSVC the Unix-style flags pkg-config prints are not understood, so
    # ask for the native "/I" and ".lib" form instead.
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

# WIN32 suppresses the console window on Windows; the app still uses main(),
# so MSVC needs mainCRTStartup as the entry point.
file(GLOB MATURITA_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
if(WIN32)
    add_executable(maturita WIN32 ${MATURITA_SOURCES})
else()
    add_executable(maturita ${MATURITA_SOURCES})
endif()

if(MSVC)
    target_compile_options(maturita PRIVATE ${GTK4_CFLAGS})
    target_link_options(maturita PRIVATE ${GTK4_LIBS})
    target_link_options(maturita PRIVATE /ENTRY:mainCRTStartup)
    target_compile_definitions(maturita PRIVATE _CRT_SECURE_NO_WARNINGS)
else()
    target_link_libraries(maturita PRIVATE PkgConfig::GTK4 m)
    target_compile_options(maturita PRIVATE
        -Wall -Wextra -Wno-deprecated-declarations)
endif()

# The GTK stylesheet and window icon are loaded at runtime from the working
# directory (or next to the binary), so keep copies beside the built binary.
add_custom_command(TARGET maturita POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/style.css
            $<TARGET_FILE_DIR:maturita>/style.css
    COMMAND ${CMAKE_COMMAND} -E make_directory
            $<TARGET_FILE_DIR:maturita>/icons/hicolor/512x512/apps
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/assets/app-icon.png
            $<TARGET_FILE_DIR:maturita>/icons/hicolor/512x512/apps/maturita.png
    COMMAND ${CMAKE_COMMAND} -E make_directory
            ${CMAKE_CURRENT_SOURCE_DIR}/assets/icons/hicolor/512x512/apps
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/assets/app-icon.png
            ${CMAKE_CURRENT_SOURCE_DIR}/assets/icons/hicolor/512x512/apps/maturita.png)
