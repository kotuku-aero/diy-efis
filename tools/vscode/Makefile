# build a code extension

all: vsix
	@echo "vsix created"

vsix: build
	node_modules\.bin\vsce.cmd package

publish:	
	node_modules\.bin\vsce.cmd publish

build:
	node_modules\.bin\tsc.cmd -p src
	@echo "build finished"


clean:
	git clean -xfd