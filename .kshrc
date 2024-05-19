_GREEN=$(tput setaf 2)
_BLUE=$(tput setaf 4)
_RED=$(tput setaf 1)
_RESET=$(tput sgr0)
_BOLD=$(tput bold)

#export PS1="${_GREEN}\h${_BLUE}@${_RED}\u${_RESET} \W${_BOLD} \$ ${_RESET}"

PS1='${_GREEN}\u${_BLUE}@${_RED}\h${_RESET} \W ${_BOLD}$(echo [$?])\n\$${_RESET} '
#PS1='$(sps)$ '
PS2='$ '

alias vim='nvim'
alias fzfp="fzf --preview 'bat --color=always --style=numbers --line-range=:500 {}'"
alias volume="wpctl set-volume @DEFAULT_AUDIO_SINK@"
alias rel="xrdb merge $HOME/Programs/st/xresources && kill -USR1 $(pidof st)"
alias ls='eza --color=always --icons=auto --hyperlink -F'
alias l='eza --color=always --icons=auto -F -l -1 --hyperlink -a --long --header --inode'
alias tree='eza --color=always --icons=auto --hyperlink -T'
alias sl='eza --color=always --icons=auto --hyperlink -F'
alias lofi="mpv 'https://www.youtube.com/watch?v=jfKfPfyJRdk'"
alias jupyter="podman run -i -t -p 8888:8888 -v $PWD:/opt/notebooks continuumio/anaconda3 /bin/bash -c 'conda install jupyter -y --quiet && conda install -c conda-forge ipympl -y --quiet && mkdir -p /opt/notebooks && jupyter notebook --notebook-dir=/opt/notebooks --ip=\* --port=8888 --no-browser --allow-root'"

set -A complete_git -- $(git --list-cmds=main)

eval "$(zoxide init posix --cmd cd --hook prompt)"
. "$HOME/.cargo/env"
