# Ubuntu, CentOS, Manjaro, OpenSUSE


# pkg-config is needed.

CONFIG *= link_pkgconfig
PKGCONFIG *= \
    libavcodec \
    libavdevice \
    libavfilter \
    libavformat \
    libavutil \
    libpostproc \
    libswresample \
    libswscale \
    sdl2


# linuxdeployqt is needed.

include(Linux_linuxdeployqt.pri)
