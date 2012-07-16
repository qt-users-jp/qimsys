isEmpty(QIMSYSRPATH_PRI) {
QIMSYSRPATH_PRI = 1

qimsys_platform_mac {
    QMAKE_LFLAGS_SONAME = -Wl,-install_name,@rpath/PlugIns/
    QMAKE_LFLAGS += -Wl,-rpath,@loader_path/../
}

qimsys_platform_linux {
    #do the rpath by hand since it's not possible to use ORIGIN in QMAKE_RPATHDIR
    # this expands to $ORIGIN (after qmake and make), it does NOT read a qmake var
    QIMSYS_PLUGIN_RPATH = $$join(QMAKE_RPATHDIR, ":")

    QMAKE_LFLAGS += -Wl,-z,origin \'-Wl,-rpath,$${QIMSYS_PLUGIN_RPATH}\'
    QMAKE_RPATHDIR =
}

} # QIMSYSRPATH_PRI
