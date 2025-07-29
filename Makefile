prefix := /usr
CC := gcc
CXX := g++
MAKE := make
RM := rm
INSTALL := install
WEB_ASSETS := src/web/index.html src/web/style.css src/web/app.js

GIT_VERSION := $(shell git describe --abbrev=6 --dirty --always --tags)

# Source file lists
SRC_C := $(filter-out src/web_server.c src/mongoose.c src/mqtt_publisher.c, $(wildcard src/*.c))
CLICK_TESTS := $(wildcard click_tests/*.c)
CPP_SRC := src/clickid_detect.cpp src/hdmi_test.cpp
WEB_SERVER_SRC := src/web_server.c src/mongoose.c
MQTT_PUBLISHER_SRC := src/mqtt_publisher.c src/config_parser.c src/ini.c

INC := $(wildcard include/*.h)

# Compilation flags
CFLAGS := $(CFLAGS_FOR_BUILD) -O3 -W -Wall -Wwrite-strings -I./include
CXXFLAGS := -O3 -Wall -I./include

MOSQUITTO_LIBS := -lmosquitto
WEB_LIBS := -lpthread

all: beagle-tester clickid_detect hdmi_test web_server mqtt_publisher

beagle-tester: $(SRC_C) $(CLICK_TESTS) $(INC)
	$(CC) -DVERSION=\"${GIT_VERSION}\" $(CFLAGS) $(SRC_C) $(CLICK_TESTS) -o beagle-tester

clickid_detect: src/clickid_detect.cpp
	$(CXX) $(CXXFLAGS) src/clickid_detect.cpp -o clickid_detect

hdmi_test: src/hdmi_test.cpp
	$(CXX) $(CXXFLAGS) src/hdmi_test.cpp -o hdmi_test `pkg-config --cflags --libs opencv4`

web_server: $(WEB_SERVER_SRC)
	$(CC) $(CFLAGS) -o web_server $(WEB_SERVER_SRC) $(WEB_LIBS)

mqtt_publisher: $(MQTT_PUBLISHER_SRC)
	$(CC) $(CFLAGS) -o mqtt_publisher $(MQTT_PUBLISHER_SRC) $(MOSQUITTO_LIBS)

config: beagle_tester.conf

beagle_tester.conf:
	@echo "Creating default configuration file..."
	@echo "[services]" > beagle_tester.conf
	@echo "enable_web = true" >> beagle_tester.conf
	@echo "enable_mqtt = false" >> beagle_tester.conf
	@echo "" >> beagle_tester.conf
	@echo "[mqtt]" >> beagle_tester.conf
	@echo "host = localhost" >> beagle_tester.conf
	@echo "port = 1883" >> beagle_tester.conf
	@echo "topic = beagle/test/results" >> beagle_tester.conf
	@echo "username = " >> beagle_tester.conf
	@echo "password = " >> beagle_tester.conf
	@echo "Default configuration created: beagle_tester.conf"

images:
	$(MAKE) -C images

clean:
	$(RM) -f beagle-tester clickid_detect hdmi_test web_server mqtt_publisher
	$(RM) -f beagle_tester.conf

install: all config
	$(INSTALL) -m 755 -d $(DESTDIR)$(prefix)/sbin
	$(INSTALL) -m 700 beagle-tester $(DESTDIR)$(prefix)/sbin
	$(INSTALL) -m 700 clickid_detect $(DESTDIR)$(prefix)/sbin
	$(INSTALL) -m 700 hdmi_test $(DESTDIR)$(prefix)/sbin
	$(INSTALL) -m 700 web_server $(DESTDIR)$(prefix)/sbin
	$(INSTALL) -m 700 mqtt_publisher $(DESTDIR)$(prefix)/sbin
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

	# Install frontend web assets
	$(INSTALL) -m 755 -d $(DESTDIR)$(prefix)/share/beagle-tester/web
	$(INSTALL) -m 644 $(WEB_ASSETS) $(DESTDIR)$(prefix)/share/beagle-tester/web/

	# Install default configuration file
	$(INSTALL) -m 755 -d $(DESTDIR)/tmp
	$(INSTALL) -m 644 beagle_tester.conf $(DESTDIR)/tmp/beagle_tester.conf

dev-install: all config
	cp beagle-tester /usr/sbin/
	cp web_server /usr/sbin/
	cp mqtt_publisher /usr/sbin/
	cp clickid_detect /usr/sbin/
	cp hdmi_test /usr/sbin/
	cp beagle_tester.conf /tmp/
	mkdir -p /usr/share/beagle-tester/web
	cp $(WEB_ASSETS) /usr/share/beagle-tester/web/

test-mqtt: mqtt_publisher config
	@echo "Testing MQTT publisher..."
	@echo '{"test": "sample", "status": "pass", "timestamp": 1234567890}' > /tmp/test_results.json
	./mqtt_publisher /tmp/test_results.json
	@echo "MQTT test completed"

.PHONY: all clean install start images config dev-install test-mqtt

start: install
	systemctl restart beagle-tester.service
