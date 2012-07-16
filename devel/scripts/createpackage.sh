#!/bin/sh

if [ $# -ne 1 ]; then
	echo $0 x.y.x
	exit
fi
VERSION=$1
shift

wget http://gitorious.org/qimsys/qimsys/archive-tarball/master -O master.tar.gz
`file master.tar.gz | grep ASCII`
if [ $? -eq 0 ]; then
	cat master.tar.gz
	exit
fi
tar zxf master.tar.gz
rm master.tar.gz
cd qimsys-qimsys
../${0%/*}/striplog.sh
cd -
mv qimsys-qimsys qimsys-$VERSION
tar czf qimsys-$VERSION.tar.gz qimsys-$VERSION
rm -rf qimsys-$VERSION

