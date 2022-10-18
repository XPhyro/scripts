define HELP_MESSAGE
Usage: make [OPTION]... [TARGET]...

If no TARGET is given, help is assumed.

     target: description
  =========: ================================================
       help: show this help
    install: install everything
  uninstall: uninstall everything previously installed
       test: run tests
     format: format C/C++ source/header files
    analyse: statically analyse C source files
      spell: check for spelling mistakes
       auto: same as `make format spell analyse install test`
      clean: ignored
endef
export HELP_MESSAGE

help:
	@printf "%s\n" "$$HELP_MESSAGE"

index:
	cd src && ./.make index o=$(o) g=$(g) view=$(view) v=$(v) m=$(m)

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

.PHONY: help install uninstall test format analyse auto clean
