lib_sources := $(wildcard $(SRC)/lib/*.c)
lib_objects := $(subst $(SRC),$(OBJ),$(lib_sources:.c=.o))
lib_deps    := $(lib_objects:.o=.d)

sources += $(lib_sources)
objects += $(lib_objects)
deps    += $(lib_deps)
