include(../../../../qimsysplatform.pri)
TEMPLATE = subdirs

!contains(QIMSYS_CONFIG, no-gtk2immodule) SUBDIRS += gtk2
!contains(QIMSYS_CONFIG, no-gtk3immodule) SUBDIRS += gtk3
