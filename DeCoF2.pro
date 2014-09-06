TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    node.cpp \
    tree_element.cpp \
    object_dictionary.cpp \
    regular_timer.cpp \
    exceptions.cpp \
    tcp_connection_manager.cpp \
    tcp_connection.cpp \
    client_context.cpp \
    string_codec.cpp \
    textproto_pubsub.cpp \
    textproto_clisrv.cpp

HEADERS += \
    node.h \
    tree_element.h \
    object_dictionary.h \
    external_readonly_parameter.h \
    external_readwrite_parameter.h \
    managed_readonly_parameter.h \
    managed_readwrite_parameter.h \
    readwrite_parameter.h \
    basic_parameter.h \
    observable_parameter.h \
    container_types.h \
    regular_timer.h \
    exceptions.h \
    errors.h \
    tcp_connection_manager.h \
    connection.h \
    tcp_connection.h \
    client_context.h \
    basic_readwrite_parameter.h \
    string_codec.h \
    textproto_pubsub.h \
    textproto_clisrv.h

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../boost_1_54_0/stage/lib/ -llibboost_system-mgw48-mt-1_54
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../boost_1_54_0/stage/lib/ -llibboost_system-mgw48-mt-1_54 -lwsock32 -lws2_32
else:unix: LIBS += -lboost_system

win32:INCLUDEPATH += $$PWD/../boost_1_54_0
win32:DEPENDPATH += $$PWD/../boost_1_54_0

OTHER_FILES +=
