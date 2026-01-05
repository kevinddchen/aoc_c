tests_sources := $(wildcard $(SRC)/tests/*.c)
tests_apps    := $(subst $(SRC),$(BIN),$(tests_sources:.c=))
tests_objects := $(subst $(SRC),$(OBJ),$(tests_sources:.c=.o))
tests_deps    := $(tests_objects:.o=.d)

apps    += $(tests_apps)
sources += $(tests_sources)
objects += $(tests_objects)
deps    += $(tests_deps)

.PHONY: tests
tests: $(tests_apps)
