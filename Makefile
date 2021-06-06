install: uninstall
	find -mindepth 1 -type f -executable -not -path "./.git/*" -and -not -path "./_archived/*" | tee -a .installed | xargs cp -fu -t /usr/local/bin

uninstall:
	cat .installed | sort -u | xargs -I {} rm -f /usr/local/bin/{}
	rm -f .installed

.PHONY: install uninstall
