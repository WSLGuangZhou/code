################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../DTSVirtualNetManager.cpp \
../DTSVirtualNetManagerFuncCall.cpp \
../DTSVirtualNetManagerInterface.cpp \
../DTSVirtualNetManagerPath.cpp 

OBJS += \
./DTSVirtualNetManager.o \
./DTSVirtualNetManagerFuncCall.o \
./DTSVirtualNetManagerInterface.o \
./DTSVirtualNetManagerPath.o 

CPP_DEPS += \
./DTSVirtualNetManager.d \
./DTSVirtualNetManagerFuncCall.d \
./DTSVirtualNetManagerInterface.d \
./DTSVirtualNetManagerPath.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/mnt/hgfs/code/OrganizedNetCtrlServicePrj/src/common -I/mnt/hgfs/code/OrganizedNetCtrlServicePrj/src/baseServiceLayer/thirdProtocolStack/cspf/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


