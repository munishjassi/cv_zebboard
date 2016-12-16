################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/axi_interrupt.c \
../src/cf_hdmi.c \
../src/main.c \
../src/sw_functions.c \
../src/transmitter.c 

LD_SRCS += \
../src/lscript.ld 

OBJS += \
./src/axi_interrupt.o \
./src/cf_hdmi.o \
./src/main.o \
./src/sw_functions.o \
./src/transmitter.o 

C_DEPS += \
./src/axi_interrupt.d \
./src/cf_hdmi.d \
./src/main.d \
./src/sw_functions.d \
./src/transmitter.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM gcc compiler'
	arm-xilinx-eabi-gcc -Wall -O0 -g3 -I"/home/ga65cur/Documents/Development/Master_Thesis/Projects/Comp_v2/videoProcessing_prj/SDK/SDK_Workspace/ce_test/inc" -c -fmessage-length=0 -I../../standalone_bsp_0/ps7_cortexa9_0/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


