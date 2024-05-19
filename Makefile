install:
	stow --verbose --restow --target=$$HOME/.config .config
	mkdir -p $$HOME/.dwm && stow --verbose --restow --target=$$HOME/.dwm .dwm
	ln -sf $$PWD/.kshrc $$HOME/.kshrc
	./install.sh "install"

uninstall:
	./install.sh "uninstall"
	stow --verbose --delete --target=$$HOME/.dwm .dwm
	stow --verbose --delete --target=$$HOME/.config .config
	stow --verbose --delete --target=$$HOME/.kshrc .kshrc

clean:
	./install.sh "clean"

update:
	git pull
	./install.sh "update"
