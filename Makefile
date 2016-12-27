CC = arm-elf-gcc
CXX = arm-elf-g++
AS = arm-elf-as
OBJCOPY = arm-elf-objcopy
STRIP = arm-elf-strip
RM = del

THUMB = -mthumb
ARM = -marm

DEVKITARM = c:/devkitARM_r11

CFLAGS = -mthumb-interwork -mlong-calls -O3
CXXFLAGS = -mthumb-interwork -mlong-calls -O3
CPPFLAGS = -I$(DEVKITARM)/include
LDFLAGS = -mthumb-interwork -L$(DEVKITARM)/lib -lm -lgba -lkrawall --specs=gba.specs
ASFLAGS = -mthumb-interwork

ifeq ($(DEBUG), 1)
	TEMPDIR = Debug
else
	TEMPDIR = Release
	CPPFLAGS += -DNDEBUG
endif

OBJS = main.o tune.o samples.o

# REALOBJS = $(foreach var, $(OBJS), $(addprefix $(TEMPDIR)/, $(var)))

# SOURCES = main.cpp math.cpp poly.cpp hline.s
# SOURCES += tune.s samples.s yo.bmp
# OBJS = $(foreach var, $(SOURCES), $(basename $(var)).o)

# OBJS = $(foreach var, $(SOURCES), $(TEMPDIR)/$(basename $(var)).o)
# OBJS = $(foreach var, $(SOURCES), $(addprefix $(TEMPDIR)/, $(basename $(var)).o))

# SOURCES = main.cpp poly.cpp hline.s
# SOURCES += tune.s samples.s yo.bmp
# OBJS = $(foreach var, $(SOURCES), $(addprefix $(TEMPDIR)/, $(var)))

all: $(TEMPDIR) demo.gba

.PHONY: clean
clean:
	$(RM) $(OBJS) demo.elf demo.gba yo.h poly_arm.cpp.s $(OBJS:.o=.d)

$(TEMPDIR):
	@mkdir $(TEMPDIR)

vba: demo.gba
	visualboyadvance demo.gba

demo.elf: $(OBJS)
	@echo linking $@...
	$(CC) $(OBJS) $(LDFLAGS) -o $@
	@$(STRIP) $@

%.gba: %.elf
	@echo making $@...
	@$(OBJCOPY) -O binary $< $@
	@gbafix $@
	
%.bin: %.bmp
	bmp2bin -i $< $@	

%.pal: %.bmp
	bmp2bin -i $< nul $@

%.h: %.pal
	@echo creating $@...
	@echo unsigned short $(basename $(notdir $<))_pal[] = { > $@
	@-sed s/\(.*\)/"\1,"/ < $< >> $@
	@echo }; >> $@

%.o: %.bin
	arm-elf-objcopy -I binary -O elf32-littlearm \
	--rename-section .data=.rodata,readonly,data,contents,alloc \
	--redefine-sym _binary_$*_bin_start=$* \
	--redefine-sym _binary_$*_bin_end=$*_end \
	--redefine-sym _binary_$*_bin_size=$*_size \
	-B arm $< $@

%.c.s: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(THUMB) -S $< -o $@

%_arm.s: %_arm.c
	$(CC) $(CPPFLAGS) $(CFLAGS) $(ARM) -S $< -o $@

%_arm.cpp.s: %_arm.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(ARM) -S $< -o $@

%.cpp.s: %.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(THUMB) -S $< -o $@

%_arm.o: %_arm.c %_arm.d
	@echo compiling $< (arm)...
	@-$(CC) $(CPPFLAGS) $(CFLAGS) $(ARM) -c $< -o $@ 2> $(TMP)\ccerr.txt
	@sed s/"^\([a-zA-Z0-9_.: \\/]\+\):\([0-9]\+\):"/"\1(\2) :"/ < $(TMP)\ccerr.txt

%.o: %.c %.d
	@echo compiling $< (thumb)...
	@-$(CC) $(CPPFLAGS) $(CFLAGS) $(THUMB) -c $< -o $@ 2> $(TMP)\ccerr.txt
	@sed s/"^\([a-zA-Z0-9_.: \\/]\+\):\([0-9]\+\):"/"\1(\2) :"/ < $(TMP)\ccerr.txt

%_arm.o: %_arm.cpp %_arm.d
	@echo compiling $< (arm)...
	@-$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(ARM) -c $< -o $@ 2> $(TMP)\ccerr.txt
	@sed s/"^\([a-zA-Z0-9_.: \\/]\+\):\([0-9]\+\):"/"\1(\2) :"/ < $(TMP)\ccerr.txt

%.o: %.cpp %.d
	@echo compiling $< (thumb)...
	@-$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(THUMB) -c $< -o $@ 2> $(TMP)\ccerr.txt
	@sed s/"^\([a-zA-Z0-9_.: \\/]\+\):\([0-9]\+\):"/"\1(\2) :"/ < $(TMP)\ccerr.txt

%.o: %.s
	@echo compiling $<...
	@-$(AS) $(ASFLAGS) $< -o $@ 2> $(TMP)\ccerr.txt
	@sed s/"^\([a-zA-Z0-9_.: \\/]\+\):\([0-9]\+\):"/"\1(\2) :"/ < $(TMP)\ccerr.txt

%.d: %.c
	@echo generating dependencies for $<...
	@$(CC) -MM $(CPPFLAGS) $< > $@

%.d: %.cpp
	@echo generating dependencies for $<...
	@$(CXX) -MM $(CPPFLAGS) $< > $@

-include $(OBJS:.o=.d)
