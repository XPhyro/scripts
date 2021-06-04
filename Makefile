install:
	find -mindepth 1 -type f -executable -not -path "./.git/*" | xargs cp -fu -t /usr/local/bin

uninstall:
	find -mindepth 1 -type f -executable -not -path "./.git/*" | xargs -I {} rm -f /usr/local/bin/{}

.PHONY: install uninstall
