apps_sources := $(wildcard $(SRC)/apps/*.c)
apps_apps    := $(subst $(SRC),$(BIN),$(apps_sources:.c=))
apps_objects := $(subst $(SRC),$(OBJ),$(apps_sources:.c=.o))
apps_deps    := $(apps_objects:.o=.d)

apps    += $(apps_apps)
sources += $(apps_sources)
objects += $(apps_objects)
deps    += $(apps_deps)
