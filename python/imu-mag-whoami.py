import smbus
import time
import subprocess

bus = smbus.SMBus(1)

address = 0x68



def Write_Sensor(reg, val):
    bus.write_byte_data(address, reg, val)

MPU9150_SMPLRT_DIV = 0x19 # 25
MPU9150_CONFIG = 0x1a # 26
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
MPU9150_I2C_MST_DELAY_CTRL = 0x67 # 103
MPU9150_I2C_SLV4_CTRL = 0x34 # 52
MPU9150_USER_CTRL = 0x6a #106

def sleep(x):
    if True:
        time.sleep(x)

def init():

    bus.write_byte_data(address, 0x6b, 0x80)
    sleep(0.1)
    bus.write_byte_data(address, 0x6b, 0)

    b = bus.read_byte_data(address, 0x49)
    print("read byte %#x" % b)

    sleep(0.1)
    Write_Sensor(MPU9150_I2C_SLV0_ADDR, 0x8C);
    b = bus.read_byte_data(address, 0x49)
    print("read byte %#x" % b)

    sleep(0.1)
    Write_Sensor(MPU9150_I2C_SLV0_CTRL, 0x88);
    sleep(0.1)
    b = bus.read_byte_data(address, 0x49)
    print("read byte %#x" % b)

    Write_Sensor(MPU9150_USER_CTRL, 0x20);
    sleep(0.1)

    while True:
        b = bus.read_byte_data(address, 0x49)
        print("read byte %#x" % b)
        if b == 0x48:
            break

    # this did the trick:
    Write_Sensor(MPU9150_CONFIG, 0x02);
    # maybe important:
    Write_Sensor(MPU9150_GYRO_CONFIG, 0x08);
    sleep(0.1)

    Write_Sensor(MPU9150_SMPLRT_DIV, 0x7);
    sleep(0.1)

    Write_Sensor(MPU9150_I2C_SLV1_ADDR, 0x0C);
    sleep(0.1)
    # Set where reading at slave 1 starts
    Write_Sensor(MPU9150_I2C_SLV1_REG, 0x0A);
    sleep(0.1)
    # Enable at set length to 1
    Write_Sensor(MPU9150_I2C_SLV1_CTRL, 0x81);
    sleep(0.1)

    # overvride register
    Write_Sensor(MPU9150_I2C_SLV1_DO, 0x01);
    sleep(0.1)

    # set delay rate
    Write_Sensor(MPU9150_I2C_MST_DELAY_CTRL, 0x03);
    sleep(0.1)
    # set i2c slv4 delay
    Write_Sensor(MPU9150_I2C_SLV4_CTRL, 0x04);
    sleep(0.1)



init()

w0 = None
while True:
    w = bus.read_i2c_block_data(address, 0x4b, 14)
    if w != w0:
        print(w)
    w0 = w

exit(0)

def make_word(high, low):
    x = high*256+low
    if x >= 32768:
        x -= 65536
    return x

def readgyro():
    for i in range(0, 1000000):
        #init()
        if True:
            w = bus.read_i2c_block_data(address, 0x3b, 32)
            #w = bus.read_i2c_block_data(address, 0x3b, 6+2+6+1+6+1)
            print(w[14:])
            x = make_word(w[0], w[1])
            y = make_word(w[2], w[3])
            z = make_word(w[4], w[5])
            p = make_word(w[8], w[9])
            q = make_word(w[10], w[11])
            r = make_word(w[12], w[13])
            k = 2
            mx = make_word(w[15+k], w[16+k])
            my = make_word(w[17+k], w[18+k])
            mz = make_word(w[19+k], w[20+k])

        print("%d %d %d %d %d %d %d %d %d" % (p, q, r, x, y, z, mx, my, mz))

        time.sleep(0.2)

readgyro()

