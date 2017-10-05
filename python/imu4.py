import smbus
import time
import subprocess

bus = smbus.SMBus(1)

address = 0x68

bus.write_byte_data(address, 0x6b, 0)
bus.read_byte_data(address, 0x75)

bus.write_byte_data(address, 0x1a, 1)
bus.write_byte_data(address, 0x1b, 16)


def Write_Sensor(reg, val):
    bus.write_byte_data(address, reg, val)

MPU9150_SMPLRT_DIV = 0x19
MPU9150_CONFIG = 0x1a
MPU9150_GYRO_CONFIG = 0x1b
MPU9150_ACCEL_CONFIG = 0x1c
MPU9150_FIFO_EN = 0x23
MPU9150_I2C_MST_CTRL = 0x24
MPU9150_I2C_SLV0_ADDR = 0x25
MPU9150_I2C_SLV0_REG = 0x26
MPU9150_I2C_SLV0_CTRL = 0x27
MPU9150_I2C_SLV1_ADDR = 0x28
MPU9150_I2C_SLV1_REG = 0x29
MPU9150_I2C_SLV1_CTRL = 0x2a
MPU9150_I2C_SLV1_DO = 0x64
MPU9150_I2C_MST_DELAY_CTRL = 0x67
MPU9150_I2C_SLV4_CTRL = 0x34
MPU9150_USER_CTRL = 0x6a

Write_Sensor(MPU9150_SMPLRT_DIV, 0x07);
# Disable FSync, enable 256Hz DLPF
Write_Sensor(MPU9150_CONFIG, 0x00);
# Disable gyro self tests, scale of 500 degrees/s
Write_Sensor(MPU9150_GYRO_CONFIG, 0x08);

# Disable accel self tests, scale of +-2g
# Write_Sensor(MPU9150_ACCEL_CONFIG, 0x00);
#  Arndt: test low pass filter at lower than 250 Hz
Write_Sensor(MPU9150_ACCEL_CONFIG, 0x06);

# Disable sensor output to FIFO buffer
Write_Sensor(MPU9150_FIFO_EN, 0x00);
# Wait for Data at Slave0
Write_Sensor(MPU9150_I2C_MST_CTRL, 0x40);
# Set i2c address of slave0 at 0x8C
Write_Sensor(MPU9150_I2C_SLV0_ADDR, 0x8C);
# Data transfer starts at an internal register within Slave 0.
# Set where reading at slave 0 starts
Write_Sensor(MPU9150_I2C_SLV0_REG, 0x02);
# set offset at start reading and enable
Write_Sensor(MPU9150_I2C_SLV0_CTRL, 0x88);
# set i2c address at slv1 at 0x0C
Write_Sensor(MPU9150_I2C_SLV1_ADDR, 0x0C);
# Set where reading at slave 1 starts
Write_Sensor(MPU9150_I2C_SLV1_REG, 0x0A);
# Enable at set length to 1
Write_Sensor(MPU9150_I2C_SLV1_CTRL, 0x81);
# overvride register
Write_Sensor(MPU9150_I2C_SLV1_DO, 0x01);
# set delay rate
Write_Sensor(MPU9150_I2C_MST_DELAY_CTRL, 0x03);
Write_Sensor(0x01, 0x80);
# set i2c slv4 delay
Write_Sensor(MPU9150_I2C_SLV4_CTRL, 0x04);
# override register
Write_Sensor(MPU9150_I2C_SLV1_DO, 0x00);
# clear usr setting
Write_Sensor(MPU9150_USER_CTRL, 0x00);
# override register
Write_Sensor(MPU9150_I2C_SLV1_DO, 0x01);
# enable master i2c mode
Write_Sensor(MPU9150_USER_CTRL, 0x20);
# disable slv4
Write_Sensor(MPU9150_I2C_SLV4_CTRL, 0x13);

def make_word(high, low):
    x = high*256+low
    if x >= 32768:
        x -= 65536
    return x

marker = "-2"
speed = "-1"

def readgyro():
    while True:
        w = bus.read_i2c_block_data(address, 0x3, 6)
        x = make_word(w[0], w[1])
        y = make_word(w[2], w[3])
        z = make_word(w[4], w[5])

        print("%d %d %d" % (x, y, z))

readgyro()
