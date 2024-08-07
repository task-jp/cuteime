#!/usr/bin/env ruby

require 'optparse'

Version = "0.2.0"

class Configure
    def initialize()
        ['', '5', '6'].each do |suffix|
            @qmake = `which qmake#{suffix}`.strip
            break unless @qmake.empty?
        end
        @qmake_version = `#{@qmake} -query QT_VERSION`[0]
        @qmake_default = @qmake.dup
        @debug = false
        @install_root = '/usr/local'
        case @qmake_version
        when '6', '5'
            @qt_im_module = `#{@qmake} -query QT_INSTALL_PLUGINS`.chomp + "/platforminputcontexts"
        when '4'
            @qt_im_module = `#{@qmake} -query QT_INSTALL_PLUGINS`.chomp + "/inputmethods"
        else
            raise ArgumentError "unknown qmake version #{@qmake_version}"
        end
        @qt_im_module_default = @qt_im_module.dup
        @gtk2_im_module = `pkg-config --variable=libdir gtk+-2.0`.chomp + "/gtk-2.0/" + `pkg-config --variable=gtk_binary_version gtk+-2.0`.chomp + "/immodules"
        @gtk3_im_module = `pkg-config --variable=libdir gtk+-3.0`.chomp + "/gtk-3.0/" + `pkg-config --variable=gtk_binary_version gtk+-3.0`.chomp + "/immodules"
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
        opt.on("--gtk2_im_module", "install dir for immodule for Gtk+2") { @gtk2_im_module = val }
        opt.on("--gtk3_im_module", "install dir for immodule for Gtk+3") { @gtk3_im_module = val }
        opt.on("--no-dbus", "build and install cuteime without dbus") { @config.push( 'no-dbus' ) }
        opt.on("--no-systemtray", "build and install cuteime without systemtray") { @config.push( 'no-systemtray' ) }
        opt.on("--no-toolbar", "build and install cuteime without toolbar") { @config.push( 'no-toolbar' ) }
        opt.on("--no-xim", "disable xim support") { @config.push( 'no-xim' ) }
        opt.on("--no-socialime", "disable socialime support") { @config.push( 'no-socialime' ) }
        opt.on("--no-googleime", "disable googleime cgi api support") { @config.push( 'no-googleime' ) }
        opt.on("--debug", 'build cuteime in debug mode') { @debug = true }
        opt.on("--sdk", "build and install cuteime sdk") { @config.push( 'sdk' ) }
        opt.on("--examples", "build and install examples") { @config.push( 'examples' ) }
        opt.on("--tests", "build tests") { @config.push( 'tests' ) }
        opt.parse!(ARGV.dup)
    end

    def checkPro()
        @pro = $0.sub(/configure.rb$/, "cuteime.pro")
        raise ArgumentError "cuteime.pro not found" if @pro.empty?
    end

    def checkQMake()
        raise ArgumentError "qmake not found" if @qmake.empty?
    end

    def exec()
        if @qmake != @qmake_default && @qt_im_module == @qt_im_module_default
            @qmake_version = `#{@qmake} -query QT_VERSION`[0]
            case @qmake_version
            when '6', '5'
                @qt_im_module = `#{@qmake} -query QT_INSTALL_PLUGINS`.chomp + "/platforminputcontexts"
            when '4'
                @qt_im_module = `#{@qmake} -query QT_INSTALL_PLUGINS`.chomp + "/inputmethods"
            else
                raise ArgumentError "unknown qmake version #{@qmake_version}"
            end
        end

        cmd = []
        cmd.push @qmake
        cmd.push @pro
        cmd.push '-r'
        cmd.push "PREFIX=#{@install_root}"
        cmd.push "QT_IM_MODULE_DIR=#{@qt_im_module}"
        cmd.push "GTK2_IM_MODULE_DIR=#{@gtk2_im_module}"
        cmd.push "GTK3_IM_MODULE_DIR=#{@gtk3_im_module}"
        cmd.push "CONFIG+=#{@debug ? 'debug' : 'release'}"
        cmd.push @config.collect{ |c| "CUTEIME_CONFIG+=#{c}" } unless @config.empty?

        print "Qt(qmake)          : #{@qmake} #{@debug ? '(Debug)' : ''}\n"
        print "Prefix             : #{@install_root}\n"
        print "immodule for Qt    : #{@qt_im_module}\n"
        print "immodule for Gtk+2 : #{@gtk2_im_module}\n"
        print "immodule for Gtk+3 : #{@gtk3_im_module}\n"
        print "DBus               : #{@config.include?('no-dbus') ? 'No' : 'Yes'}\n"
        print "System Tray        : #{@config.include?('no-systemtray') ? 'No' : 'Yes'}\n"
        print "Toolbar            : #{@config.include?('no-toolbar') ? 'No' : 'Yes'}\n"
        print "XIM                : #{@config.include?('no-xim') ? 'No' : 'Yes'}\n"
        print "Social IME         : #{@config.include?('no-socialime') ? 'No' : 'Yes'}\n"
        print "Google IME CGI API : #{@config.include?('no-googleime') ? 'No' : 'Yes'}\n"
        print "SDK                : #{@config.include?('sdk') ? 'Yes' : 'No'}\n"
        print "Examples           : #{@config.include?('examples') ? 'Yes' : 'No'}\n"
        print "Tests              : #{@config.include?('tests') ? 'Yes' : 'No'}\n"
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

