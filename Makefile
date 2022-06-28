.DEFAULT_GOAL := noop

noop:

install: uninstall
	sh -c 'cd src && ./.make install o=$(o) g=$(g) view=$(view) v=$(v) m=$(m)'

uninstall:
	sh -c 'cd src && ./.make uninstall o=$(o) g=$(g) view=$(view) v=$(v) m=$(m)'

test:
	sh -c 'cd src && ./.make test o=$(o) g=$(g) view=$(view) v=$(v) m=$(m)'

format:
	sh -c 'cd src && ./.make format o=$(o) g=$(g) view=$(view) v=$(v) m=$(m)'

analyse:
	sh -c 'cd src && ./.make analyse o=$(o) g=$(g) view=$(view) v=$(v) m=$(m)'

clean:

.PHONY: install uninstall test format analyse clean
