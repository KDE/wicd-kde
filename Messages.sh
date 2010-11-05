#!bin/sh

$EXTRACTRC `find . -name \*.rc -o -name \*.ui -o -name \*.kcfg` >> rc.cpp
$XGETTEXT `find . -name "*.cpp"` -o $podir/wicd-client-kde.pot
rm -f rc.cpp

