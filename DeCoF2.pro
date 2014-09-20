TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    example/main.cpp \
    src/core/client_context.cpp \
    src/core/event.cpp \
    src/core/exceptions.cpp \
    src/core/node.cpp \
    src/core/object.cpp \
    src/core/object_dictionary.cpp \
    src/core/object_visitor.cpp \
    src/core/regular_timer.cpp \
    src/core/string_codec.cpp \
    src/core/tcp_connection.cpp \
    src/core/tcp_connection_manager.cpp \
    src/textproto/textproto_clisrv.cpp \
    src/textproto/textproto_pubsub.cpp \
    src/textproto/textproto_visitor.cpp \
    src/webservice/http_reply.cpp \
    src/webservice/request_parser.cpp \
    src/webservice/webservice.cpp \
    src/webservice/xml_visitor.cpp

HEADERS += \
    include/decof.h \
    include/decof/basic_parameter.h \
    include/decof/basic_readwrite_parameter.h \
    include/decof/client_context.h \
    include/decof/conversion.h \
    include/decof/event.h \
    include/decof/exceptions.h \
    include/decof/external_readonly_parameter.h \
    include/decof/external_readwrite_parameter.h \
    include/decof/managed_readonly_parameter.h \
    include/decof/managed_readwrite_parameter.h \
    include/decof/node.h \
    include/decof/object.h \
    include/decof/object_dictionary.h \
    include/decof/readwrite_parameter.h \
    include/decof/typed_parameter.h \
    include/decof/types.h \
    include/decof/userlevel.h \
    src/core/connection.h \
    src/core/errors.h \
    src/core/object_visitor.h \
    src/core/regular_timer.h \
    src/core/string_codec.h \
    src/core/tcp_connection.h \
    src/core/tcp_connection_manager.h \
    src/textproto/textproto_clisrv.h \
    src/textproto/textproto_pubsub.h \
    src/textproto/textproto_visitor.h \
    src/webservice/header.hpp \
    src/webservice/http_reply.h \
    src/webservice/request.hpp \
    src/webservice/request_parser.hpp \
    src/webservice/webservice.h \
    src/webservice/xml_visitor.h

INCLUDEPATH += include include/decof src src/core

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../boost_1_54_0/stage/lib/ -llibboost_system-mgw48-mt-1_54
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../boost_1_54_0/stage/lib/ -llibboost_system-mgw48-mt-1_54 -lwsock32 -lws2_32
else:unix: LIBS += -lboost_system

win32:INCLUDEPATH += $$PWD/../boost_1_54_0
win32:DEPENDPATH += $$PWD/../boost_1_54_0

OTHER_FILES += \
    README.md
