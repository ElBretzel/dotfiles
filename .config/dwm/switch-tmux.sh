#!/bin/sh

cd "$HOME/.config/dwm/tmux-integration/"
session=$(tmux ls -F "#{session_name}" | fzf-tmux -p 80%,60% --header="Change session" --ansi --layout=reverse-list --bind='ctrl-u:preview-half-page-up,ctrl-d:preview-half-page-down' --preview="python tmux-pane.py {}")
if [ "$session" != '' ]; then
	tmux switch -t "$session"
fi
