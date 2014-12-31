#!/usr/bin/ruby
# coding: utf-8
'a'.encoding   #=> #<Encoding:UTF-8>

require 'optparse'

LICENSE=<<-"EOL".split( /\n/, -1 )
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   qimsys                                                                  *
 *   Copyright (C) 2010-2015 by Tasuku Suzuki <stasuku@gmail.com>            *
 *                                                                           *
 *   This program is free software; you can redistribute it and/or modify    *
 *   it under the terms of the GNU General Lesser Public License as          *
 *   published by the Free Software Foundation; either version 2 of the      *
 *   License, or (at your option) any later version.                         *
 *                                                                           *
 *   This program is distributed in the hope that it will be useful,         *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
 *   GNU Lesser General Public License for more details.                     *
 *                                                                           *
 *   You should have received a copy of the GNU Lesser General Public        *
 *   License along with this program; if not, write to the                   *
 *   Free Software Foundation, Inc.,                                         *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.               *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
EOL

def main
    begin
        ARGV.push( '-h' ) if ARGV.empty?

        linefeed = "\n"
        ARGV.options do |opt|
            opt.on( '-u VALUE', '--linefeed=VALUE', { 'unix' => "\n", 'msdos' => "\r\n" }, '改行コード(unix or msdos) デフォルトは unix' ) { |l| linefeed = l }
            opt.banner += " file[s]\n"
            opt.banner += "   or: find -name \"*.h\" -o -name \"*.cpp\" | xargs #{opt.program_name} [options]"
            opt.parse!()
        end
        ARGV.each do |arg|
	p arg
            raise ArgumentError, "\"#{arg}\" は通常ファイルではありません。" unless FileTest.file?( arg )

            print "#{arg} ... "
            case File.basename( arg )
            when /^moc_/, /^qrc_/, /^ui_/
                print "skipped\n"
            else
                File.open( arg, "r+" ) do |file|
                    lines = file.readlines().collect() { |line| line.chomp  }

                    # ライセンス表示の差し替え
                    while( lines.first !~ /^#/ )
                        lines.shift
                    end
                    lines.unshift LICENSE

                    # ファイル末尾の改行数の調整
                    while( lines.last.empty? )
                        lines.pop
                    end
                    lines.push( "" )

                    # 改行コードを設定してファイルを上書き
                    file.rewind
                    file.truncate( 0 )
                    file.write lines.join( linefeed )
                    file.flush
                end
                print "done\n"
            end
        end
    rescue ArgumentError
        print $!.to_s + "\n"
    end
end

main
