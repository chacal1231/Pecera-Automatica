#ifndef SPIKEHW_H
#define SPIKEHW_H

#define PROMSTART 0x00000000
#define RAMSTART  0x00000800
#define RAMSIZE   0x400
#define RAMEND    (RAMSTART + RAMSIZE)

#define RAM_START 0x40000000
#define RAM_SIZE  0x04000000

#define FCPU      50000000

#define UART_RXBUFSIZE 32

#define DISPLAY_ADDR  0X3C
#define DISPLAY_COMMAND 0X00
#define DISPLAY_INDEX 0X40


/****************************************************************************
 * Types
 */
typedef unsigned int  uint32_t;    // 32 Bit
typedef signed   int   int32_t;    // 32 Bit

typedef unsigned char  uint8_t;    // 8 Bit
typedef signed   char   int8_t;    // 8 Bit


/****************************************************************************
 * Interrupt handling
 */
typedef void(*isr_ptr_t)(void);

void     irq_enable(void);
void     irq_disable(void);
void     irq_set_mask(uint32_t mask);
uint32_t irq_get_mak(void);

void     isr_init(void);
void     isr_register(int irq, isr_ptr_t isr);
void     isr_unregister(int irq);
void     isr_null(void);

/****************************************************************************
 * General Stuff
 */
void     halt(void);
void     jump(uint32_t addr);


/****************************************************************************
 * Timer
 */
#define TIMER_EN     0x08    // Enable Timer
#define TIMER_AR     0x04    // Auto-Reload
#define TIMER_IRQEN  0x02    // IRQ Enable
#define TIMER_TRIG   0x01    // Triggered (reset when writing to TCR)

typedef struct {
    volatile uint32_t tcr0;
    volatile uint32_t compare0;
    volatile uint32_t counter0;
    volatile uint32_t tcr1;
    volatile uint32_t compare1;
    volatile uint32_t counter1;
} timerH_t;

void mSleep(uint32_t msec);
void uSleep(uint32_t usec);
void tic_init(void);
void tic_isr(void);
/***************************************************************************
 * Comunicaciones
 */
void WIFI_INIT(void);
void WIFIConnectServer(void);
void WIFIStartSend(void);
void WIFISendVar(void);
void WIFISendPotencia(uint32_t Potencia);
void WIFISendpH(uint32_t pH);
void WIFISendTemp(uint32_t Temp);
void WIFISendFiltro(uint32_t Filtro);
void WIFISendImagen(uint32_t imagen);
void WIFISendComida(uint32_t Comida);
void WIFIRecivFiltro(void);
void WIFIRecivTakeImagen(void);
/***************************************************************************
 * Iluminación
 */

typedef struct {
   volatile uint32_t init;        //0x00
   volatile uint32_t done;        //0x04
   volatile uint32_t start_add;   //0x08
   volatile uint32_t data;        //0x0C
   volatile uint32_t rw;      //0x10
} leds_t;

void set_start(uint32_t start0, uint32_t data0);
void leds_refresh(void);
uint32_t leds_finish(void);

/***************************************************************************
 * I2C0
 */

#define I2C_DR   0x03     //RX Data Ready
#define I2C_BUSY 0x04     //I2C Busy
#define I2C_ERR  0x02     //RX Error

typedef struct {
   volatile uint32_t scr;
   volatile uint32_t i2c_rx_data;
   volatile uint32_t s_address;
   volatile uint32_t s_reg;
   volatile uint32_t tx_data;
   volatile uint32_t start_wr;
   volatile uint32_t start_rd;
} i2c_t;

uint8_t i2c_read(uint32_t slave_addr, uint32_t per_addr);
void i2c_write(uint32_t slave_addr, uint32_t per_addr, uint32_t data);

/***************************************************************************
 * GPIO0
 */
typedef struct {
    volatile uint32_t ctrl;
    volatile uint32_t dummy1;
    volatile uint32_t dummy2;
    volatile uint32_t dummy3;
    volatile uint32_t in;
    volatile uint32_t out;
    volatile uint32_t oe;
} gpio_t;

/***************************************************************************
 * UART0
 */
#define UART_DR   0x01                    // RX Data Ready
#define UART_ERR  0x02                    // RX Error
#define UART_BUSY 0x10                    // TX Busy

typedef struct {
   volatile uint32_t ucr;
   volatile uint32_t rxtx;
} uart_t;

//UART0
void uart_init(void);
void uart_putchar(char c);
void uart_putstr(char *str);
char uart_getchar(void);
//UART1
void uart1_putchar(char c);
void uart1_putstr(char *str);
char uart1_getchar(void);

/***************************************************************************
 * SPI0
 */

typedef struct {
   volatile uint32_t rxtx;
   volatile uint32_t nop1;
   volatile uint32_t cs;
   volatile uint32_t nop2;
   volatile uint32_t divisor;
} spi_t;

void spi_init(void);
void spi_putchar(char c);
char spi_getchar(void);

/***************************************************************************
 * Pantalla y pH
 */
void send_command_display(uint32_t addr, uint32_t command);
void send_data_display(uint32_t addr, uint32_t data);
void sec_on_display(void);
void clear_GDRAM(void);

/***************************************************************************
 * SK6812RGBW0
 */

typedef struct {
   volatile uint32_t nBits;
   volatile uint32_t rgbw;
   volatile uint32_t source;
   volatile uint32_t busy;
} SK6812RGBW_t;

void SK6812RGBW_init(void);
uint32_t SK6812RGBW_busy(void);
void SK6812RGBW_rgbw(uint32_t rgbw);
void SK6812RGBW_nBits(uint32_t nBits);
void SK6812RGBW_source(uint32_t source);
void SK6812RGBW_ram(uint32_t color, uint32_t add);
void SK6812RGBW_ram_w(void);

/***************************************************************************
 * Pointer to actual components
 */
extern    timerH_t       *timer0;
extern    uart_t         *uart0; 
extern    gpio_t         *gpio0;
extern    uart_t         *uart1;
extern    leds_t         *leds0;
extern    uint32_t       *sram0;
extern    SK6812RGBW_t   *SK6812RGBW0; 

#endif // SPIKEHW_H
