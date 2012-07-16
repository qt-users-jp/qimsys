TEMPLATE = subdirs

!contains(QIMSYS_CONFIG, no-gui): SUBDIRS += standard
