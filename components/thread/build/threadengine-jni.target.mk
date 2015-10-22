# This file is generated by gyp; do not edit.

TOOLSET := target
TARGET := threadengine-jni
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
	$(obj).target/$(TARGET)/jni/threadengine-jni.o

# Add to the list of files we specially track dependencies for.
all_deps += $(OBJS)

# Make sure our dependencies are built before any of us.
$(OBJS): | $(builddir)/lib.target/libthread_engine.so $(obj).target/libthread_engine.so

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
LDFLAGS_Default := \
	-llog \
	-Wl,-rpath=\$$ORIGIN/lib.target/ \
	-Wl,-rpath-link=\$(builddir)/lib.target/

LIBS :=

$(obj).target/libthreadengine-jni.so: GYP_LDFLAGS := $(LDFLAGS_$(BUILDTYPE))
$(obj).target/libthreadengine-jni.so: LIBS := $(LIBS)
$(obj).target/libthreadengine-jni.so: LD_INPUTS := $(OBJS) $(obj).target/libthread_engine.so
$(obj).target/libthreadengine-jni.so: TOOLSET := $(TOOLSET)
$(obj).target/libthreadengine-jni.so: $(OBJS) $(obj).target/libthread_engine.so FORCE_DO_CMD
	$(call do_cmd,solink)

all_deps += $(obj).target/libthreadengine-jni.so
# Add target alias
.PHONY: threadengine-jni
threadengine-jni: $(builddir)/lib.target/libthreadengine-jni.so

# Copy this to the shared library output path.
$(builddir)/lib.target/libthreadengine-jni.so: TOOLSET := $(TOOLSET)
$(builddir)/lib.target/libthreadengine-jni.so: $(obj).target/libthreadengine-jni.so FORCE_DO_CMD
	$(call do_cmd,copy)

all_deps += $(builddir)/lib.target/libthreadengine-jni.so
# Short alias for building this shared library.
.PHONY: libthreadengine-jni.so
libthreadengine-jni.so: $(obj).target/libthreadengine-jni.so $(builddir)/lib.target/libthreadengine-jni.so

# Add shared library to "all" target.
.PHONY: all
all: $(builddir)/lib.target/libthreadengine-jni.so

