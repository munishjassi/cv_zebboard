################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/IP_SW/SobelIP/SW/function/SobelIP_SW.c \
../src/IP_SW/SobelIP/SW/function/SobelIP_func.c 

OBJS += \
./src/IP_SW/SobelIP/SW/function/SobelIP_SW.o \
./src/IP_SW/SobelIP/SW/function/SobelIP_func.o 

C_DEPS += \
./src/IP_SW/SobelIP/SW/function/SobelIP_SW.d \
./src/IP_SW/SobelIP/SW/function/SobelIP_func.d 


# Each subdirectory must supply rules for building sources it contributes
src/IP_SW/SobelIP/SW/function/%.o: ../src/IP_SW/SobelIP/SW/function/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: ARM gcc compiler'
	arm-xilinx-eabi-gcc -Wall -O0 -g3 -I"/nfs/TUEIEDAprojects/SoCOptimization/uzair/architectures/SEG/SDK/SDK_Workspace/SOERGR_sw/src/inc" -c -fmessage-length=0 -I../../standalone_bsp_0/ps7_cortexa9_0/include -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


