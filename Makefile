
all:
	stow --verbose --restow --target=$$HOME/.config .config

clean:
	stow --verbose --delete --target=$$HOME/.config .config
