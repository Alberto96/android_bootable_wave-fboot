OutDir := out
ObjDir := out
SrcDir := src

SrcMake := src/Makefile

#Path to ARM toolchain (in this case is included in same folder as this file)
ARM := ARM

CC := $(ARM)/bin/arm-bada-eabi-gcc.exe
AS := $(ARM)/bin/arm-bada-eabi-as.exe
LD := $(ARM)/bin/arm-bada-eabi-ld.exe
RM := $(ARMTK)/bin/rm.exe
OBJCOPY := $(ARM)/bin/arm-bada-eabi-objcopy.exe

#Build only for Wave I (bada 2.0)
Applications := FBOOT_S8500_b2x 
OutputFiles := $(patsubst %, $(OutDir)/%.fota, $(Applications))

application: $(OutputFiles)

rebuild: clean application

include $(SrcMake)