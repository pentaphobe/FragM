#!/usr/bin/env bash



qmake -spec macx-xcode Fragmentarium-Source.pro
qmake -project -after "QT += widgets opengl xml script scripttools" -after "INCLUDEPATH += '../../glm'"
