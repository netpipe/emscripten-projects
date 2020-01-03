#!/bin/bash

export LANG=en_CA

for NAME in *.po
do
  echo $NAME
  msgfmt -c -v $NAME -o `basename $NAME .po`.mo 2> temp
  cat temp | awk '{ str=substr($0, match($0, /([0-9]+) translated/), RLENGTH); if(str)print str;else print 0; }' | awk '{ print $1 }' > `basename $NAME .po`.percent
  cat temp | awk '{ str=substr($0, match($0, /([0-9]+) fuzzy/), RLENGTH); if(str)print str;else print 0; }' | awk '{ print $1 }' >> `basename $NAME .po`.percent
  cat temp | awk '{ str=substr($0, match($0, /([0-9]+) untranslated/), RLENGTH); if(str)print str;else print 0; }' | awk '{ print $1 }' >> `basename $NAME .po`.percent
done

rm -f temp
