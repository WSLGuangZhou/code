################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../BISPMem.cpp \
../BISPUtil.cpp \
../DTSCommonLibFunc.cpp \
../DTSDebug.cpp \
../DTSTestCommonLib.cpp \
../DTSTestUnit.cpp \
../FastMalloc.cpp \
../NslProfile.cpp \
../SoftBus.cpp 

OBJS += \
./BISPMem.o \
./BISPUtil.o \
./DTSCommonLibFunc.o \
./DTSDebug.o \
./DTSTestCommonLib.o \
./DTSTestUnit.o \
./FastMalloc.o \
./NslProfile.o \
./SoftBus.o 

CPP_DEPS += \
./BISPMem.d \
./BISPUtil.d \
./DTSCommonLibFunc.d \
./DTSDebug.d \
./DTSTestCommonLib.d \
./DTSTestUnit.d \
./FastMalloc.d \
./NslProfile.d \
./SoftBus.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


