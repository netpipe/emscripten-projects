#!/bin/bash
name=SoftBody
mkdir /home/netpipe/public_html/emtests/irrlicht/$name
directory=/home/netpipe/public_html/emtests/irrlicht/$name

cp $name.html $name.js $name.data $name.wasm $directory

cd $directory
sed -i '/___buildEnvironment(__get_environ());/d' "./$name.js"
#./fix


#put in args into html file to generate the text
