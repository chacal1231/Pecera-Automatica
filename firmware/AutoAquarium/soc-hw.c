#include "soc-hw.h"

uart_t              *uart0          = (uart_t *)            0x20000000;
timerH_t            *timer0         = (timerH_t *)          0x30000000;
gpio_t              *gpio0          = (gpio_t *)            0x40000000;
uart_t              *uart1          = (uart_t *)            0x50000000;
leds_t              *leds0          = (leds_t *)            0x60000000;
i2c_t               *i2c0           = (i2c_t *)             0x70000000;
SK6812RGBW_t        *SK6812RGBW0    = (SK6812RGBW_t *)      0x80000000;

isr_ptr_t isr_table[32];    

/***************************************************************************
 * IRQ handling
 */
void isr_null(void)
{
}

void irq_handler(uint32_t pending)
{     
    uint32_t i;

    for(i=0; i<32; i++) {
        if (pending & 0x00000001){
            (*isr_table[i])();
        }
        pending >>= 1;
    }
}

void isr_init(void)
{
    int i;
    for(i=0; i<32; i++)
        isr_table[i] = &isr_null;
}

void isr_register(int irq, isr_ptr_t isr)
{
    isr_table[irq] = isr;
}

void isr_unregister(int irq)
{
    isr_table[irq] = &isr_null;
}

/***************************************************************************
 * TIMER Functions
 */
uint32_t tic_msec;

void mSleep(uint32_t msec)
{
    uint32_t tcr;

    // Use timer0.1
    timer0->compare1 = (FCPU/1000)*msec;
    timer0->counter1 = 0;
    timer0->tcr1 = TIMER_EN;

    do {
        //halt();
         tcr = timer0->tcr1;
     } while ( ! (tcr & TIMER_TRIG) );
}

void uSleep(uint32_t usec)
{
    uint32_t tcr;

    // Use timer0.1
    timer0->compare1 = (FCPU/1000000)*usec;
    timer0->counter1 = 0;
    timer0->tcr1 = TIMER_EN;

    do {
        //halt();
         tcr = timer0->tcr1;
     } while ( ! (tcr & TIMER_TRIG) );
}

void tic_isr(void)
{
    tic_msec++;
    timer0->tcr0     = TIMER_EN | TIMER_AR | TIMER_IRQEN;
}

void tic_init(void)
{
    tic_msec = 0;

    // Setup timer0.0
    timer0->compare0 = (FCPU/10000);
    timer0->counter0 = 0;
    timer0->tcr0     = TIMER_EN | TIMER_AR | TIMER_IRQEN;

    isr_register(1, &tic_isr);
}


/***************************************************************************
 * UART Functions
 */

char uart_getchar(void)
{   
    while (! (uart0->ucr & Uart_RXData_Ready));
    return uart0->rx_data;
}

void uart_putchar(char c)
{
    while (uart0->ucr & Uart_TX_Busy);
        uart0->tx_data = c;
        uart0->tx_run = 1;
        uart0->tx_run = 0; 
    
}

void uart_putstr(char *str)
{
    char *c = str;
    while(*c) {
        uart_putchar(*c);
        while(txbusy()){
        uSleep(1);
        }
        c++;
    }
}

uint32_t txbusy(void){
    return uart0->tx_busy;
}
uint32_t rxavail(void){
    return uart0->rx_avail;
}

/***************************************************************************
 * Comunicaciones Functions
 */


/*************************************************************************/ /**
Función iniciar ESP8266
*****************************************************************************/
void WIFI_INIT(void){
    mSleep(2000);
    uart_putstr("AT\r\n");
    mSleep(2000);
    uart_putstr("AT+CWMODE=1\r\n");
    mSleep(2000);
    //uart_putstr("AT+CWJAP=\"LenovoAndroid\",\"54321osk\"\r\n");
    uart_putstr("AT+CWJAP=\"-David McMahon\",\"masteryi\"\r\n");
    //uart_putstr("AT+CWJAP=\"Moto G\",\"09aa276522ec\"\r\n");
    mSleep(16000);
    uart_putstr("AT+CWJAP?\r\n");
}
/*************************************************************************/ /**
Función Conectar al servidor
*****************************************************************************/
void WIFIConnectServer(void){
    uart_putstr("AT\r\n");
    mSleep(2000);
    uart_putstr("AT+CIPMUX=0\r\n");
    mSleep(2000);
    uart_putstr("AT+CIPMODE=1\r\n");
    mSleep(2000);
    uart_putstr("AT+CIPSTART=\"TCP\",\"200.112.210.132\",7778\r\n");
    mSleep(6000);
    uart_putstr("AT+CIPSTATUS\r\n");
}
/*************************************************************************/ /**
Función establecer conexión con el servidor Sockets
*****************************************************************************/
void WIFIStartSend(void){
    uart_putstr("AT+CIPSEND\r\n");
    mSleep(2000);
    WIFISendPotencia(0x30);
    mSleep(1000);
    WIFISendpH(0x31);
    mSleep(1000);
    WIFISendTemp(0x32);
    mSleep(1000);
    WIFISendFiltro(0x33);
    mSleep(1000);
    WIFISendImagen(0x34);
    mSleep(1000);
    WIFISendComida(0x35);
}

/*************************************************************************/ /**
Función enviar potencia
*****************************************************************************/
void WIFISendPotencia(uint32_t Potencia){
    uart_putstr("Potencia=");
    uart_putchar(Potencia);
    uart_putstr("\r\n");
}
/*************************************************************************/ /**
Función enviar pH
*****************************************************************************/
void WIFISendpH(uint32_t pH){
    uart_putstr("pH=");
    uart_putchar(pH);
    uart_putstr("\r\n");
}
/*************************************************************************/ /**
Función enviar Temperatura
*****************************************************************************/
void WIFISendTemp(uint32_t Temp){
    uart_putstr("Temp=");
    uart_putchar(Temp);
    uart_putstr("\r\n");
}
/*************************************************************************/ /**
Función enviar estado del filtro
*****************************************************************************/
void WIFISendFiltro(uint32_t Filtro){
    uart_putstr("Filtro=");
    uart_putchar(Filtro);
    uart_putstr("\r\n");
}
/*************************************************************************/ /**
Función enviar imagen
*****************************************************************************/
void WIFISendImagen(uint32_t Imagen){
    uart_putstr("Imagen=");
    uart_putchar(Imagen);
    uart_putstr("\r\n");
}
/*************************************************************************/ /**
Función enviar Comida
*****************************************************************************/
void WIFISendComida(uint32_t Comida){
    uart_putstr("Comida=");
    uart_putchar(Comida);
    uart_putstr("\r\n");
}
/*************************************************************************/ /**
Función recibir comando del filtro y enviarlo al módulo del filtro
*****************************************************************************/
void WIFIRecivFiltro(void){

}
/*************************************************************************/ /**
Función recibir comando de tomar imagen y enviarlo al módulo de cámara
*****************************************************************************/
void WIFIRecivTakeImagen(void){

}
/*************************************************************************/ /**
Función recibir comando de tomar imagen y enviarlo al módulo de cámara
*****************************************************************************/
void WIFIRecivAlimen(void){

}

/***************************************************************************
 *Iluminación
 */

void set_start(uint32_t start0, uint32_t data0)
{   
     leds0->init = 0;
     leds0->rw = 0;
     leds0->start_add = start0;
     leds0->data = data0;
}
void leds_refresh(void){
  leds0->rw=1;
  leds0->init = 1;
  leds0->init = 0;  
}
uint32_t leds_finish(void){
    return leds0->done;
}

/***************************************************************************
 * I2C Functions
 */
 
uint8_t i2c_read(uint32_t slave_addr, uint32_t per_addr)
{
        
    while(!(i2c0->scr & I2C_DR));       //Se verifica que el bus esté en espera
    i2c0->s_address = slave_addr;
    i2c0->s_reg     = per_addr;
    i2c0->start_rd  = 0x00;
    while(!(i2c0->scr & I2C_DR));
    return i2c0->i2c_rx_data;
}

void i2c_write(uint32_t slave_addr, uint32_t per_addr, uint32_t data){
    
    while(!(i2c0->scr & I2C_DR));       //Se verifica que el bus esté en espera
    i2c0->s_address = slave_addr;
    i2c0->s_reg     = per_addr;
    i2c0->tx_data   = data;
    i2c0->start_wr  = 0x00;
}

/***************************************************************************
 * Pantalla y pH Functions
 */

void send_command_display(uint32_t addr, uint32_t command){
    i2c_write(addr, DISPLAY_COMMAND, command);
};

void send_data_display(uint32_t addr, uint32_t data){
    i2c_write(addr, DISPLAY_INDEX, data);
};

void sec_on_display(void){
        mSleep(1);
    send_command_display(DISPLAY_ADDR,0xAE); //OFF PANTALLA 
    mSleep(1);
    send_command_display(DISPLAY_ADDR,0X20); // MODO DE DIRECCIONAMIENTO
    mSleep(1);
    send_command_display(DISPLAY_ADDR,0x00);
    mSleep(1);
    send_command_display(DISPLAY_ADDR,0xB0); // CUADRAR DIRECCIÓN INICIAL DE PAGINA
    mSleep(1);
        send_command_display(DISPLAY_ADDR,0xC8); //OUTPUT SCAN COM DIRECTORY
        mSleep(1);
        send_command_display(DISPLAY_ADDR,0x00); // --- SET LOW COLUMN ADDR ADDRES       
        mSleep(1);
        send_command_display(DISPLAY_ADDR,0x10); // --- SET HIGH COLUMN ADDR
        mSleep(1);
        send_command_display(DISPLAY_ADDR,0x40); // --- SET STAR LINE ADDR
        mSleep(1);
        send_command_display(DISPLAY_ADDR,0x81); // SET CONTRAST
        mSleep(1);
        send_command_display(DISPLAY_ADDR,0x3F); //
        mSleep(1);
        send_command_display(DISPLAY_ADDR,0xA1); // SET SEGMENT RE-MAP. A1=addr 127 MAPPED
        mSleep(1);
        send_command_display(DISPLAY_ADDR,0xA6); // SET DISPLAY MODE. A6=NORMAL
        mSleep(1);
        send_command_display(DISPLAY_ADDR,0xA8); // SET MUX RATIO
        mSleep(1);
        send_command_display(DISPLAY_ADDR,0x3F);
        mSleep(1);
        send_command_display(DISPLAY_ADDR,0xA4); // OUTPUT RAM TO DISPLAY
        //send_command_display(DISPLAY_ADDR,0xA5); // ENTIRE DISPLAY ON
        mSleep(1);
        send_command_display(DISPLAY_ADDR,0xD3); // DISPLAY OFFSET. 00= NO
        mSleep(1);
        send_command_display(DISPLAY_ADDR,0x00);
        mSleep(1);
        send_command_display(DISPLAY_ADDR,0xD5); //---SET DISPLAY CLOCK  DIVIDE RATIO /OSCILATOR
        mSleep(1);
        send_command_display(DISPLAY_ADDR,0xF0); //-- SET DIVIDE RATIO
        mSleep(1);
        send_command_display(DISPLAY_ADDR,0xD9); //SET PRE-CHARGUE PERIOD
        mSleep(1);
        send_command_display(DISPLAY_ADDR,0x22);
        mSleep(1);
        send_command_display(DISPLAY_ADDR,0xDA); //SET COM PINS 
        mSleep(1);
        send_command_display(DISPLAY_ADDR,0x12);
        mSleep(1);
        send_command_display(DISPLAY_ADDR,0xDB); //--SET VCOMH
        mSleep(1);
        send_command_display(DISPLAY_ADDR,0x20); //0x20,0.77xVcc
        mSleep(1);
        send_command_display(DISPLAY_ADDR,0x8D); //SET DC-DC ENABLE
        mSleep(1);
        send_command_display(DISPLAY_ADDR,0x14); 
        mSleep(1);
        send_command_display(DISPLAY_ADDR,0xAF); //ON PANTALLA
        mSleep(1);
};  

void clear_GDRAM(void){
        int i, j;
        mSleep(1);
    send_command_display(DISPLAY_ADDR,0x21); //Configurar el direccionamiento por columna
        mSleep(1);
    send_command_display(DISPLAY_ADDR,0x00);
    mSleep(1);
    send_command_display(DISPLAY_ADDR,0x7F);
    mSleep(1);
    send_command_display(DISPLAY_ADDR,0x22); //Configurar el diferccionamiento por página
    mSleep(1);
    send_command_display(DISPLAY_ADDR,0x00);
    mSleep(1);
    send_command_display(DISPLAY_ADDR,0x07);
    for(j=1;j<9;j++){
            for (i=1;i<129;i++) {  
                mSleep(1);
                    send_data_display(0x3C, 0x00);
            }
        }
    
};

/***************************************************************************
 * SK6812RGBW Functions
 */
void SK6812RGBW_init(void)
{
    SK6812RGBW_nBits(35*32);//35 Leds
    SK6812RGBW_source(0);
    SK6812RGBW_rgbw(0x00000000);
    SK6812RGBW_rgbw(0x00000000);
}

uint32_t SK6812RGBW_busy(void)
{
    return SK6812RGBW0->busy;
}

void SK6812RGBW_rgbw(uint32_t rgbw)
{
    while (SK6812RGBW_busy());
    SK6812RGBW0->rgbw = rgbw;
}

void SK6812RGBW_nBits(uint32_t nBits)
{
    while (SK6812RGBW_busy());
    SK6812RGBW0->nBits = nBits;
}

void SK6812RGBW_source(uint32_t source)
{
    while (SK6812RGBW_busy());
    SK6812RGBW0->source = source;
}

void SK6812RGBW_ram(uint32_t color, uint32_t add)
{
  uint32_t   *wram   = (uint32_t *)    (0x60000000 + 0x1000 + add);
  *wram = color;
}

void SK6812RGBW_ram_w(void)
{
    while (SK6812RGBW_busy());
    SK6812RGBW0->rgbw = 0;
}


/***************************************************************************
 * SK6812RGBW Functions
 */
/**
 * strstr - Find the first substring in a %NUL terminated string
 * @s1: The string to search for
 * @s2: The string to be searched
 */
 /**

 * memcmp - Compare two areas of memory
 * @cs: One area of memory
 * @ct: Another area of memory
 * @count: The size of the area.
 */
int memcmp(const void *cs, const void *ct, size_t count)
{
    const unsigned char *su1, *su2;
    int res = 0;

    for (su1 = cs, su2 = ct; 0 < count; ++su1, ++su2, count--)
        if ((res = *su1 - *su2) != 0)
            break;
    return res;
}
char *strstr(const char *s1, const char *s2)
{
    size_t l1, l2;

    l2 = strlen(s2);
    if (!l2)
        return (char *)s1;
    l1 = strlen(s1);
    while (l1 >= l2) {
        l1--;
        if (!memcmp(s1, s2, l2))
            return (char *)s1;
        s1++;
    }
    return NULL;
}

/**
 * strlen - Find the length of a string
 * @s: The string to be sized
 */
size_t strlen(const char *s)
{
    const char *sc;

    for (sc = s; *sc != '\0'; ++sc)
        /* nothing */;
    return sc - s;
}

