#!/bin/ksh
#
# Script to prepare a dmg bundle with application for distribution
#
DIR=`dirname $0`/..
cd $DIR

if [ -d build/release ]
then
  rm -rf build/release
fi

rm -f Makefile
$QTDIR/bin/qmake pictureviewer.pro -spec macx-clang CONFIG+=release CONFIG+=x86_64 \
  && make -j8 \
  && $QTDIR/bin/macdeployqt build/release/PictureViewer.app -dmg -appstore-compliant

rm -f Makefile
