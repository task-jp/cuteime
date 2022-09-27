#!/bin/sh

if [ $# -ne 1 ]; then
	echo $0 x.y.x
	exit
fi
VERSION=$1
shift

wget http://gitorious.org/cuteime/cuteime/archive-tarball/master -O master.tar.gz
`file master.tar.gz | grep ASCII`
if [ $? -eq 0 ]; then
	cat master.tar.gz
	exit
fi
tar zxf master.tar.gz
rm master.tar.gz
cd cuteime-cuteime
../${0%/*}/striplog.sh
cd -
mv cuteime-cuteime cuteime-$VERSION
tar czf cuteime-$VERSION.tar.gz cuteime-$VERSION
rm -rf cuteime-$VERSION

