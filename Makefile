.DEFAULT_GOAL := noop

noop:

install: uninstall
	sh -c 'cd src && ./.make install o=$(o) g=$(g) view=$(view) v=$(v)'

uninstall:
	sh -c 'cd src && ./.make uninstall o=$(o) g=$(g) view=$(view) v=$(v)'

test:
	sh -c 'cd src && ./.make test o=$(o) g=$(g) view=$(view) v=$(v)'

format:
	sh -c 'cd src && ./.make format o=$(o) g=$(g) view=$(view) v=$(v)'

analyse:
	sh -c 'cd src && ./.make analyse o=$(o) g=$(g) view=$(view) v=$(v)'

clean:

.PHONY: install uninstall test format analyse clean
