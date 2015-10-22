# This file is generated by gyp; do not edit.

TOOLSET := target
TARGET := thread_engine
DEFS_Default := \
	'-DANDROID'

# Flags passed to all source files.
CFLAGS_Default :=

# Flags passed to only C files.
CFLAGS_C_Default :=

# Flags passed to only C++ files.
CFLAGS_CC_Default :=

INCS_Default := \
	-I$(srcdir)/../include

OBJS := \
	$(obj).target/$(TARGET)/thread.o

# Add to the list of files we specially track dependencies for.
all_deps += $(OBJS)

# CFLAGS et al overrides must be target-local.
# See "Target-specific Variable Values" in the GNU Make manual.
$(OBJS): TOOLSET := $(TOOLSET)
$(OBJS): GYP_CFLAGS := $(DEFS_$(BUILDTYPE)) $(INCS_$(BUILDTYPE))  $(CFLAGS_$(BUILDTYPE)) $(CFLAGS_C_$(BUILDTYPE))
$(OBJS): GYP_CXXFLAGS := $(DEFS_$(BUILDTYPE)) $(INCS_$(BUILDTYPE))  $(CFLAGS_$(BUILDTYPE)) $(CFLAGS_CC_$(BUILDTYPE))

# Suffix rules, putting all outputs into $(obj).

$(obj).$(TOOLSET)/$(TARGET)/%.o: $(srcdir)/%.cpp FORCE_DO_CMD
	@$(call do_cmd,cxx,1)

# Try building from generated source, too.

$(obj).$(TOOLSET)/$(TARGET)/%.o: $(obj).$(TOOLSET)/%.cpp FORCE_DO_CMD
	@$(call do_cmd,cxx,1)

$(obj).$(TOOLSET)/$(TARGET)/%.o: $(obj)/%.cpp FORCE_DO_CMD
	@$(call do_cmd,cxx,1)

# End of this set of suffix rules
### Rules for final target.
LDFLAGS_Default :=

LIBS :=

$(obj).target/libthread_engine.so: GYP_LDFLAGS := $(LDFLAGS_$(BUILDTYPE))
$(obj).target/libthread_engine.so: LIBS := $(LIBS)
$(obj).target/libthread_engine.so: LD_INPUTS := $(OBJS)
$(obj).target/libthread_engine.so: TOOLSET := $(TOOLSET)
$(obj).target/libthread_engine.so: $(OBJS) FORCE_DO_CMD
	$(call do_cmd,solink)

all_deps += $(obj).target/libthread_engine.so
# Add target alias
.PHONY: thread_engine
thread_engine: $(builddir)/lib.target/libthread_engine.so

# Copy this to the shared library output path.
$(builddir)/lib.target/libthread_engine.so: TOOLSET := $(TOOLSET)
$(builddir)/lib.target/libthread_engine.so: $(obj).target/libthread_engine.so FORCE_DO_CMD
	$(call do_cmd,copy)

all_deps += $(builddir)/lib.target/libthread_engine.so
# Short alias for building this shared library.
.PHONY: libthread_engine.so
libthread_engine.so: $(obj).target/libthread_engine.so $(builddir)/lib.target/libthread_engine.so

# Add shared library to "all" target.
.PHONY: all
all: $(builddir)/lib.target/libthread_engine.so

