#!/bin/bash

VERSION=$1

if [ "$VERSION asd" = " asd" ]
then
    echo Usage: release.sh VERSION
    exit 1
fi

svn export . ../releases/isomaster-$VERSION
if [ $? -ne 0 ]
then
    exit 1
fi

rm ../releases/isomaster-$VERSION/bkisofs-manual
if [ $? -ne 0 ]
then
    exit 1
fi

make bk-doc
if [ $? -ne 0 ]
then
    exit 1
fi

mkdir ../releases/isomaster-$VERSION/bkisofs-manual
if [ $? -ne 0 ]
then
    exit 1
fi

cp ../doc/byme/bkisofs-manual/* ../releases/isomaster-$VERSION/bkisofs-manual/
if [ $? -ne 0 ]
then
    exit 1
fi

rm -r ../releases/isomaster-$VERSION/icons/originals
if [ $? -ne 0 ]
then
    exit 1
fi

cd ../releases
tar cvjf isomaster-$VERSION.tar.bz2 isomaster-$VERSION/
if [ $? -ne 0 ]
then
    exit 1
fi

echo isomaster-$VERSION.tar.bz2 created
