.DEFAULT_GOAL := noop

noop:
	@echo "No operation given. Pass 'install' or 'uninstall'."

install: uninstall
	sh -c 'cd src && ./.make install'

uninstall:
	sh -c 'cd src && ./.make uninstall'

.PHONY: install uninstall
