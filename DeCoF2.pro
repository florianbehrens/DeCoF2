TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    node.cpp \
    tree_element.cpp \
    object_dictionary.cpp \
    scheme_protocol.cpp \
    errors.cpp \
    scheme_monitor_protocol.cpp \
    string_encoder.cpp \
    client_proxy.cpp \
    poll_timer.cpp \
    server.cpp

HEADERS += \
    node.h \
    tree_element.h \
    object_dictionary.h \
    scheme_protocol.h \
    errors.h \
    scheme_errors.h \
    external_readonly_parameter.h \
    external_readwrite_parameter.h \
    managed_readonly_parameter.h \
    managed_readwrite_parameter.h \
    readwrite_parameter.h \
    basic_parameter.h \
    observable_parameter.h \
    scheme_monitor_protocol.h \
    scheme_common.h \
    container_types.h \
    string_encoder.h \
    client_proxy.h \
    poll_timer.h \
    server.h

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../boost_1_54_0/stage/lib/ -llibboost_system-mgw48-mt-1_54
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../boost_1_54_0/stage/lib/ -llibboost_system-mgw48-mt-1_54 -lwsock32 -lws2_32
else:unix: LIBS += -L$$PWD/../boost_1_54_0/stage/lib/ -llibboost_system-mgw48-mt-1_54

INCLUDEPATH += $$PWD/../boost_1_54_0
DEPENDPATH += $$PWD/../boost_1_54_0

OTHER_FILES +=
