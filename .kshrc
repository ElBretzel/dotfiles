#export PS1="${_GREEN}\h${_BLUE}@${_RED}\u${_RESET} \W${_BOLD} \$ ${_RESET}"

PS1='[\[\e[38;5;255m\]\u\[\e[0m\]@\[\e[38;5;255m\]\h\[\e[0m\]] \W \$ '
PS2='$ '

setxkbmap us -variant altgr-intl
alias volume="wpctl set-volume @DEFAULT_AUDIO_SINK@"
alias ls='eza --color=always --icons=auto --hyperlink -F'
alias l='eza --color=always --icons=auto -F -l -1 --hyperlink -a --long --header --inode'
alias tree='eza --color=always --icons=auto --hyperlink -T'
alias sl='eza --color=always --icons=auto --hyperlink -F'

eval "$(zoxide init posix --cmd cd --hook prompt)"
. "$HOME/.cargo/env"

setxkbmap us -variant altgr-intl
#export TERM=xterm
alias cmus='dbus-launch cmus'
alias void='distrobox enter voidlinux-glibc'
export PATH="$HOME/Programs/zig:$PATH"

set -o emacs
alias __A=$(print '\0020') # ^P = up = previous command
alias __B=$(print '\0016') # ^N = down = next command
alias __C=$(print '\0006') # ^F = right = forward a character
alias __D=$(print '\0002') # ^B = left = back a character
alias __H=$(print '\0001') # ^A = home = beginning of line
