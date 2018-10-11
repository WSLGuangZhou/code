################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../test/DTSVirtualNetManagerFuncCallTestStub.cpp \
../test/DTSVirtualNetManagerTestFunc.cpp \
../test/DTSVirtualNetManagerTestFunc2.cpp \
../test/DTSVirtualNetManagerTestFunc2_1.cpp \
../test/DTSVirtualNetManagerTestFunc3.cpp \
../test/DTSVirtualNetManagerTestFunc4.cpp \
../test/DTSVirtualNetManagerTestFunc5.cpp 

OBJS += \
./test/DTSVirtualNetManagerFuncCallTestStub.o \
./test/DTSVirtualNetManagerTestFunc.o \
./test/DTSVirtualNetManagerTestFunc2.o \
./test/DTSVirtualNetManagerTestFunc2_1.o \
./test/DTSVirtualNetManagerTestFunc3.o \
./test/DTSVirtualNetManagerTestFunc4.o \
./test/DTSVirtualNetManagerTestFunc5.o 

CPP_DEPS += \
./test/DTSVirtualNetManagerFuncCallTestStub.d \
./test/DTSVirtualNetManagerTestFunc.d \
./test/DTSVirtualNetManagerTestFunc2.d \
./test/DTSVirtualNetManagerTestFunc2_1.d \
./test/DTSVirtualNetManagerTestFunc3.d \
./test/DTSVirtualNetManagerTestFunc4.d \
./test/DTSVirtualNetManagerTestFunc5.d 


# Each subdirectory must supply rules for building sources it contributes
test/%.o: ../test/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -I/mnt/hgfs/code/OrganizedNetCtrlServicePrj/src/common -I/mnt/hgfs/code/OrganizedNetCtrlServicePrj/src/baseServiceLayer/thirdProtocolStack/cspf/include -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


