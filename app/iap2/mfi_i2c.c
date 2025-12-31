#include <stdio.h>
#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <sys/ioctl.h>
#include <unistd.h>

#define MFI_I2C_ADDR 0x10 // MFi 3959 默认地址，可能是 0x10 或 0x11

int mfi_i2c_read(int fd, unsigned char reg, unsigned char *buf, int len) {
    // 1. 先写入寄存器地址
    if (write(fd, &reg, 1) != 1) return -1;
    // 2. 读取对应长度的数据
    if (read(fd, buf, len) != len) return -1;
    return 0;
}

int mfi_i2c_write(int fd, unsigned char reg, unsigned char *data, int len) {
    unsigned char tmp[len + 1];
    tmp[0] = reg;
    for (int i = 0; i < len; i++) tmp[i+1] = data[i];
    // 写入寄存器地址 + 数据内容
    if (write(fd, tmp, len + 1) != (len + 1)) return -1;
    return 0;
}

// 测试函数：读取 MFi 芯片版本号
void test_mfi_chip(const char* i2c_bus) {
    int fd = open(i2c_bus, O_RDWR);
    if (fd < 0) return;
    ioctl(fd, I2C_SLAVE, MFI_I2C_ADDR);

    unsigned char ver = 0;
    if (mfi_i2c_read(fd, 0x00, &ver, 1) == 0) {
        printf("MFi 3959 Detected! Version: 0x%02X\n", ver);
    } else {
        printf("MFi Chip not found on %s\n", i2c_bus);
    }
    close(fd);
}
