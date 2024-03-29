TEMPLATE = lib

TARGET = CQColors

QT += widgets svg

DEPENDPATH += .

QMAKE_CXXFLAGS += \
-std=c++17 \
-DCQCHARTS_FOLDED_MODEL \

MOC_DIR = .moc

CONFIG += staticlib
CONFIG += c++17

SOURCES += \
CQColors.cpp \
CQColorsPalette.cpp \
CQColorsTheme.cpp \
CQColorsDefPalettes.cpp \
CQColorsDefThemes.cpp \
\
CQColorsEditCanvas.cpp \
CQColorsEditControl.cpp \
CQColorsEditList.cpp \
CQColorsEditColorModel.cpp \
CQColorsEditColorType.cpp \
CQColorsEditDefinedColors.cpp \
CQColorsEditModel.cpp \

HEADERS += \
../include/CQColorsDefPalettes.h \
../include/CQColorsDefThemes.h \
../include/CQColors.h \
../include/CQColorsPalette.h \
../include/CQColorsTheme.h \
\
../include/CQColorsEditCanvas.h \
../include/CQColorsEditControl.h \
../include/CQColorsEditList.h \
../include/CQColorsEditColorModel.h \
../include/CQColorsEditColorType.h \
../include/CQColorsEditDefinedColors.h \
../include/CQColorsEditModel.h \
\
../include/CCubeHelix.h \

DESTDIR     = ../lib
OBJECTS_DIR = ../obj
LIB_DIR     = ../lib

INCLUDEPATH += \
. \
../include \
../../CQCharts/include \
../../CQUtil/include \
../../CImageLib/include \
../../CFont/include \
../../CFile/include \
../../CStrUtil/include \
../../CUtil/include \
../../CMath/include \
../../COS/include \
/usr/include/tcl \
