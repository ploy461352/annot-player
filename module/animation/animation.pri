# animation.pri
# 5/5/2012

include(../../config.pri)

DEFINES += WITH_MODULE_ANIMATION

DEPENDPATH += $$PWD

HEADERS += \
    $$PWD/fadeanimation.h

SOURCES += \
    $$PWD/fadeanimation.cc

QT      += core

# EOF
