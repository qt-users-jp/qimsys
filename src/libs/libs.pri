contains(CONFIG, qt) {
    isEqual(QT_MAJOR_VERSION, 4) {
        include(qt4/qt4.pri)
    }
}

contains(CONFIG, gtk): include(gtk/gtk.pri)
