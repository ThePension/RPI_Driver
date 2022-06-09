QT += core gui widgets network charts

CONFIG += C++20

SOURCES += main_client.cpp \
          mainwindow.cpp \
          client.cpp \
          ../common/data.cpp

HEADERS += mainwindow.h \
            client.h \
            ../common/data.h
