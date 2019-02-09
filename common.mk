# requires: BIN, SRCS

# based on:
# - https://gist.github.com/maxtruxa/4b3929e118914ccef057f8a05c614b0f
# - Makefiles from the Nordic nRF5 SDK

GNU_INSTALL_ROOT := /opt/gcc-arm-none-eabi-8-2018-q4-major
GNU_PREFIX       := arm-none-eabi

CC      := "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-gcc"
CXX     := "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-c++"
AS      := "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-as"
AR      := "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-ar" -r
LD      := "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-ld"
NM      := "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-nm"
OBJDUMP := "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-objdump"
OBJCOPY := "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-objcopy"
SIZE    := "$(GNU_INSTALL_ROOT)/bin/$(GNU_PREFIX)-size"

NRF5_MDK := $(PLAYGROUND_ROOT)/nrf5x_mdk_v8.21.1_gcc
NRF5_SDK := $(PLAYGROUND_ROOT)/nrf5_sdk_v12.3.0
S130_HEX := $(PLAYGROUND_ROOT)/s130_nrf51_2.0.1_softdevice.hex
S130_INCLUDE := $(PLAYGROUND_ROOT)/s130_nrf51_2.0.1_include
SEGGER_RTT := $(PLAYGROUND_ROOT)/segger_rtt_v640b

SRCS := \
	$(NRF5_MDK)/system_nrf51.c $(NRF5_MDK)/gcc_startup_nrf51.S \
	$(SEGGER_RTT)/RTT/SEGGER_RTT.c $(SEGGER_RTT)/RTT/SEGGER_RTT_printf.c \
	$(SRCS)

OUT := _build/$(BIN)

OBJS := $(patsubst %,$(OUT)/%.o,$(basename $(SRCS)))
DEPS := $(patsubst %,$(OUT)/%.d,$(basename $(SRCS)))

$(shell mkdir -p $(dir $(OBJS)) >/dev/null)

CFLAGS := \
	-Wall -Werror \
	-mcpu=cortex-m0 -mabi=aapcs \
	-ffunction-sections -fdata-sections -Os -flto \
	-I $(NRF5_MDK) -I $(S130_INCLUDE) \
	-I $(NRF5_SDK)/components/toolchain/cmsis/include \
	-I $(SEGGER_RTT)/RTT \
	-D NRF51
ASMFLAGS :=
LDFLAGS := \
	-mcpu=cortex-m0 -mabi=aapcs \
	--specs=nano.specs -lc -lnosys \
	-Wl,-Map=$(OUT)/$(BIN).map \
	-Wl,--gc-sections -Os -flto \
	-L $(NRF5_MDK) -T config.ld

DEPFLAGS = -MT $@ -MD -MP -MF $(OUT)/$*.Td

POSTCOMPILE = mv -f $(OUT)/$*.Td $(OUT)/$*.d

all : $(OUT)/$(BIN).hex

.PHONY: flash flash_softdevice clean

flash: $(OUT)/$(BIN).hex
	nrfjprog --program $< -f nrf51 --sectorerase
	nrfjprog --reset -f nrf51

flash_softdevice:
	nrfjprog --program $(S130_HEX) -f nrf51 --sectorerase
	nrfjprog --reset -f nrf51

clean:
	$(RM) -r $(OUT)

$(OUT)/$(BIN).hex : $(OUT)/$(BIN)
	$(OBJCOPY) -O ihex $< $@

$(OUT)/$(BIN) : $(OBJS)
	$(CC) $(LDFLAGS) -o $@ $^

$(OUT)/%.o : %.c
$(OUT)/%.o : %.c $(OUT)/%.d
	$(CC) $(DEPFLAGS) $(CFLAGS) -c -o $@ $<
	$(POSTCOMPILE)

$(OUT)/%.o : %.S
$(OUT)/%.o : %.S $(OUT)/%.d
	$(CC) $(DEPFLAGS) $(ASMFLAGS) -c -o $@ $<
	$(POSTCOMPILE)

.PRECIOUS = $(OUT)/%.d
$(OUT)/%.d : ;

-include $(DEPS)
