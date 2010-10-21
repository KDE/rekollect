#! /usr/bin/env bash
$EXTRACTRC `find . -name \*.rc` >> rc.cpp
$XGETTEXT *.cpp -o $podir/rekollect.pot
rm -f *.cpp
