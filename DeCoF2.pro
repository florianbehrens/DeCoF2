TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    example/main.cpp \
    src/client_context.cpp \
    src/event.cpp \
    src/exceptions.cpp \
    src/node.cpp \
    src/object_dictionary.cpp \
    src/object_visitor.cpp \
    src/regular_timer.cpp \
    src/string_codec.cpp \
    src/tcp_connection.cpp \
    src/tcp_connection_manager.cpp \
    src/textproto_clisrv.cpp \
    src/textproto_pubsub.cpp \
    src/textproto_visitor.cpp \
    src/tree_element.cpp \
    src/xml_visitor.cpp

HEADERS += \
    include/decof.h \
    include/decof/basic_parameter.h \
    include/decof/basic_readwrite_parameter.h \
    include/decof/client_context.h \
    include/decof/event.h \
    include/decof/exceptions.h \
    include/decof/external_readonly_parameter.h \
    include/decof/external_readwrite_parameter.h \
    include/decof/managed_readonly_parameter.h \
    include/decof/managed_readwrite_parameter.h \
    include/decof/node.h \
    include/decof/object_dictionary.h \
    include/decof/observable_parameter.h \
    include/decof/readwrite_parameter.h \
    include/decof/tree_element.h \
    include/decof/types.h \
    include/decof/userlevel.h \
    src/connection.h \
    src/errors.h \
    src/object_visitor.h \
    src/regular_timer.h \
    src/string_codec.h \
    src/tcp_connection.h \
    src/tcp_connection_manager.h \
    src/textproto_clisrv.h \
    src/textproto_pubsub.h \
    src/textproto_visitor.h \
    src/xml_visitor.h

INCLUDEPATH += include include/decof src

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../boost_1_54_0/stage/lib/ -llibboost_system-mgw48-mt-1_54
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../boost_1_54_0/stage/lib/ -llibboost_system-mgw48-mt-1_54 -lwsock32 -lws2_32
else:unix: LIBS += -lboost_system

win32:INCLUDEPATH += $$PWD/../boost_1_54_0
win32:DEPENDPATH += $$PWD/../boost_1_54_0

OTHER_FILES +=
