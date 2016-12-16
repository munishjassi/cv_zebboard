################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/src/axi_interrupt.c \
../src/src/cf_hdmi.c \
../src/src/main.c \
../src/src/transmitter.c \
../src/src/vdma_config.c 

OBJS += \
./src/src/axi_interrupt.o \
./src/src/cf_hdmi.o \
./src/src/main.o \
./src/src/transmitter.o \
./src/src/vdma_config.o 

C_DEPS += \
./src/src/axi_interrupt.d \
./src/src/cf_hdmi.d \
./src/src/main.d \
./src/src/transmitter.d \
./src/src/vdma_config.d 


# Each subdirectory must supply rules for building sources it contributes
src/src/%.o: ../src/src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM gcc compiler'
	arm-xilinx-eabi-gcc -Wall -O0 -g3 -I"/nfs/TUEIEDAprojects/SoCOptimization/uzair/architectures/SEG/SDK/SDK_Workspace/SOERGR_sw/src/inc" -c -fmessage-length=0 -I../../standalone_bsp_0/ps7_cortexa9_0/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


