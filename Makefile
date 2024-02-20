install:
	stow --verbose --restow --target=$$HOME/.config .config
	./install.sh "install"

uninstall:
	./install.sh "uninstall"
	stow --verbose --delete --target=$$HOME/.config .config

clean:
	./install.sh "clean"

update:
	git pull
	./install.sh "update"
