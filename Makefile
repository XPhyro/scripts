install:
	find -mindepth 1 -type f -executable -not -path "./.git/*" -and -not -path "./_archived/*" | xargs cp -fu -t /usr/local/bin

uninstall:
	find -mindepth 1 -type f -executable -not -path "./.git/*" -and -not -path "./_archived/*" | xargs -I {} rm -f /usr/local/bin/{}

.PHONY: install uninstall
