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

withCreateDmg()
{
  if [ ! -d /tmp/create-dmg ]
  then
    (cd /tmp; git clone https://github.com/andreyvit/create-dmg.git)
  fi

  rm -rf /tmp/create-dmg/PictureViewer.app
  rm -rf /tmp/create-dmg/PictureViewer*.dmg
  \cp -Rp build/PictureViewer.app /tmp/create-dmg
  \cp resources/images/Icon.icns /tmp/create-dmg/icon.icns
  (cd /tmp/create-dmg;
    ./create-dmg --window-size 500 300 --icon-size 96 \
      --volname "PictureViewer Installer" --app-drop-link 380 205 \
      'PictureViewer.dmg' 'PictureViewer.app')
}

withMakeDmg()
{
  if [ ! -d /tmp/make_dmg ]
  then
    (cd /tmp; hg clone https://bitbucket.org/rgl/make_dmg)
  fi

  rm -rf /tmp/make_dmg/PictureViewer.app
  rm -rf /tmp/make_dmg/PictureViewer*.dmg
  \cp -Rp build/PictureViewer.app /tmp/make_dmg
  \cp resources/images/Icon.icns /tmp/make_dmg
  \cp resources/images/background.jpg /tmp/make_dmg/
  (cd /tmp/make_dmg;
    ./make_dmg \
    -image background.jpg \
    -volname "PictureViewer Installer" \
    -volicon Icon.icns \
    -file 144,144 PictureViewer.app \
    -symlink 416,144 /Applications \
    -convert UDBZ \
    PictureViewer.dmg)
  \cp /tmp/make_dmg/PictureViewer.dmg ~/projects/cpp/products/
}

rm -f Makefile
rm -f lib/Makefile
rm -f src/Makefile
rm -rf build/PictureViewer.app
$QTDIR/bin/qmake pictureviewer.pro -spec macx-clang CONFIG+=release CONFIG+=x86_64 \
  && make -j8 \
  && $QTDIR/bin/macdeployqt build/PictureViewer.app -appstore-compliant -fs=APFS \
  && withMakeDmg

rm -f Makefile
