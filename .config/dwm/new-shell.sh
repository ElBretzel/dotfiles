#!/bin/sh

set -e

cd "$HOME/.config/dwm/tmux-integration/"
p=$(python tmux-fzf.py | fzf -n 1 -d '\|' --tac +m --cycle --layout=reverse-list --preview='python tmux-pane.py {}' | sed "s/'\(.*\)'.*/\1/")
cd ~

if [ "$p" = 'CREATE NEW SESSION' ]; then
	read -p "Enter the name of the new session: " choice
	if [ "$choice" = '' ]; then
		rand=$(dd if=/dev/urandom count=2 bs=1 | od -t d | awk '{print $2}' | head -n 1)
		tmux new-session -s "$rand-session"
	else
		tmux new-session -s "$choice"
	fi
elif [ "$p" = 'NONE' ]; then
	/bin/oksh
else
	tmux new-session -A -s $p
fi
