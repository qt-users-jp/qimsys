contains(CONFIG, qt) {
    isEqual(QT_MAJOR_VERSION, 4): include(qt4/qt4.pri)
    isEqual(QT_MAJOR_VERSION, 5): include(qt5/qt5.pri)
}

contains(CONFIG, gtk): include(gtk/gtk.pri)
