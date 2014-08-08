#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>

/* this is the i2c-dev.h file from the i2c-tools package *NOT* the
 * linux kernel include.  This include defines the i2c_smbus_*
 * functions */
#include <i2c-dev.h>

/* define the i2c slave address */
#define I2C_GPIO_ADDR 0x3E

#define REG_INPUT   0x00
#define REG_OUTPUT  0x01
#define REG_INVERT  0x02
#define REG_CONFIG  0x03

int gpio_i2c_reg_wr(int fd, __u8 reg, __u8 val)
{
    /* write to the specified register of i2c port expander */
    if(i2c_smbus_write_byte_data(fd, reg, val) < 0)
        fprintf( stderr, "failed to write to slave %d\n",errno );
        return 2;

    return 0;
}

__s32 gpio_i2c_reg_rd(int fd, __u8 reg)
{
    __s32 reg_out;

    //Read register
    reg_out = i2c_smbus_read_byte_data(fd, reg);
    if(reg_out < 0) {
        fprintf( stderr, "failed to read from slave \n" );
        return -1;
    }

    return reg_out;
}


int gpio_config_port(int addr, __u8 v)
{
    int fd;

    /* open the i2c adapter */
    if((fd = open( "/dev/i2c-1", O_RDWR )) == -1) {
        fprintf( stderr, "Failed to open /dev/i2c-1\n" );
        return 2;
    }

    /* set the slave device address we want to talk to */
    if( ioctl( fd, I2C_SLAVE, addr ) < 0 ) {
        fprintf( stderr, "Failed to set slave address: %d\n", addr);
        close(fd);
        return 2;
    }

    if(0!=gpio_i2c_reg_wr(fd, REG_CONFIG, v)) {
        close(fd);
        return 2;
    }

    close(fd);
    return 0;
}

int gpio_config_pin(int addr, __u8 pin, __u8 dir)
{
    int fd;
    __s32 reg_out;

    // Check for valid pin or value.
    if(pin > 7) return 2; //error pin out of range.
    if(dir > 1) return 2;  //error value out of range.

    /* open the i2c adapter */
    if((fd = open( "/dev/i2c-1", O_RDWR )) == -1) {
        fprintf( stderr, "Failed to open /dev/i2c-1\n" );
        return 2;
    }

    /* set the slave device address we want to talk to */
    if( ioctl( fd, I2C_SLAVE, addr ) < 0 ) {
        fprintf( stderr, "Failed to set slave address: %d\n", addr);
        close(fd);
        return 2;
    }

    //Read config register
    reg_out = gpio_i2c_reg_rd(fd, REG_CONFIG);
    if(reg_out < 0) {
        close(fd);
        return 2;
    }

    //set or clear the pin based on v.
    reg_out = (0!=dir)?(reg_out |= (dir<<pin)):(reg_out &= ~(0x01<<pin));

    if(0!=gpio_i2c_reg_wr(fd, REG_CONFIG, (__u8)reg_out)) {
        close(fd);
        return 2;
    }

    /* clean up */
    close(fd);

    return 0;
}

int gpio_wr_port(int addr, __u8 v)
{
    int fd;

    /* open the i2c adapter */
    if((fd = open( "/dev/i2c-1", O_RDWR )) == -1) {
        fprintf( stderr, "Failed to open /dev/i2c-1\n" );
        return 2;
    }

    /* set the slave device address we want to talk to */
    if( ioctl( fd, I2C_SLAVE, addr ) < 0 ) {
        fprintf( stderr, "Failed to set slave address: %d\n", addr);
        close(fd);
        return 2;
    }

    if(0!=gpio_i2c_reg_wr(fd, REG_OUTPUT, v)) {
        close(fd);
        return 2;
    }

    /* clean up */
    close(fd);

    return 0;

}

int gpio_wr_pin(int addr, __u8 pin, __u8 v)
{
    int fd;
    __s32 reg_out;

    // Check for valid pin or value.
    if(pin > 7) return 2; //error pin out of range.
    if(v > 1) return 2;  //error value out of range.

    /* open the i2c adapter */
    if((fd = open( "/dev/i2c-1", O_RDWR )) == -1) {
        fprintf( stderr, "Failed to open /dev/i2c-1\n" );
        return 2;
    }

    /* set the slave device address we want to talk to */
    if( ioctl( fd, I2C_SLAVE, addr ) < 0 ) {
        fprintf( stderr, "Failed to set slave address: %d\n", addr);
        close(fd);
        return 2;
    }

    //Read output register.
    reg_out = gpio_i2c_reg_rd(fd, REG_OUTPUT);
    if(reg_out < 0) {
        close(fd);
        return 2;
    }

    //set or clear the pin based on v.
    reg_out = (0!=v)?(reg_out |= (v<<pin)):(reg_out &= ~(0x01<<pin));

    if(0!=gpio_i2c_reg_wr(fd, REG_OUTPUT, (__u8)reg_out)) {
        close(fd);
        return 2;
    }

    /* clean up */
    close(fd);

    return 0;
}

__s32 gpio_rd_reg(int addr, __u8 reg)
{
    int fd;
    __s32 ret;

    /* open the i2c adapter */
    if((fd = open( "/dev/i2c-1", O_RDWR )) == -1) {
        fprintf( stderr, "Failed to open /dev/i2c-1\n" );
        return -1;
    }

    /* set the slave device address we want to talk to */
    if( ioctl( fd, I2C_SLAVE, addr ) < 0 ) {
        fprintf( stderr, "Failed to set slave address: %d\n", addr);
        close(fd);
        return -1;
    }

    ret = gpio_i2c_reg_rd(fd, reg);
    if(ret<0) {
        close(fd);
        return -1;
    }

    /* clean up */
    close(fd);

    return ret;

}

__s32 gpio_rd_reg_bit(int addr, __u8 reg, __u8 pin)
{
    int fd;
    __s32 ret;

    /* open the i2c adapter */
    if((fd = open( "/dev/i2c-1", O_RDWR )) == -1) {
        fprintf( stderr, "Failed to open /dev/i2c-1\n" );
        return -1;
    }

    /* set the slave device address we want to talk to */
    if( ioctl( fd, I2C_SLAVE, addr ) < 0 ) {
        fprintf( stderr, "Failed to set slave address: %d\n", addr);
        close(fd);
        return -1;
    }

    ret = gpio_i2c_reg_rd(fd, reg);
    if(ret<0) {
        close(fd);
        return -1;
    }

    ret = (ret>>pin)&0x01;

    /* clean up */
    close(fd);

    return ret;

}

int  parse_options(int argc, char *argv[]);
void show_help(char *name);

int main(int argc, char* argv[])
{
    int i;

    // Parse command line options.
    if (parse_options(argc, argv) != 0) {
        show_help(argv[0]);
    }

    return 0;
}


int parse_options(int argc, char *argv[]) {
    int c;
    int pin = -1;
    int dir = -1;
    int rval = -1;
    int wval = -1;
    __s32 r;

    while((c = getopt(argc, argv, "d:w:p:?r")) != -1) {
        switch(c) {
            case 'd':
                dir = strtoul(optarg, NULL, 0);
                if(dir > 0xFF) {
                    dir = -1;
                }
                break;

            case 'r':
                rval = 1;
                break;
            case 'w':
                wval = strtoul(optarg, NULL, 0);
                if(wval > 0xFF) {
                    wval = -1;
                }
                break;
            case 'p':
                pin = strtoul(optarg, NULL, 0);
                if(pin > 7) {
                    pin = -1;
                }
                break;
            case '?':
                show_help(argv[0]);
                break;
        }
    }

    // Check if they want to set port or pin.
    if(pin != -1)
    {
        //First set the output port if requested.
        if(wval != -1) {
            gpio_wr_pin(I2C_GPIO_ADDR, pin, wval);
            r=gpio_rd_reg(I2C_GPIO_ADDR, REG_OUTPUT);
            printf("Output Register = 0x%02x\n", r);
        }

        //Configure direction if requested.
        if(dir != -1) {
            gpio_config_pin(I2C_GPIO_ADDR, pin, dir);
            r=gpio_rd_reg(I2C_GPIO_ADDR, REG_CONFIG);
            printf("Config Register = 0x%02x\n", r);
        }

        //Read the input register if requested.
        if(rval != -1) {
            r=gpio_rd_reg_bit(I2C_GPIO_ADDR, REG_INPUT, pin);
            printf("Input on pin %d = %1d\n", pin, r);
            //r=gpio_rd_reg(I2C_GPIO_ADDR, REG_INPUT);
            //printf("Input Register = 0x%02x\n", r);
        }

    } else { // Its a port.
        //First set the output port if requested.
        if(wval != -1) {
            gpio_wr_port(I2C_GPIO_ADDR, wval);
            r=gpio_rd_reg(I2C_GPIO_ADDR, REG_OUTPUT);
            printf("Output Register = 0x%02x\n", r);

        }

        //Configure direction if requested.
        if(dir != -1) {
            gpio_config_port(I2C_GPIO_ADDR, dir);
            r=gpio_rd_reg(I2C_GPIO_ADDR, REG_CONFIG);
            printf("Config Register = 0x%02x\n", r);

        }

        //Read the input register if requested.
        if(rval != -1) {
            r=gpio_rd_reg(I2C_GPIO_ADDR, REG_INPUT);
            printf("Input Register = 0x%02x\n", r);
        }
    }


#if 0
    for (c = optind; c < argc; ++c) {
        if (device) {
            fprintf(stderr, "ERROR: Invalid parameter specified\n");
            show_help(argv[0]);
            return 1;
        }
        device = argv[c];
    }
#endif

    return 0;
}

void show_help(char *name) {
    fprintf(stderr,
        "Usage: %s [-dwrp] \n"
        "	-d direction  set the direction of the pin\n"
        "	-w write      write value to gpio port/pin.\n"
        "	-r read       read input register value.\n"
        "	-p pin        0-7 for pin.\n"
        "\n"
        "Examples:\n"
        "	Set pin 0 high:\n"
        "		%s -p 0 -w 1\n"
        "\n"
        "	Configure pin 0 as output:\n"
        "		%s -p 0 -d 0\n"
        "\n"
        "	Set GPIO port:\n"
        "		%s -w \n"
        "\n",
        name,
        name,
        name,
        name
    );
}

