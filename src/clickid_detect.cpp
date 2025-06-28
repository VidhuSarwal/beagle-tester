// clickid_detect.cpp
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <string>
#include <vector>

std::string read_click_id(int bus, int addr) {
    char devname[32];
    snprintf(devname, sizeof(devname), "/dev/i2c-%d", bus);
    int fd = open(devname, O_RDWR);
    if (fd < 0) return "";

    if (ioctl(fd, I2C_SLAVE, addr) < 0) {
        close(fd);
        return "";
    }

    char buffer[32] = {0};
    if (read(fd, buffer, sizeof(buffer)) <= 0) {
        close(fd);
        return "";
    }

    close(fd);
    return std::string(buffer);
}

int main() {
    std::vector<int> buses = {2};           // assuming mikroBUS typically uses i2c-2
    std::vector<int> addresses = {0x50};    // EEPROM  address 

    for (int bus : buses) {
        for (int addr : addresses) {
            std::string id = read_click_id(bus, addr);
            if (!id.empty()) {
                std::cout << id << std::endl;
                return 0;
            }
        }
    }

    return 1;
}
