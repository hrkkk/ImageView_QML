cmake_minimum_required(VERSION 3.16)

project(ImageViewerQML VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.6 REQUIRED COMPONENTS Quick)

qt_standard_project_setup(REQUIRES 6.5)

# Exiv2库
include_directories("./3rdparty/Exiv2/include/")
link_directories("./3rdparty/Exiv2/lib")
# libheif库
include_directories("./3rdparty/libheif/include/")
link_directories("./3rdparty/libheif/lib/")
# libde265库
include_directories("./3rdparty/libde265/include/")
link_directories("./3rdparty/libde265/lib/")
# libx265库
include_directories("./3rdparty/libx265/include/")
link_directories("./3rdparty/libx265/lib/")
# libjpeg库
include_directories("./3rdparty/libjpeg/libjpeg-turbo64/include/")
link_directories("./3rdparty/libjpeg/libjpeg-turbo64/lib/")
# TinyEXIF库
include_directories("./3rdparty/TinyEXIF/")
# libraw库
include_directories("./3rdparty/libraw/libraw/")
link_directories("./3rdparty/libraw/lib/")
# sqlite库
include_directories("./3rdparty/sqlite/include")
link_directories("./3rdparty/sqlite/lib")
# opencv库
include_directories("./3rdparty/opencv/include")
link_directories("./3rdparty/opencv/lib")
# stb_image库
include_directories("./3rdparty/stb_image")

add_definitions(-DTINYEXIF_NO_XMP_SUPPORT)


qt_add_executable(ImageViewerQML
    main.cpp
    ExifReader.cpp ExifReader.h
    FileDeleter.cpp FileDeleter.h
    ImageItemModel.cpp ImageItemModel.h
    ImageReader.cpp ImageReader.h
    JpegReader.cpp JpegReader.h
    Utils.cpp Utils.h
)

qt_add_qml_module(ImageViewerQML
    URI ImageViewerQML
    VERSION 1.0
    QML_FILES
        Main.qml
)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(ImageViewerQML PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.ImageViewerQML
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(ImageViewerQML
    PRIVATE Qt6::Quick
	heif.lib libx265.lib de265.lib jpeg.lib libraw.lib sqlite3.lib opencv_world480.lib
)

include(GNUInstallDirs)
install(TARGETS ImageViewerQML
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
