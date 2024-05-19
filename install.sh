#!/bin/sh

install_proc() {
	echo "WARNING: YOU SHOULD ENTER THIS COMMAND ONLY ONCE!"
	echo "TO UPDATE EXISTING CONFIGURATION, RUN \`make update\`, AND TO REMOVE COMPLETELY, RUN \`make uninstall\`"
	echo
	echo "YOU SHOULD ONLY SEE THIS TEXT THROUGH THE \`make\` COMMAND."
	echo "PLEASE BE SURE TO FOLLOW THE INSTALLATION INSTRUCTION IN THE README"
	echo "IF SUCKLESS FAILED TO COMPILE FIRST TIME, RUN \`make update\`"
	echo "Press any key ONLY if you have read and understood all the instructions above!"
	read -p "? " c
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
	echo "Ex: ././config/st/new-bg.sh my_background.png"
	read -p "Press any key to continue the installation." a
	suckless_init dmenu

	echo ""
	echo ""

	if [ -f "$HOME/.xinitrc" ]; then
		echo "Creating backup of old .xinitrc config"
		mv $HOME/.xinitrc $HOME/.xinitrc.OLD
	fi
	echo "Adding dwm in .xinitrc"
	echo "exec dbus-run-session -- dwm" >>"$HOME/.xinitrc"

	echo "Adding termux new shell script"
	if [ ! -f "/usr/local/bin/new-tmux.sh" ]; then
		sudo ln -s $HOME/.config/dwm/new-tmux.sh /usr/local/bin/new-tmux.sh
	fi
	if [ ! -f "/usr/local/bin/switch-tmux.sh" ]; then
		sudo ln -s $HOME/.config/dwm/switch-tmux.sh /usr/local/bin/switch-tmux.sh
	fi

	if [ -d "/etc/lightdm/" ]; then
		echo "Creating backup of old lightdm config"
		sudo mv /etc/lightdm /etc/lightdm.OLD/
	fi

	echo "Copying lightdm config"
	sudo mkdir -p /etc/lightdm
	sudo cp -r $HOME/.config/lightdm/ /etc/lightdm/

	echo "New lightdm entry (dwm)"
	sudo mkdir -p /usr/share/xsessions/
	sudo sh -c 'echo "[Desktop Entry]\
  Encoding=UTF-8\
  Name=dwm\
  Comment=Dynamic window Manager\
  Exec=dwm\
  Icon=dwm\
  Type=XSession" >/usr/share/xsessions/dwm2.desktop'
	echo "Created backgrounds directory"
	sudo mkdir -p "/usr/share/backgrounds/"
	sudo chmod -R 777 "/usr/share/backgrounds/"

	echo "Changing default shell to oksh"
	echo "To change back to your shell, run \`chsh -s /<path_to_favorite_shell>\`"
	chsh -s "/bin/oksh"

	echo "Installing FiraCode Nerd Font Regular"
	mkdir -p $HOME/.local/share/fonts/
	curl -fLo "$HOME/.local/share/fonts/FiraCode Nerd Font Regular.ttf" "https://github.com/ryanoasis/nerd-fonts/blob/master/patched-fonts/FiraCode/Regular/FiraCodeNerdFont-Regular.ttf"
	echo "Reloading font cache"
	fc-cache -f

	echo "Installing tmux plugins"
	tmux source $HOME/.config/tmux/tmux.conf
	$HOME/.tmux/plugins/tpm/scripts/install_plugins.sh

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

	echo "WARNING: SOME DATA WILL BE LOSE AFTER THE EXECUTION OF THIS SCRIPT."
	echo "PLEASE READ WHAT WILL BE DELETED BEFORE CONTINUING!!!"
	echo "PRESS ANY TOUCH TO CONTINUE OR CTRL-C TO QUIT"
	read -p "? " c
	suckless_uninstall dwm
	suckless_uninstall slock
	suckless_uninstall slstatus
	suckless_uninstall st
	suckless_uninstall dmenu

	echo "Clean tmux plugins"
	$HOME/.tmux/plugins/tpm/scripts/clean_plugins.sh
	echo "Removing tmux scripts"
	sudo rm /usr/local/bin/new-tmux.sh
	sudo rm /usr/local/bin/switch-tmux.sh
	echo "Switching xinitrc"
	mv $HOME/.xinitrc.OLD $HOME/.xinitrc
	echo "Switching lightdm"
	sudo mv /etc/lightdm.OLD /etc/lightdm
	echo "Removing dwm desktop file"
	sudo rm /usr/share/xsessions/dwm.desktop
	echo ""
	echo ""
	echo "Dont forget to clean /usr/share/backgrounds/ images !"

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

	echo "Clean tmux plugins"
	$HOME/.tmux/plugins/tpm/scripts/clean_plugins.sh

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

	echo "Update tmux plugins"
	$HOME/.tmux/plugins/tpm/scripts/update_plugins.sh
	echo "Update font cache"
	fc-cache -f

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
