#include "driver/i2c_master.h"

#define	AMG8833_PCLT  0x00
#define	AMG8833_RST   0x01
#define	AMG8833_FPSC  0x02
#define	AMG8833_INTC  0x03
#define	AMG8833_STAT  0x04
#define	AMG8833_SCLR  0x05
#define	AMG8833_AVE   0x07
#define	AMG8833_AVE_COMP   0x1F
#define	AMG8833_INTHL 0x08
#define	AMG8833_INTHH 0x09
#define	AMG8833_INTLL 0x0A
#define	AMG8833_INTLH 0x0B
#define	AMG8833_INTSL 0x0C
#define	AMG8833_INTSH 0x0D
#define	AMG8833_TTHL  0x0E
#define	AMG8833_TTHH  0x0F
#define	AMG8833_INT_OFF   0x10
#define	AMG8833_PIXEL_OFF 0x80

struct amg8833_dev_s{
	i2c_port_t i2cport;
	uint8_t scl_pin;
	uint8_t sda_pin;
	uint8_t dev_addr;

	i2c_master_bus_handle_t i2c_bus_handle;
	i2c_master_dev_handle_t dev_handle;
};

esp_err_t amg8833_config(struct amg8833_dev_s *dev);
esp_err_t amg8833_get_temp(struct amg8833_dev_s *dev, float *buff);
esp_err_t amg8833_get_frame(struct amg8833_dev_s *dev, float *buff, size_t in_buff_size);
esp_err_t amg8833_get_rawframe(struct amg8833_dev_s *dev, int16_t *in_buff, size_t in_buff_size);
