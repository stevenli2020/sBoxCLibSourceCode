GCC=gcc

WPATH=.
OBJPATH=$(WPATH)/objs
TARGETPATH=$(WPATH)/target

DEP_LIB_DIR=$(WPATH)/libs

App_Include_Paths := -I$(WPATH)/include -I$(WPATH)/include/pbc -I$(WPATH)/include/abecore -I$(WPATH)/include/cpabe

App_C_Files := $(wildcard *.c)

App_C_Flags := $(App_Include_Paths)

# $(shell pkg-config --cflags glib-2.0)
App_C_Flags := $(App_C_Flags) 
# App_C_Objects := $(patsubst %.c, $(OBJPATH)/%.o, $(notdir $(App_C_Files)))
App_C_Objects := $(App_C_Files:.c=.o)

App_Lib_Dir := -L$(DEP_LIB_DIR) -Wl,-rpath=$(DEP_LIB_DIR)
# specify the link path for running time
# $(shell pkg-config --libs glib-2.0)
App_Link_Flags := $(App_Lib_Dir) -lpthread -lgmp -lpbc -lm -lpbc_sig $(DEP_LIB_DIR)/libglib-2.0.so -lssl -lcrypto -labecore -lcpabe

App_Objects_A := $(App_C_Objects)
App_Objects_L := $(App_C_Objects:%.o=%.o.l)
App_Objects_Com_A := $(patsubst %.o,$(OBJPATH)/%.o,$(notdir $(App_Objects_A)))
App_Objects_Com_L := $(patsubst %.l,$(OBJPATH)/%.l,$(notdir $(App_Objects_L)))

App_Name := test

Target_Lib := libtest 

######################################################3
all: dirs $(App_Name) $(Target_Lib)
share: dirs $(Target_Lib)
exe: dirs $(App_Name)

dirs:
	@mkdir -p $(OBJPATH)
	@mkdir -p $(TARGETPATH)

$(App_Name): $(App_Objects_A)
	@echo "start: LINK $@  <=  $^" #$< the first linked file; $^ all linked file; $@ target
	$(GCC) -o $(TARGETPATH)/$@ $(App_Objects_Com_A) $(App_Link_Flags)
	@echo "end LINK  $@  <=  $^"
	@echo ""

$(Target_Lib): $(App_Objects_L)
	@echo "start: LINK  $@  <=  $^"
	$(GCC) -fPIC -shared -o $(TARGETPATH)/$@.so $(App_Objects_Com_L)  $(App_Link_Flags)
	@echo "end LINK  $@  <=  $^"
	@echo ""

%.o: %.c
	@echo "start: $(CC) $@  <=  $<"
	$(GCC) $(App_C_Flags) -c $< -o $(OBJPATH)/$(notdir $@)
	@echo "end $(CC)  $@  <=  $<"
	@echo ""

%.o.l: %.c
	@echo "start: $(CC) $@  <=  $<"
	$(GCC) $(App_C_Flags) -fPIC -c $< -o $(OBJPATH)/$(notdir $@)
	@echo "end $(CC)  $@  <=  $<"
	@echo ""

install:
	install -c $(TARGETPATH)/$(App_Name) $(App_Name)
	

clean:	
	rm -r $(TARGETPATH) $(OBJPATH)

