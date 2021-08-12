install: uninstall
	sh -c 'cd src && ./.make install'

uninstall:
	sh -c 'cd src && ./.make uninstall'

.PHONY: install uninstall
