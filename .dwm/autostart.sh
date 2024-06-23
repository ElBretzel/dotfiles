#!/bin/sh

tmp_file="/tmp/dwm_once.START"

if [ ! -f "$tmp_file" ]; then
	echo "0" >"$tmp_file"
fi

dwm_once=$(cat "$tmp_file")

exec_once() {
	name=$(find "/usr/share/backgrounds/" -name "background.st.*")
	if [ name != "" ]; then
		feh --bg-scale "$name"
	fi
	#ntpdate -s fr.pool.ntp.org
	sleep 1
	pipewire &
	sleep 1
	slstatus &
	echo "1" >"$tmp_file"
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
