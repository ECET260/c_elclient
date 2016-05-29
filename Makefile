#################################################################################
#                            Very basic Makefile                                #
#                         (C) Marco Vedovati, 2016                              #
#################################################################################

PROJECT_NAME = el_client_demo

#################################################################################
# Note: VPATH is a standard Makefile variable,
# specifying a list of directories that make should search.
VPATH = .

CFLAGS        = -Wall -Werror -c -std=c99
BUILD_OBJ_DIR = build

#################################################################################
#
# Define the C source files by searching in each VPATH element.
C_SRCS  := $(foreach dir,$(VPATH),$(wildcard $(dir)/*.c))


# Define the object files
OBJS := $(addprefix $(BUILD_OBJ_DIR)/,$(notdir $(C_SRCS:.c=.o)))



.PHONY: obj_compile
obj_compile:
	mkdir -p $(BUILD_OBJ_DIR) || exit $$?
	make $(OBJS) || exit $$?


# Compile the C source files.
# NOTE: this is a pattern rule.
$(BUILD_OBJ_DIR)/%.o: %.c
	$(CC) $(CFLAGS) -o $@ $< || exit $$?


$(BUILD_OBJ_DIR)/$(PROJECT_NAME).bin: obj_compile
	$(CC) $(LFLAGS) -o $@ $(OBJS) || exit $$?
	echo "All is done!"


all: $(BUILD_OBJ_DIR)/$(PROJECT_NAME).bin


.PHONY: clean
clean:
	rm -rf $(BUILD_OBJ_DIR) || exit $$?
