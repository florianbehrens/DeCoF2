TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    example/main.cpp \
    src/client_context.cpp \
    src/exceptions.cpp \
    src/node.cpp \
    src/object_dictionary.cpp \
    src/regular_timer.cpp \
    src/string_codec.cpp \
    src/tcp_connection.cpp \
    src/tcp_connection_manager.cpp \
    src/textproto_clisrv.cpp \
    src/textproto_pubsub.cpp \
    src/tree_element.cpp

HEADERS += \
    include/basic_parameter.h \
    include/basic_readwrite_parameter.h \
    include/client_context.h \
    include/exceptions.h \
    include/external_readonly_parameter.h \
    include/external_readwrite_parameter.h \
    include/managed_readonly_parameter.h \
    include/managed_readwrite_parameter.h \
    include/node.h \
    include/object_dictionary.h \
    include/observable_parameter.h \
    include/readwrite_parameter.h \
    include/tree_element.h \
    src/connection.h \
    src/errors.h \
    src/regular_timer.h \
    src/string_codec.h \
    src/tcp_connection.h \
    src/tcp_connection_manager.h \
    src/textproto_clisrv.h \
    src/textproto_pubsub.h \
    include/types.h

INCLUDEPATH += include src

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../boost_1_54_0/stage/lib/ -llibboost_system-mgw48-mt-1_54
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../boost_1_54_0/stage/lib/ -llibboost_system-mgw48-mt-1_54 -lwsock32 -lws2_32
else:unix: LIBS += -lboost_system

win32:INCLUDEPATH += $$PWD/../boost_1_54_0
win32:DEPENDPATH += $$PWD/../boost_1_54_0

OTHER_FILES +=
