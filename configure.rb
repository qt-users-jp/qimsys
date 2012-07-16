#!/usr/bin/env ruby

require 'optparse'

Version = "0.2.0"

class Configure
    def initialize()
        @qmake = `which qmake`.chomp
        @qmake_default = @qmake.dup
        @install_root = '/usr/local'
        @qt_im_module = `qmake -query QT_INSTALL_PLUGINS`.chomp + "/inputmethods"
        @qt_im_module_default = @qt_im_module.dup
        @gtk_im_module = `pkg-config --variable=libdir gtk+-2.0`.chomp + "/gtk-2.0/" + `pkg-config --variable=gtk_binary_version gtk+-2.0`.chomp + "/immodules"
        @config = []
        parseOpt()
        checkPro()
        checkQMake()
    end

    def parseOpt
        opt = OptionParser.new
        opt.on("--prefix=#{@install_root}", "install root" ) { |val| @install_root = val }
        opt.on("--qmake=#{@qmake}", 'path to qmake to use') { |val| @qmake = val }
        opt.on("--qt_im_module", "install dir for immodule for Qt") { @qt_im_module = val }
        opt.on("--gtk_im_module", "install dir for immodule for Gtk") { @gtk_im_module = val }
        opt.on("--no-dbus", "build and install qimsys without dbus") { @config.push( 'no-dbus' ) }
        opt.on("--sdk", "build and install qimsys sdk") { @config.push( 'sdk' ) }
        opt.on("--examples", "build and install examples") { @config.push( 'examples' ) }
        opt.on("--tests", "build tests") { @config.push( 'tests' ) }
        opt.parse!(ARGV.dup)
    end

    def checkPro()
        @pro = $0.sub(/configure.rb$/, "qimsys.pro")
        raise ArgumentError "qimsys.pro not found" if @pro.empty?
    end

    def checkQMake()
        raise ArgumentError "qmake not found" if @qmake.empty?
    end

    def exec()
        if @qmake != @qmake_default && @qt_im_module == @qt_im_module_default
            @qt_im_module = `#{@qmake} -query QT_INSTALL_PLUGINS`.chomp + "/inputmethods"
        end

        cmd = []
        cmd.push @qmake
        cmd.push @pro
        cmd.push '-r'
        cmd.push "PREFIX=#{@install_root}"
        cmd.push "QT_IM_MODULE_DIR=#{@qt_im_module}"
        cmd.push "GTK_IM_MODULE_DIR=#{@gtk_im_module}"
        cmd.push @config.collect{ |c| "QIMSYS_CONFIG+=#{c}" } unless @config.empty?

        print "Qt(qmake)        : #{@qmake}\n"
        print "Prefix           : #{@install_root}\n"
        print "immodule for Qt  : #{@qt_im_module}\n"
        print "immodule for Gtk : #{@gtk_im_module}\n"
        print "DBus             : #{@config.include?('no-dbus') ? 'No' : 'Yes'}\n"
        print "SDK              : #{@config.include?('sdk') ? 'Yes' : 'No'}\n"
        print "Examples         : #{@config.include?('examples') ? 'Yes' : 'No'}\n"
        print "Tests            : #{@config.include?('tests') ? 'Yes' : 'No'}\n"
#        print( "#{cmd.join(' ')}\n" )
        `#{cmd.join(' ')}`
        print( "configured!\n" )
    end
end

begin
    configure = Configure.new
    configure.exec()
rescue
    print "#{$!}\n"
end

