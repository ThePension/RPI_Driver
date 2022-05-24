QT += network core
CONFIG += console
SOURCES += main_server.cpp \
            server.cpp \
            ../common/data.cpp
          
HEADERS += server.h \
            ../common/data.h
