GCC=gcc

WPATH=.
OBJPATH=$(WPATH)/objs
TARGETPATH=$(WPATH)/target

DEP_LIB_DIR=$(WPATH)/libs

App_Include_Paths := -I$(WPATH)/include -I$(WPATH)/include/pbc -I$(WPATH)/include/abecore -I$(WPATH)/include/cpabe -I$(WPATH)/include/sbox

App_C_Files := $(wildcard *.c)

App_C_Flags := $(App_Include_Paths)

# $(shell pkg-config --cflags glib-2.0)
App_C_Flags := $(App_C_Flags) 
# App_C_Objects := $(patsubst %.c, $(OBJPATH)/%.o, $(notdir $(App_C_Files)))
App_C_Objects := $(App_C_Files:.c=.o)

App_Lib_Dir := -L$(DEP_LIB_DIR) -Wl,-rpath=$(DEP_LIB_DIR)
# specify the link path for running time
# $(shell pkg-config --libs glib-2.0)
App_Link_Flags := $(App_Lib_Dir) -lgmp -lpbc -lpbc_sig -lpthread -lcrypto $(DEP_LIB_DIR)/libglib-2.0.so -labecore -lcpabe
App_Link_Flags_1 := ./libs/static/libcpabe.a ./libs/static/libabecore.a ./libs/static/libpbc_sig.a ./libs/static/libpbc.a ./libs/static/libgmp.a ./libs/static/libcrypto.a ./libs/static/libglib-2.0.a ./libs/static/libdl.a ./libs/static/libpthread.a ./libs/static/libc.a 

App_Objects_A := $(App_C_Objects)
App_Objects_L := $(App_C_Objects:%.o=%.o.l)
App_Objects_LA := $(App_C_Objects:%.o=%.o.al)
App_Objects_Com_A := $(patsubst %.o,$(OBJPATH)/%.o,$(notdir $(App_Objects_A)))
App_Objects_Com_L := $(patsubst %.l,$(OBJPATH)/%.l,$(notdir $(App_Objects_L)))
App_Objects_Com_LA := $(patsubst %.al,$(OBJPATH)/%.al,$(notdir $(App_Objects_LA)))

App_Name := sbox

Target_Lib := libsboxapi

######################################################3
all: dirs $(App_Name) $(App_Name)_1 $(Target_Lib).so $(Target_Lib).a
libs: dirs $(Target_Lib).so $(Target_Lib).a
exe: dirs $(App_Name) $(App_Name)_1

dirs:
	@mkdir -p $(OBJPATH)
	@mkdir -p $(TARGETPATH)

$(App_Name): $(App_Objects_A)
	@echo "start: LINK $@  <=  $^" #$< the first linked file; $^ all linked file; $@ target
	$(GCC) -o $(TARGETPATH)/$@ $(App_Objects_Com_A) $(App_Link_Flags)
	@echo "end LINK  $@  <=  $^"
	@echo ""

$(App_Name)_1: $(App_Objects_A)
	@echo "start: LINK $@  <=  $^" #$< the first linked file; $^ all linked file; $@ target
	$(GCC) -o $(TARGETPATH)/$@ -no-pie -flto -static $(App_Objects_Com_A) $(App_Link_Flags_1)
	@echo "end LINK  $@  <=  $^"
	@echo ""

$(Target_Lib).so: $(App_Objects_L)
	@echo "start: LINK  $@  <=  $^"
	$(GCC) -fPIC -shared -o $(TARGETPATH)/$@ $(App_Objects_Com_L)  $(App_Link_Flags)
	@echo "end LINK  $@  <=  $^"
	@echo ""

$(Target_Lib).a: $(App_Objects_LA)
	@echo "start: LINK  $@  <=  $^"
	ar crv $(TARGETPATH)/$@ $(App_Objects_Com_LA)
	@echo "end LINK  $@  <=  $^"
	@echo ""

%.o: %.c
	@echo "start: $(CC) $@  <=  $<"
	$(GCC) $(App_C_Flags) -fPIC -c $< -o $(OBJPATH)/$(notdir $@) -Dsbox_exe $(EXEFLAG)
	@echo "end $(CC)  $@  <=  $<"
	@echo ""

%.o.l: %.c
	@echo "start: $(CC) $@  <=  $<"
	$(GCC) $(App_C_Flags) -fPIC -c $< -o $(OBJPATH)/$(notdir $@)
	@echo "end $(CC)  $@  <=  $<"
	@echo ""

%.o.al: %.c
	@echo "start: $(CC) $@  <=  $<"
	$(GCC) $(App_C_Flags) -c $< -o $(OBJPATH)/$(notdir $@)
	@echo "end $(CC)  $@  <=  $<"
	@echo ""

install:
	install -c $(TARGETPATH)/$(App_Name) $(App_Name)
	install -c $(TARGETPATH)/$(App_Name)_1 $(App_Name)_1
	

clean:	
	rm -r $(TARGETPATH) $(OBJPATH)

