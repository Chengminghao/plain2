TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
LIBS += -Ldll -lws2_32
SOURCES += \
        main.cpp \
        modbus-data.c \
        modbus-rtu.c \
        modbus-tcp.c \
        modbus.c \
        open62541.c \
        test.cpp

HEADERS += \
    modbus-private.h \
    modbus-rtu-private.h \
    modbus-rtu.h \
    modbus-tcp-private.h \
    modbus-tcp.h \
    modbus-version.h \
    modbus.h \
    open62541.h
