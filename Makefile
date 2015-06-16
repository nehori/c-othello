################################################################
#
# Makefile
#
# Copyright (c) 2015 Kazutaka Yasuda
# Released under the MIT license
# http://opensource.org/licenses/mit-license.php
#
################################################################
#---------------------------------------------------------------
# User setting
#---------------------------------------------------------------

TARGET    = $(shell basename `readlink -f .`)
INCLUDES  = -I./include
LDFLAGS   =
NOMAKEDIR = .git% data% doc% bin%
OBJDIR    = objs

#---------------------------------------------------------------
# Don't change the following
#---------------------------------------------------------------
GCC     = gcc
CFLAGS  = -O2 -MMD -MP -Wall -Wextra -Winit-self -Wno-unused-parameter -Wfloat-equal

CPPS    = $(shell find * -name *.c)
SRCS    = $(filter-out $(NOMAKEDIR), $(CPPS))
DIRS    = $(dir $(SRCS))
BINDIRS = $(addprefix $(OBJDIR)/, $(DIRS))

OBJS    = $(addprefix $(OBJDIR)/, $(patsubst %.c, %.o, $(SRCS)))
DEPS    = $(OBJS:.o=.d)
TILS    = $(patsubst %.c, %.c~, $(SRCS))

ifeq "$(strip $(OBJDIR))" ""
  OBJDIR = .
endif

ifeq "$(strip $(DIRS))" ""
  OBJDIR = .
endif

default:
	@[ -d  $(OBJDIR)   ] || mkdir -p $(OBJDIR)
	@[ -d "$(BINDIRS)" ] || mkdir -p $(BINDIRS)
	@make all --no-print-directory

all : $(OBJS) $(TARGET)

$(TARGET): $(OBJS) $(LIBS)
	$(GCC) -o $@ $^ $(LDFLAGS)

$(OBJDIR)/%.o: %.c
	$(GCC) $(CFLAGS) $(INCLUDES) -o $@ -c $<

clean:
	@rm -rf $(TARGET) $(TILS) $(OBJDIR)

-include $(DEPS)
