.DEFAULT_GOAL := noop

noop:

install: uninstall
	sh -c 'cd src && ./.make install o=$(o) g=$(g) view=$(view)'

uninstall:
	sh -c 'cd src && ./.make uninstall o=$(o) g=$(g) view=$(view)'

test:
	sh -c 'cd src && ./.make test o=$(o) g=$(g) view=$(view)'

format:
	sh -c 'cd src && ./.make format o=$(o) g=$(g) view=$(view)'

analyse:
	sh -c 'cd src && ./.make analyse o=$(o) g=$(g) view=$(view)'

clean:

.PHONY: install uninstall test format analyse clean
