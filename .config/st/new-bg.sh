#!/bin/sh

set -e

name="st_wallpaper.ff"

mkdir -p $HOME/.config/st-wallpaper
echo "Compiling ff-blur"
cc -o ff-blur -lm ff-blur.c
echo "Compiling ff-border"
cc -o ff-border -lm ff-border.c
echo "Compiling ff-bright"
cc -o ff-bright -lm ff-bright.c
echo "Converting wallpaper to farbfeld..."
echo "This will take some time..."
jpg2ff <$1 | ./ff-border e 50 | ./ff-bright rgba 0 0.5 1 | ./ff-blur 50 15 >"$HOME/.config/st-wallpaper/$name"
echo "The wallpaper has been converted to $name"
