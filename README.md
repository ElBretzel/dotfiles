# void-dotfile
VoidLinux personal and minimal dotfile

# Philosophy:
Follows Unix philosophy: Keep it simple stupid.

# Info

- OS: Void Linux x86_64 (GLibC)
- WM: Dwm
- Term: St
- Bar: Dwm with Slstatus
- Shell: yash
- Editor: Neovim (in my other dotfile repo, heavily bloated as it is my main editor for coding)
- Font: Fira Code Nerd Font

# Dependencies

- libX11-dev
- libXft-dev
- libXinerama-dev
- libXext-dev
- libXrender-dev
- imlib2-dev
- libharfbuzz-dev
- gd-dev
- freetype2-dev
- fontconfig-dev
- xset
- xrandr

# Install

```
git clone https://github.com/ElBretzel/void-dotfile
cd void-dotfile
mv * $HOME/.config/
echo "exec dbus-run-session -- dwm" > $HOME/.xinitrc
```

For VoidLinux base system:
```
xbps-install -Su\
        libX11-devel\
        libXft-devel\
        libXinerama-devel\
        libXext-devel\
        libXrender-devel\
        imlib2-devel\
        harfbuzz-devel\
        gd-devel\
        freetype-devel\
        fontconfig-devel\
        xset\
        xrandr
```

Otherwise check Dependencies

# Prerequisite

Unix based system
X11 (minimal)
dbus
A seat manager (elogind)
[Fira Code Nerd Font](https://www.nerdfonts.com/font-downloads) (I use this font in dwm, slock, st. Can be changed in config header file)
It should work on a musl system be I didn't tested it

# Run

startx

# Display Manager

If you use a display manager like Lightdm, you must create a desktop entry

```
echo "[Desktop Entry]\
Encoding=UTF-8\
Name=dwm\
Comment=Dynamic window Manager\
Exec=dwm\
Icon=dwm\
Type=XSession" > /usr/share/xsessions/dwm.desktop
```

# Credits

st build took from [siduck](https://github.com/siduck/st) (go check his
works)
dwm build look inspired by [siduck chadwm](https://github.com/siduck/chadwm/)
[dwm's colorful tag patch](https://github.com/fitrh/dwm/issues/1) from fitrh

# Patches (so far)

## DWM:

[attach bellow](https://dwm.suckless.org/patches/attachbelow/)
[auto start](https://dwm.suckless.org/patches/autostart/)
[bar padding](https://dwm.suckless.org/patches/barpadding/)
[cfacts](https://dwm.suckless.org/patches/cfacts/)
[colorful tag](https://github.com/fitrh/dwm/issues/1)
[exit menu](https://dwm.suckless.org/patches/exitmenu/)
[fibo layout](https://dwm.suckless.org/patches/fibonacci/)
[focus adjacent](https://dwm.suckless.org/patches/focusadjacenttag/) (to be
deleted)
[hide vacant tag](https://dwm.suckless.org/patches/hide_vacant_tags/)
[pertag](https://dwm.suckless.org/patches/pertag/) (i will probably delete this)
[restartsig](https://dwm.suckless.org/patches/restartsig/)
[status2d](https://dwm.suckless.org/patches/status2d/) (probably will replace
with [status colors](https://dwm.suckless.org/patches/statuscolors/)
[underline tag](https://dwm.suckless.org/patches/underlinetags/)
[vanity gaps](https://dwm.suckless.org/patches/vanitygaps/) (needs cleanup)
[zoomswap](https://dwm.suckless.org/patches/zoomswap/)
And some personal modifications such as bar height and bar workspace dot skin

## ST

st build took from [siduck](https://github.com/siduck/st) (didn't touched)
I like his build right now. When I will have time, I will do some minor tweaking

## Slstatus

No patch but custom made scripts (I tried to not hardcode so it should work on all systems except for the 3 following scripts)
The xbps script only works for VoidLinux base system: it fetches all updatable packages and show the total number.
The connection script works if Network Manager is installed (you can delete or
change the script to remove the connection name's variable if you don't have Network Manager)
The volume script works if pipewire and wireplumber is installed. You can change
this script if you have Pulseaudio or Alsa

## Slock

[auto timeoff](https://tools.suckless.org/slock/patches/auto-timeout/)
[blur pixelated screen](https://tools.suckless.org/slock/patches/blur-pixelated-screen/)
[caps color](https://tools.suckless.org/slock/patches/capscolor/) with personal
modification
[dwm logo](https://tools.suckless.org/slock/patches/dwmlogo/) changed to
VoidLinux logo
[dpms](https://tools.suckless.org/slock/patches/dpms/)

I also modified source of Slock
I plan change dwmlogo patch because it is very painful to create custom logo
