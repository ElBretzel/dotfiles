# Sample .bashrc for SUSE Linux
# Copyright (c) SUSE Software Solutions Germany GmbH

# There are 3 different types of shells in bash: the login shell, normal shell
# and interactive shell. Login shells read ~/.profile and interactive shells
# read ~/.bashrc; in our setup, /etc/profile sources ~/.bashrc - thus all
# settings made here will also take effect in a login shell.
#
# NOTE: It is recommended to make language settings in ~/.profile rather than
# here, since multilingual X sessions would not work properly if LANG is over-
# ridden in every subshell.

test -s ~/.alias && . ~/.alias || true
. "$HOME/.cargo/env"
alias void='distrobox enter voidlinux-glibc'
alias vim='nvim'
alias fzfp="fzf --preview 'bat --color=always --style=numbers --line-range=:500 {}' --bind 'enter:execute($EDITOR {})'"
alias volume="wpctl set-volume @DEFAULT_AUDIO_SINK@"
alias ls='eza --color=always --icons=auto --hyperlink -F'
alias l='eza --color=always --icons=auto -F -l -1 --hyperlink -a --long --header --inode'
alias tree='eza --color=always --icons=auto --hyperlink -T'
alias sl='eza --color=always --icons=auto --hyperlink -F'
alias jupyter="podman run -i -t -p 8888:8888 -v $PWD:/opt/notebooks continuumio/anaconda3 /bin/bash -c 'conda install jupyter -y --quiet && conda install -c conda-forge ipympl -y --quiet && mkdir -p /opt/notebooks && jupyter notebook --notebook-dir=/opt/notebooks --ip=\* --port=8888 --no-browser --allow-root'"
setxkbmap us -variant altgr-intl
#export TERM=tmux-256color
export EDITOR='nvim'
source $HOME/.bash_git
eval "$(zoxide init posix --cmd cd --hook prompt)"
if [[ "$(env | grep 'DISTROBOX')" != '' ]]; then
	distro=$(lsb_release -a | grep "Distributor ID" | awk '{print $3}')
	PROMPT_COMMAND='PS1_CMD1=$(__git_ps1 "(%s)")'
	PS1='\[\e[2m\]<\[\e[3m\]$?\[\e[23m\]|\[\e[3m\]\A\[\e[23m\]>\[\e[0m\] [\[\e[38;5;196m\]\u\[\e[0m\]@\[\e[38;5;26m\]\[\e[1m\]BOXED_${distro}\[\e[0m\]\[\e[0m\]] \[\e[1m\]\w\[\e[0m\] \[\e[3;7m\]${PS1_CMD1}\[\e[0m\] 42\$ '
else
	PROMPT_COMMAND='PS1_CMD1=$(__git_ps1 "(%s)")'
	PS1='\[\e[2m\]<\[\e[3m\]$?\[\e[23m\]|\[\e[3m\]\A\[\e[23m\]>\[\e[0m\] [\[\e[38;5;196m\]\u\[\e[0m\]@\[\e[38;5;26m\]\h\[\e[0m\]] \[\e[1m\]\w\[\e[0m\] \[\e[3;7m\]${PS1_CMD1}\[\e[0m\] 42\$ '
fi
export PATH="$HOME/Programs/zig:$PATH"
