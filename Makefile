prefix := /usr
CC := gcc
CXX := g++                         # ← NEW
MAKE := make
RM := rm
INSTALL := install
GIT_VERSION := $(shell git describe --abbrev=6 --dirty --always --tags)
SRC := $(wildcard src/*.c)
CPP_SRC := src/clickid_detect.cpp # ← NEW
INC := $(wildcard include/*.h)
OBJS := ${patsubst %.c,%.o,${SRC}}
CFLAGS := $(CFLAGS_FOR_BUILD) -O3 -W -Wall -Wwrite-strings -I./include
CXXFLAGS := -O3 -Wall             # ← NEW

all: beagle-tester clickid_detect # ← UPDATED

beagle-tester: $(SRC) $(INC)
	$(CC) -DVERSION=\"${GIT_VERSION}\" $(CFLAGS) $(SRC) -o beagle-tester

clickid_detect: $(CPP_SRC)        # ← NEW
	$(CXX) $(CXXFLAGS) $(CPP_SRC) -o clickid_detect

images:
	$(MAKE) -C images

clean:
	$(RM) -f beagle-tester clickid_detect # ← UPDATED

install:
	$(INSTALL) -m 755 -d $(DESTDIR)$(prefix)/sbin
	$(INSTALL) -m 700 beagle-tester $(DESTDIR)$(prefix)/sbin
	$(INSTALL) -m 700 clickid_detect $(DESTDIR)$(prefix)/sbin  # ← NEW
	$(INSTALL) -m 744 bb-connect-ap $(DESTDIR)$(prefix)/sbin
	$(INSTALL) -m 744 beagle-tester-open.sh $(DESTDIR)$(prefix)/sbin
	$(INSTALL) -m 744 beagle-tester-close.sh $(DESTDIR)$(prefix)/sbin
	$(INSTALL) -m 755 -d $(DESTDIR)/lib/systemd/system
	$(INSTALL) -m 644 beagle-tester.service $(DESTDIR)/lib/systemd/system
	$(INSTALL) -m 755 -d $(DESTDIR)/etc/udev/rules.d
	$(INSTALL) -m 644 beagle-tester.rules $(DESTDIR)/etc/udev/rules.d
	$(INSTALL) -m 755 -d $(DESTDIR)/lib/firmware
	$(INSTALL) -m 644 techlab-buzz.out $(DESTDIR)/lib/firmware
	$(INSTALL) -m 644 gamepup-buzz-on-buttons.out $(DESTDIR)/lib/firmware
	$(MAKE) -C images -s install

start: install
	systemctl restart beagle-tester.service
