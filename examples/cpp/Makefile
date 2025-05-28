DIRS-y := $(shell find . -maxdepth 1 -type d -not -name '.*' -not -name 'build*' -not -path .)

.PHONY: all clean compile_commands $(DIRS-y)

all: $(DIRS-y)
	@:

clean: $(DIRS-y)
	@:

compile_commands: $(DIRS-y)
	@:

$(DIRS-y):
	$(MAKE) -C $@ $(MAKECMDGOALS)
