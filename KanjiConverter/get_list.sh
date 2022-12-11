#!/bin/bash

if [ -f 'out.txt' ]
then
    rm out.txt
fi

if [ -f 'temp.txt' ]
then
    rm temp.txt
fi

touch out.txt
touch temp.txt

if ! [ -f "tagsoup-1.2.1.jar" ]
then
    curl "https://repo1.maven.org/maven2/org/ccil/cowan/tagsoup/tagsoup/1.2.1/tagsoup-1.2.1.jar" > "tagsoup-1.2.1.jar"
fi

while read -r line
do
    curl $line > words.html
    java -jar tagsoup-1.2.1.jar --files words.html
    python3 parse.py words.xhtml
    rm words.xhtml
    rm words.html
done < links.txt

awk '!x[$0]++' temp.txt > out.txt
rm temp.txt