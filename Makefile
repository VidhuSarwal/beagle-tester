prefix := /usr
CC := gcc
CXX := g++
MAKE := make
RM := rm
INSTALL := install

GIT_VERSION := $(shell git describe --abbrev=6 --dirty --always --tags)

# Source file lists
SRC_C := $(wildcard src/*.c)
CLICK_TESTS := $(wildcard click_tests/*.c)
CPP_SRC := src/clickid_detect.cpp src/hdmi_test.cpp

# Header files
INC := $(wildcard include/*.h)

# Compilation flags
CFLAGS := $(CFLAGS_FOR_BUILD) -O3 -W -Wall -Wwrite-strings -I./include
CXXFLAGS := -O3 -Wall -I./include

# Targets
all: beagle-tester clickid_detect hdmi_test

beagle-tester: $(SRC_C) $(CLICK_TESTS) $(INC)
	$(CC) -DVERSION=\"${GIT_VERSION}\" $(CFLAGS) $(SRC_C) $(CLICK_TESTS) -o beagle-tester

clickid_detect: src/clickid_detect.cpp
	$(CXX) $(CXXFLAGS) src/clickid_detect.cpp -o clickid_detect

hdmi_test: src/hdmi_test.cpp
	$(CXX) $(CXXFLAGS) src/hdmi_test.cpp -o hdmi_test `pkg-config --cflags --libs opencv4`

images:
	$(MAKE) -C images

clean:
	$(RM) -f beagle-tester clickid_detect hdmi_test

install:
	$(INSTALL) -m 755 -d $(DESTDIR)$(prefix)/sbin
	$(INSTALL) -m 700 beagle-tester $(DESTDIR)$(prefix)/sbin
	$(INSTALL) -m 700 clickid_detect $(DESTDIR)$(prefix)/sbin
	$(INSTALL) -m 700 hdmi_test $(DESTDIR)$(prefix)/sbin
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
