GCC=gcc

WPATH=.
OBJPATH=$(WPATH)/objs
TARGETPATH=$(WPATH)/target

DEP_LIB_DIR=$(WPATH)/libs

App_Include_Paths := -I$(WPATH)/include -I$(WPATH)/include/pbc -I$(WPATH)/include/abes -I$(WPATH)/include/sbox

App_C_Files := $(wildcard *.c)

App_C_Flags := $(App_Include_Paths)

# $(shell pkg-config --cflags glib-2.0)
App_C_Flags := $(App_C_Flags) 
# App_C_Objects := $(patsubst %.c, $(OBJPATH)/%.o, $(notdir $(App_C_Files)))
App_C_Objects := $(App_C_Files:.c=.o)

App_Lib_Dir := -L$(DEP_LIB_DIR) -Wl,-rpath=$(DEP_LIB_DIR)
# specify the link path for running time
# $(shell pkg-config --libs glib-2.0)
# $(DEP_LIB_DIR)/libglib-2.0.so
App_Link_Flags := $(App_Lib_Dir) -lgmp -lpbc -lpbc_sig -lpthread -lcrypto $(DEP_LIB_DIR)/libglib.so -labes
App_Objects_A := $(App_C_Objects)
App_Objects_T := $(App_C_Objects:%.o=%.o.t)
App_Objects_L := $(App_C_Objects:%.o=%.o.l)
App_Objects_Com_A := $(patsubst %.o,$(OBJPATH)/%.o,$(notdir $(App_Objects_A)))
App_Objects_Com_T := $(patsubst %.t,$(OBJPATH)/%.t,$(notdir $(App_Objects_T)))
App_Objects_Com_L := $(patsubst %.l,$(OBJPATH)/%.l,$(notdir $(App_Objects_L)))

App_Name := sbox
Test_Name := $(App_Name)_test

Target_Lib := libsboxapi

######################################################3
all: dirs $(App_Name) $(Test_Name) $(Target_Lib).so
libs: dirs $(Target_Lib).so
exe: dirs $(App_Name) $(Test_Name)

dirs:
	@mkdir -p $(OBJPATH)
	@mkdir -p $(TARGETPATH)

$(App_Name): $(App_Objects_A)
	@echo "start: LINK $@  <=  $^" #$< the first linked file; $^ all linked file; $@ target
	$(GCC) -o $(TARGETPATH)/$@ $(App_Objects_Com_A) $(App_Link_Flags)
	@echo "end LINK  $@  <=  $^"
	@echo ""

$(Test_Name): $(App_Objects_T)
	@echo "start: LINK $@  <=  $^"
	$(GCC) -o $(TARGETPATH)/$@ $(App_Objects_Com_T) $(App_Link_Flags)
	@echo "end LINK  $@  <=  $^"
	@echo ""

$(Target_Lib).so: $(App_Objects_L)
	@echo "start: LINK  $@  <=  $^"
	$(GCC) -fPIC -shared -o $(TARGETPATH)/$@ $(App_Objects_Com_L)  $(App_Link_Flags)
	@echo "end LINK  $@  <=  $^"
	@echo ""

%.o: %.c
	@echo "start: $(CC) $@  <=  $<"
	$(GCC) $(App_C_Flags) -fPIC -c $< -o $(OBJPATH)/$(notdir $@) -DEXE
	@echo "end $(CC)  $@  <=  $<"
	@echo ""

%.o.t: %.c
	@echo "start: $(CC) $@  <=  $<"
	$(GCC) $(App_C_Flags) -fPIC -c $< -o $(OBJPATH)/$(notdir $@) -DEXE -DTEST
	@echo "end $(CC)  $@  <=  $<"
	@echo ""

%.o.l: %.c
	@echo "start: $(CC) $@  <=  $<"
	$(GCC) $(App_C_Flags) -fPIC -c $< -o $(OBJPATH)/$(notdir $@)
	@echo "end $(CC)  $@  <=  $<"
	@echo ""

install:
	install -c $(TARGETPATH)/$(App_Name) $(App_Name)
	install -c $(TARGETPATH)/$(Test_Name) $(Test_Name)
	

clean:	
	rm -r $(TARGETPATH) $(OBJPATH) $(App_Name)_test

