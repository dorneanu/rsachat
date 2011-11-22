################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/RSAChat.c \
../src/args.c \
../src/client.c \
../src/db.c \
../src/server.c 

OBJS += \
./src/RSAChat.o \
./src/args.o \
./src/client.o \
./src/db.o \
./src/server.o 

C_DEPS += \
./src/RSAChat.d \
./src/args.d \
./src/client.d \
./src/db.d \
./src/server.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -g -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


