#!/bin/sh

set -e

name="st_wallpaper.ff"

if [ ! -d "/usr/share/backgrounds/" ]; then
	sudo mkdir -p "/usr/share/backgrounds/"
	sudo chmod -R 777 "/usr/share/backgrounds/"
fi

cp "$1" "/usr/share/backgrounds/$1"
ext=$(echo "$1" | sed "s/.*\.\(.*\)/\1/")
find "/usr/share/backgrounds/" -maxdepth 1 -name "*.st.*" -type l -delete
ln -s "/usr/share/backgrounds/$1" "/usr/share/backgrounds/background.st.$ext"
name_wh_ext=$(echo "$1" | sed "s/\..*//")
echo "Converting to jpg"
convert "$1" -resize 1920x1080 "$name_wh_ext.jpg"
echo "Compiling ff-blur"
cc -o ff-blur -lm ff-blur.c
echo "Compiling ff-border"
cc -o ff-border -lm ff-border.c
echo "Compiling ff-bright"
cc -o ff-bright -lm ff-bright.c
echo "Converting wallpaper to farbfeld..."
echo "This will take some time..."
jpg2ff <"$name_wh_ext.jpg" | ./ff-border e 50 | ./ff-bright rgba 0 0.5 1 | ./ff-blur 50 15 >"/usr/share/backgrounds/$name"
echo "The wallpaper has been converted to $name"
feh --bg-scale "$1"
echo "Cleaning..."
rm "$name_wh_ext.jpg"
find -name "ff-*.o" -delete
