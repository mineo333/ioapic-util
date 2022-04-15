############################ object declaration ############################

MODULE_NAME := ioapic_util

obj-m += $(MODULE_NAME).o

OBJ := ioapic_util.o

$(MODULE_NAME)-y := $(patsubst %.o, src/%.o, $(OBJ))

############################ directories ############################



KDIR := /lib/modules/$(shell uname -r)/build #kernel source

BUILD_DIR := $(PWD)/bin #output directory

############################ options ############################

VICTIM_FILE := /home/mineo333/libc-2.33.so # #change this to change the target file

############################ Kbuild options ############################

CFLAGS_MODULE := -I$(PWD)/include -Wno-declaration-after-statement #this warning clobbers the make

KBUILD_OUTPUT:= $(PWD)/bin


all:
	mkdir -p bin
	@echo $($(MODULE_NAME)-y)
	make -C $(KDIR) M=$(PWD) modules
	mv src/*.o $(MODULE_NAME).mod* modules.order $(MODULE_NAME).o $(BUILD_DIR) #clean up
clean:
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
