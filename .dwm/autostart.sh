#!/bin/sh

tmp_file="/tmp/dwm_once.START"

if [ ! -f "$tmp_file" ]; then
	echo "0" >"$tmp_file"
fi

dwm_once=$(cat "$tmp_file")

exec_once() {
	name=$(find "/usr/share/backgrounds/" -name "background.st.*")
	if [ name != "" ]; then
		feh --bg-scale "usr/share/backgrounds/$name"
	fi
	slstatus &
	pipewire &
	echo "1" >"$exec_once"
}

exec_always() {
	pkill --signal SIGUSR1 slstatus
}

if [ "$dwm_once" != "1" ]; then
	echo "1" >"$tmp_file"
	exec_once
else
	exec_always
fi
