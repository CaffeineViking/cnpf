config="debug"

all: FORCE
	premake5 gmake
	make -j4 -C build config=${config}
test: FORCE
	premake5 gmake
	make -j4 -C build cnpf-tests config=${config}
	bin/cnpf-tests ${args}
run: FORCE
	premake5 gmake
	make -j4 -C build cnpf config=${config}
	bin/cnpf ${args}

docs: FORCE
	make -C docs
clean: FORCE
	rm -rf build
	rm -rf docs/build
distclean: clean
	rm -rf docs/*.pdf
	rm -rf bin
FORCE:

.PHONY: all test run clean distclean
