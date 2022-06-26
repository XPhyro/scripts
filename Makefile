.DEFAULT_GOAL := noop

noop:
	@echo "No operation given. Pass 'install' or 'uninstall'."

install: uninstall
	sh -c 'cd src && ./.make install csa=$(csa) o=$(o)'

uninstall:
	sh -c 'cd src && ./.make uninstall'

test:
	sh -c 'cd src && ./.make test'

format:
	sh -c 'cd src && ./.make format'

clean:

.PHONY: install uninstall test format clean
