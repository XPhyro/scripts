install: uninstall
	cd src && find '.' -mindepth 1 -type f -executable -not -path "./.git/*" -and -not -path "./_archived/*" | tee -a ../.installed | xargs -d '\n' -r install -t /usr/local/bin --

uninstall:
	cat .installed | sort -u | xargs -d '\n' -rI {} rm -f /usr/local/bin/{}
	rm -f .installed

.PHONY: install uninstall
