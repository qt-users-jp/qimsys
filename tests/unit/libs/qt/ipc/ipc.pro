TEMPLATE = subdirs
!contains(QIMSYS_CONFIG, no-dbus) {
    SUBDIRS = qimsysabstractipcobject qimsysapplicationmanager qimsysinputmethodmanager qimsyspreeditmanager qimsyscandidatemanager qimsyskeyboardmanager
}
