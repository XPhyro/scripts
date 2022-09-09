.DEFAULT_GOAL := noop

noop:

install: uninstall
	cd src && ./.make install o=$(o) g=$(g) view=$(view) v=$(v) m=$(m)

uninstall:
	cd src && ./.make uninstall o=$(o) g=$(g) view=$(view) v=$(v) m=$(m)

test:
	cd src && ./.make test o=$(o) g=$(g) view=$(view) v=$(v) m=$(m)

format:
	cd src && ./.make format o=$(o) g=$(g) view=$(view) v=$(v) m=$(m)

analyse:
	cd src && ./.make analyse o=$(o) g=$(g) view=$(view) v=$(v) m=$(m)

spell:
	cd src && ./.make spell o=$(o) g=$(g) view=$(view) v=$(v) m=$(m)

auto: format spell analyse install test

clean:

.PHONY: install uninstall test format analyse auto clean
