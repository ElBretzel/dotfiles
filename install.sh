#!/bin/sh




echo "Make sure to first run \`make\` before continuing the installation."
read -p "Press any touch to continue." choice

cur=$(pwd)

suckless_init()
{
    echo "---------------"
    echo "Accessing $1 folder: $HOME/.config/$1"
    cd "$HOME/.config/$1"
    echo "Trying to compile $1"
    echo "Executing command: sudo make clean install in folder $HOME/.config/$1"
    sudo make clean install 2> "$cur/$1.LOG" 1> /dev/null
    echo "Installation completed. Please check the logs if any failure ($cur/$1.LOG)"
    cd "$cur"
}

suckless_init dwm
suckless_init slock
suckless_init slstatus
suckless_init st
