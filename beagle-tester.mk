################################################################################
#
# beagle-tester buildroot package
#
################################################################################

BEAGLE_TESTER_VERSION = 1.0

BEAGLE_TESTER_SITE = $(TOPDIR)/../path-to-your-source  # adjust this path
BEAGLE_TESTER_SITE_METHOD = local

BEAGLE_TESTER_DEPENDENCIES = libmosquitto opencv4 pthreads  # depends on Buildroot package names

# Files to install
BEAGLE_TESTER_INSTALL_TARGET = YES

define BEAGLE_TESTER_BUILD_CMDS
    $(MAKE) CC="$(CC)" CXX="$(CXX)" CFLAGS="$(TARGET_CFLAGS)" CXXFLAGS="$(TARGET_CXXFLAGS)" all
endef

define BEAGLE_TESTER_INSTALL_TARGET_CMDS
    $(INSTALL_DIR) $(TARGET_DIR)/usr/sbin
    $(INSTALL) -m 755 beagle-tester $(TARGET_DIR)/usr/sbin/
    $(INSTALL) -m 755 clickid_detect $(TARGET_DIR)/usr/sbin/
    $(INSTALL) -m 755 hdmi_test $(TARGET_DIR)/usr/sbin/
    $(INSTALL) -m 755 web_server $(TARGET_DIR)/usr/sbin/
    $(INSTALL) -m 755 mqtt_publisher $(TARGET_DIR)/usr/sbin/

    # Install scripts (adjust paths and add if they exist)
    $(INSTALL) -m 755 bb-connect-ap $(TARGET_DIR)/usr/sbin/ || true
    $(INSTALL) -m 755 beagle-tester-open.sh $(TARGET_DIR)/usr/sbin/ || true
    $(INSTALL) -m 755 beagle-tester-close.sh $(TARGET_DIR)/usr/sbin/ || true

    # Install systemd service and udev rules
    $(INSTALL_DIR) $(TARGET_DIR)/lib/systemd/system
    $(INSTALL) -m 644 beagle-tester.service $(TARGET_DIR)/lib/systemd/system/
    $(INSTALL_DIR) $(TARGET_DIR)/etc/udev/rules.d
    $(INSTALL) -m 644 beagle-tester.rules $(TARGET_DIR)/etc/udev/rules.d/

    # Install firmware files
    $(INSTALL_DIR) $(TARGET_DIR)/lib/firmware
    $(INSTALL) -m 644 techlab-buzz.out $(TARGET_DIR)/lib/firmware/
    $(INSTALL) -m 644 gamepup-buzz-on-buttons.out $(TARGET_DIR)/lib/firmware/

    # Install web frontend
    $(INSTALL_DIR) $(TARGET_DIR)/usr/share/beagle-tester/web
    $(INSTALL) -m 644 src/web/index.html $(TARGET_DIR)/usr/share/beagle-tester/web/
    $(INSTALL) -m 644 src/web/style.css $(TARGET_DIR)/usr/share/beagle-tester/web/
    $(INSTALL) -m 644 src/web/app.js $(TARGET_DIR)/usr/share/beagle-tester/web/

    # Install config file in /tmp
    $(INSTALL_DIR) $(TARGET_DIR)/tmp
    $(CP) beagle_tester.conf $(TARGET_DIR)/tmp/beagle_tester.conf
endef

$(eval $(generic-package))
