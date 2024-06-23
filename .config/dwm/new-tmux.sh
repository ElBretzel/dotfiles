#!/bin/sh

set -e

cd "$HOME/.config/dwm/tmux-integration/"
p=$(
	python tmux-fzf.py | fzf -n 1 -d '\|' \
		--tac +m --cycle --ansi --info=inline --border --header-lines=2 \
		--layout=reverse-list --height=95% --preview-window=down,70%,wrap --preview='python tmux-pane.py {}' \
		--bind='ctrl-x:reload(python tmux-bind.py {} DELETE; python tmux-fzf.py),ctrl-r:refresh-preview,ctrl-r:reload(python tmux-fzf.py),ctrl-u:preview-up,ctrl-d:preview-down,ctrl-p:toggle-preview,ctrl-h:execute(echo "ctrl-r : Reload the current selection preview and label\nctrl-x : Delete the buffer from tmux and the list\nctrl-p : Toggle the preview\nctrl-u : Move up in the preview\nctrl-d : Move down in the preview\nctrl-h : Open this helper menu" | fzf --reverse --disabled --header="Keybind helper")' |
		sed "s/'\(.*\)'.*/\1/"
)
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
elif [ "$(echo $p | grep 'BOX: ')" != '' ]; then
	name="$(echo $p | awk '{print $2}')"
	/bin/oksh -c "distrobox enter $name -e tmux new -A -s $name"
else
	tmux new-session -A -s "$p"
fi
