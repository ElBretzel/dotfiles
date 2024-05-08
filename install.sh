#!/bin/sh

install_proc() {
	echo "This is the first installation process."
	echo "You can also update an existing configuration with \`make update\` or uninstall with \`make uninstall\`"
	echo
	echo "Make sure you runned \`make\` before continuing the installation."
	echo "If you are unsure, please follow the README.md file (Installation part)"
	read -p "Press any key to continue the installation." choice

	cur=$(pwd)

	suckless_init() {
		echo "---------------"
		echo "Accessing $1 folder: $HOME/.config/$1"
		cd "$HOME/.config/$1"
		echo "Trying to compile $1"
		echo "Executing command: sudo make clean install in folder $HOME/.config/$1"
		sudo make clean install 2>"$cur/$1.LOG" 1>/dev/null
		echo "Installation completed. Please check the logs if any failure ($cur/$1.LOG)"
		cd "$cur"
	}

	suckless_init dwm
	suckless_init slock
	suckless_init slstatus
	suckless_init st
	echo ""
	echo ""
	echo "st information:"
	echo "You can set your own background using the sh script at ./.config/st/new-bg.sh"
	echo "You will need to install farbfeld to use jpg2ff command"
	echo "Script usage: Specify a JPG/JPEG file in the script argument"
	echo "Ex: ././config/st/new-bg.sh my_background.jpg"
	echo "Then execute the 'make update' command to apply the changes in the dotfile root directory"
	read -p "Press any key to continue the installation." a
	suckless_init dmenu

	echo
	echo

	present=$(cat $HOME/.xinitrc | grep -E "^exec dbus-run-session -- dwm$" | wc -c)
	instance=$(cat $HOME/.xinitrc | grep "dwm" | wc -c)
	if [ "$present" -eq 0 ] && [ "$instance" -gt 0 ]; then
		echo "dwm instance found in $HOME/.xinitrc"
		echo "If the installation does not work, you can replace manually this instance with: \`exec dbus-run-session -- dwm\`"
		exit
	fi

	if [ "$present" -eq 0 ]; then
		echo "Adding dwm in .xinitrc"
		echo "exec dbus-run-session -- dwm" >>"$HOME/.xinitrc"
		exit
	fi

	echo "Adding termux new shell script"
	sudo ln -s $HOME/.config/dwm/new-tmux.sh /usr/local/bin/new-tmux.sh
	sudo ln -s $HOME/.config/dwm/switch-tmux.sh /usr/local/bin/switch-tmux.sh
}

uninstall_proc() {
	suckless_uninstall() {
		echo "---------------"
		echo "Accessing $1 folder: $HOME/.config/$1"
		cd "$HOME/.config/$1"
		echo "Trying to uninstall $1"
		echo "Executing command: sudo make clean in folder $HOME/.config/$1"
		sudo make clean
		echo "Executing command: sudo make uninstall in folder $HOME/.config/$1"
		sudo make uninstall
		cd "$cur"
	}

	suckless_uninstall dwm
	suckless_uninstall slock
	suckless_uninstall slstatus
	suckless_uninstall st
	suckless_uninstall dmenu

	echo "Removing termux new shell script"
	sudo rm /usr/local/bin/new-shell.sh

}

clean_proc() {
	cd "$cur"
	suckless_clean() {
		echo "---------------"
		echo "Accessing $1 folder: $HOME/.config/$1"
		cd "$HOME/.config/$1"
		echo "Trying to clean $1"
		echo "Executing command: sudo make clean in folder $HOME/.config/$1"
		sudo make clean
	}

	suckless_clean dwm
	suckless_clean slock
	suckless_clean slstatus
	suckless_clean st
	suckless_clean dmenu

}

update_proc() {
	cur=$(pwd)
	suckless_update() {
		echo "---------------"
		echo "Accessing $1 folder: $HOME/.config/$1"
		cd "$HOME/.config/$1"
		echo "Trying to copy header file in $1"
		sudo cp config.h config.h.OLD
		sudo rm config.h
		echo "Executing command: sudo make config.h in folder $HOME/.config/$1"
		sudo make config.h
		echo "Trying to update in $1"
		echo "Executing command: sudo make clean install in folder $HOME/.config/$1"
		sudo make clean install 2>"$cur/$1.LOG" 1>/dev/null
		echo "Update completed. Please check the logs if any failure ($cur/$1.LOG)"
		cd "$cur"
	}

	suckless_update dwm
	suckless_update slock
	suckless_update slstatus
	suckless_update st
	suckless_update dmenu

	echo
	echo
	echo "Do not forget to restart the updated binaries"

}

case "$1" in
"install") install_proc ;;
"uninstall") uninstall_proc ;;
"clean") clean_proc ;;
"update") update_proc ;;
*) echo "Please run \`make\` instead of this file!" ;;
esac
