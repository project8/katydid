#####D. Furse's amazing makefile!#####

#---------- build options -----------
# use "yes" to include lice. anything other than that will deactivate the use of lice
USE_LICE := no


#---------- definitions of compiler, suffixes, and options ----------

# Use ROOT's configuration
ROOTCONFIG   := root-config

ARCH         := $(shell $(ROOTCONFIG) --arch)
PLATFORM     := $(shell $(ROOTCONFIG) --platform)
ALTCXX       := $(shell $(ROOTCONFIG) --cxx)
ALTCXXFLAGS	 := $(shell $(ROOTCONFIG) --auxcflags)
ALTLD        := $(shell $(ROOTCONFIG) --ld)
ALTLDFLAGS   := $(shell $(ROOTCONFIG) --ldflags)

ifeq ($(ARCH),macosx64)
# MacOS X >= 10.4 with gcc 64 bit mode (GNU gcc 4.*)
# Only specific option (-m64) comes from root-config
MACOSX_MINOR := $(shell sw_vers | sed -n 's/ProductVersion://p' | cut -d . -f 2)
MACOSXTARGET := MACOSX_DEPLOYMENT_TARGET=10.$(MACOSX_MINOR)
CXX           = g++
CXXFLAGS      = -g -01 -pipe -Wall -W -Woverloaded-virtual
LD            = $(MACOSXTARGET) g++ -m64

ifdef USE_MPI
CXX           = mpic++
CXXFLAGS   = -g -O1 -pipe -W -Wall -Woverloaded-virtual -Wno-unused-variable -DUSE_MPI
LD            = $(MACOSXTARGET) mpic++
endif

LDFLAGS       = -g -O1 -bind_at_load
# The SOFLAGS will be used to create the .dylib,
# the .so will be created separately
ifeq ($(subst $(MACOSX_MINOR),,1234),1234)
DllSuf        = so
else
DllSuf        = dylib
endif
SOFLAGS       = -m64 -dynamiclib -single_module -undefined dynamic_lookup
endif

ifeq ($(ARCH),macosx)
# MacOS X with cc (GNU cc 2.95.2 and gcc 3.3)
MACOSX_MINOR := $(shell sw_vers | sed -n 's/ProductVersion://p' | cut -d . -f 2)
MACOSXTARGET := MACOSX_DEPLOYMENT_TARGET=10.$(MACOSX_MINOR)
CXX           = g++
CXXFLAGS      = -g -O1 -pipe -Wall -W -Woverloaded-virtual
LD            = $(MACOSXTARGET) g++

ifdef USE_MPI
CXX           = mpic++
CXXFLAGS   = -g -O1 -pipe -W -Wall -Woverloaded-virtual -Wno-unused-variable -DUSE_MPI
LD            = $(MACOSXTARGET) mpic++
endif

LDFLAGS       = -g -O1 -bind_at_load
# The SOFLAGS will be used to create the .dylib,
# the .so will be created separately
ifeq ($(subst $(MACOSX_MINOR),,1234),1234)
DllSuf        = so
else
DllSuf        = dylib
endif
UNDEFOPT      = dynamic_lookup
ifneq ($(subst $(MACOSX_MINOR),,12),12)
UNDEFOPT      = suppress
LD            = g++
endif
SOFLAGS       = -dynamiclib -single_module -undefined $(UNDEFOPT)
endif

#ifeq ($(ARCH),linux)
# Linux with egcs, gcc 2.9x, gcc 3.x
#CXX           = g++
#CXXFLAGS      = -g -O1 -Wall -Wno-unused-variable -fPIC 
#LD            = g++
#LDFLAGS       = -g -O1 
#SOFLAGS       = -shared
#ifdef USE_MPI
#CXX           = mpic++
#CXXFLAGS   = -g -O1 -Wall -WfPIC -Wno-unused-variable -DUSE_MPI
#LD            = mpic++
#endif
#endif


# for linux (root-config picks up the 64-bit specific flags)
ifneq (,$(findstring linux, $(ARCH)))
CXX           = $(ALTCXX)
CXXFLAGS      = -g -Wall $(ALTCXXFLAGS) -fPIC -O2 
LD            = $(ALTLD)
LDFLAGS       = $(ALTLDFLAGS) -O2 -g
SOFLAGS       = -shared

endif



ifndef USE_MPI
ifneq ($(ALTCXX),)
CXX = $(ALTCXX)
endif
ifneq ($(ALTLD),)
LD  = $(ALTLD)
endif
endif

# Define Suffixes
ObjSuf = o
SrcSuf = cc
HdrSuf = hh
ExeSuf =
DllSuf = so
OutPutOpt = -o 

#---------- definitions of paths and libraries for Project 8 packages -----------

# Paths and Libraries for lice
ifeq ($(USE_LICE), yes)
LICECFLAGS   := -I../lice
LICELIBS     := -L../lice -llice
else
LICECFLAGS   := 
LICELIBS     := 
endif

#---------- definitions of paths and libraries for external packages ----------

# Paths and Libraries for ROOT
ROOTCFLAGS   := $(shell $(ROOTCONFIG) --cflags)
ROOTLDFLAGS  := $(shell $(ROOTCONFIG) --ldflags)
ROOTLIBS     := $(shell $(ROOTCONFIG) --libs) -lFFTW
ROOTGLIBS    := $(shell $(ROOTCONFIG) --glibs)

# Paths and Libraries for GSL
#GSLCONFIG := gsl-config
#GSLCFLAGS := $(shell $(GSLCONFIG) --cflags)
#GSLLIBS   := $(shell $(GSLCONFIG) --libs)

# Paths and Libraries for Boost
#BOOSTFLAGS := -I/usr/local/include/boost
#BOOSTFLAGS := -I$(BOOSTDIR)
#BOOSTLIBS := -lboost_filesystem -lboost_system

#$(shell echo ROOTLIBS is --$(ROOTLIBS)-- 1>&2)

# Paths and Libraries for Mini-XML
MXMLCFLAGS := 
MXMLLIBS := -lmxml


CXXFLAGS     += $(ROOTCFLAGS) $(MXMLCFLAGS) $(LICECFLAGS) #$(GSLCFLAGS) $(BOOSTFLAGS) -I.
LDFLAGS      += $(ROOTLDFLAGS)
LIBS          = $(ROOTLIBS) $(MXMLLIBS) $(LICELIBS) #$(GSLLIBS) $(BOOSTLIBS)

#---------- Program sections and variable settings ----------

#Define the sections of the program which get their own libraries and their variable settings

SRCDIR   = Source
OBJDIR   = tmp
INCDIR   = 
BINDIR   = bin
LIBDIR   = lib
LINKDEF  = LinkDef

DIRSECTIONS  = $(OBJDIR) $(LIBDIR) $(BINDIR)
INCSECTIONS  = Egg
LIBSECTIONS  = Egg
DICTSECTIONS = Egg
EXESECTIONS  = $(notdir $(basename $(wildcard $(SRCDIR)/Main/*.$(SrcSuf))))

#LDFLAGS += -rpath $(PWD)/$(LIBDIR)

$(foreach SECT,$(INCSECTIONS),$(eval CXXFLAGS += -I./$$(SRCDIR)/$(SECT)/$$(INCDIR)))
TRANSLIBS        = -L./$(LIBDIR)
$(foreach SECT,$(LIBSECTIONS),$(eval TRANSLIBS += -l$(SECT)))

#---------- actual make 'rules' ----------

#macro to define rules for each section of the program
.PHONY: all
all: $(DIRSECTIONS) $(foreach SECT,$(DICTSECTIONS),$(addsuffix _dict,$(SECT))) $(LIBSECTIONS) $(EXESECTIONS)

define DICTSECTION_RULE
.PHONY: $(1)_dict
ifdef USE_MPI
$(1)DICTOBJ_MPI = $$(addprefix $$(OBJDIR)/,$$(addsuffix Dict_MPI.$$(ObjSuf),$(1)))
endif
$(1)DICTOBJ = $$(addprefix $$(OBJDIR)/,$$(addsuffix Dict.$$(ObjSuf),$(1)))
$(1)_dict: $$(addprefix $$(SRCDIR)/$(1)/$$(LINKDEF)/,$$(addsuffix Dict.$$(SrcSuf),$(1)))
endef

define LIBSECTION_RULE
.PHONY: $(1)
$(1): $(OBJDIR) $(LIBDIR) $$(PWD)/$$(LIBDIR)/lib$(1).$$(DllSuf)
endef

define EXESECTION_RULE
.PHONY: $(1)
$(1): $(BINDIR) $(LIBSECTIONS) $$(addsuffix $$(ExeSuf),$$(addprefix $$(BINDIR)/,$(1)))
endef

$(foreach SECT,$(DICTSECTIONS),$(eval $(call DICTSECTION_RULE,$(SECT))))

$(foreach SECT,$(LIBSECTIONS),$(eval $(call LIBSECTION_RULE,$(SECT))))

$(foreach SECT,$(EXESECTIONS),$(eval $(call EXESECTION_RULE,$(SECT))))

#macro to define rules for creating a directory
define DIRECTORY_RULE
$(1) : 
ifneq (exists, $$(shell [ -d $(1)/ ] ) && echo exists )
	@echo Creating directory $$@...
	@mkdir $(1)
endif
endef

$(foreach SECT,$(DIRSECTIONS),$(eval $(call DIRECTORY_RULE,$(SECT))))


#macro to define rules for creating a section's dictionary
define DICTIONARY_RULE
$$(SRCDIR)/$(1)/$$(LINKDEF)/$(1)Dict.$$(SrcSuf) : $$(wildcard $$(SRCDIR)/$(1)/*.$$(HdrSuf)) $$(SRCDIR)/$(1)/$$(LINKDEF)/$(1)LinkDef.$$(HdrSuf)
	@echo Generating dictionary $$@...
	@rootcint -f $$@ -c -I$$(SRCDIR)/Egg $$^
endef

$(foreach SECT,$(DICTSECTIONS),$(eval $(call DICTIONARY_RULE,$(SECT))))



#macro to define rules for creating a section's library
define LIBRARY_RULE
$$(PWD)/$$(LIBDIR)/lib$(1).$$(DllSuf) : $$(addprefix $$(OBJDIR)/,$$(addsuffix .$$(ObjSuf),$$(notdir $$(basename $$(wildcard $$(SRCDIR)/$(1)/*$$(SrcSuf)))))) $$($(1)DICTOBJ)
ifeq ($$(ARCH),aix)
		@/usr/ibmcxx/bin/makeC++SharedLib $$(OutPutOpt) $$@ $$(LIBS) -p 0 $$^
else
ifeq ($$(ARCH),aix5)
		@/usr/vacpp/bin/makeC++SharedLib $$(OutPutOpt) $$@ $$(LIBS) -p 0 $$^
else
ifeq ($$(PLATFORM),macosx)
	@echo -n creating library $$@...
# We need to make both the .dylib and the .so
		@$$(LD) $$(LDFLAGS) $$(SOFLAGS) $$^ $$(LIBS) $$(OutPutOpt) $$@
ifneq ($$(subst $$(MACOSX_MINOR),,1234),1234)
ifeq ($$(MACOSX_MINOR),4)
		@ln -sf $$@ $(subst .$$(DllSuf),.so,$$@)
else
		@$$(LD) -bundle -undefined $$(UNDEFOPT) $$(LDFLAGS) $$^ $$(LIBS) \
		   $$(OutPutOpt) $$(subst .$$(DllSuf),.so,$$@)
endif
endif
else
ifeq ($$(PLATFORM),win32)
		bindexplib $$* $$^ > $$*.def
		lib -nologo -MACHINE:IX86 $$^ -def:$$*.def \
		   $$(OutPutOpt)$$(LIBDIR)/lib$(1).$$(DllSuf)
		$$(LD) $$(SOFLAGS) $$(LDFLAGS) $$^ $$*.exp $$(LIBS) \
		   $$(OutPutOpt)$$@
		$$(MT_DLL)
else
		@$$(LD) $$(SOFLAGS) $$(LDFLAGS) $$^ $$(OutPutOpt) $$@ $$(EXPLLINKLIBS)
endif
endif
endif
endif
		@echo " done."

$$(OBJDIR)/%.$$(ObjSuf) : $$(SRCDIR)/$(1)/%.$$(SrcSuf)
	@echo compiling $$@...
	@$$(CXX) $$(CXXFLAGS) -c $$< -o $$@
	
$$(OBJDIR)/%.$$(ObjSuf) : $$(SRCDIR)/$(1)/LinkDef/%.$$(SrcSuf)
	@echo compiling $$@...
	@$$(CXX) $$(CXXFLAGS) -I. -c $$< -o $$@

endef


$(foreach SECT,$(LIBSECTIONS),$(eval $(call LIBRARY_RULE,$(SECT))))

#macro to define rules for creating a section's executable
define EXECUTABLE_RULE
$$(BINDIR)/$(1)$$(ExeSuf) : $$(addprefix $$(OBJDIR)/,$$(addsuffix .$(ObjSuf),$(1)))
	@echo -n creating executable $$@...
	@$$(LD) $$(LDFLAGS) $$(LIBS) $$(TRANSLIBS) $$^  -o $$@
	@echo " done."
endef

$(foreach SECT,$(EXESECTIONS),$(eval $(call EXECUTABLE_RULE,$(SECT))))

$(OBJDIR)/%.$(ObjSuf) : $(SRCDIR)/Main/%.$(SrcSuf)
	@echo compiling $@
	@$(CXX) $(CXXFLAGS) -c $< -o $@

#rules for cleaning up
.PHONY:clean distclean


clean:
	@echo cleaning up...
	@rm -f $(OBJDIR)/*.$(ObjSuf) $(BINDIR)/*
	@rm -f $(LIBDIR)/*
	$(foreach SECT,$(DICTSECTIONS), \
	@rm -f $(SRCDIR)/$(SECT)/$(LINKDEF)/*Dict*)

distclean: clean
	@echo deleting temporary folders...
	@rm -rf $(OBJDIR)
	@rm -rf $(LIBDIR)
	@rm -rf $(BINDIR)
	@echo ...done.
