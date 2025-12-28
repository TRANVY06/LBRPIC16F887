#include "Piclb_byTV.h"

void I2C_Master_Wait(void) {
    //using SSPSTAT and SSPCON2 register
    //SSPCON2: in transmitting process will using bit 4 -> bit 0, all of them will be set as 1
    //SSPSTAT: using bit 2 R/W
    while while ((SSPCON2 & 0x1F) || (SSPSTATbits.R_nW));// wait until the I2C bus is idle
    /* SSPSTAT & 0x04: when I2C transmit or receive the result will be 1 
     we need to wait
        SSPCON2 & 0x1F   // 0001 1111 
        bit 0: Initiate Start condition on SDA and SCL pins. Automatically cleared by hardware.
        bit 1: repeated start
        bit 2: stop
        bit 3: receive enable
        bit 4: send ACK
        SSPSTATbits.R_nW: Read/Write Information bit 
        */   
}
 void I2C_Master_Init(const unsigned long Br){  //Br is the baud rate
    SSPCON = 0b00101000; // Cấu hình I2C master mode
 /* ||||
    ||||__ SSPEN = 1 → bật MSSP
    ||____ SSPM = 1000 → I2C Master mode, clock = FOSC / (4 * (SSPADD + 1))
 */
    SSPCON2 = 0;        // Xóa các bit điều khiển
    SSPADD = (_XTAL_FREQ/(4*Br))-1; // Cấu hình baud rate
    SSPSTAT = 0;        // Cấu hình chế độ slew rate
    
    TRISC3 = 1;         // Cấu hình SCL và SDA là đầu vào
    TRISC4 = 1;
}
void I2C_Master_Start()
{
    I2C_Master_Wait();
    SEN = 1; // Bắt đầu điều kiện start
}
void I2C_Master_Stop()
{
    I2C_Master_Wait();
    PEN = 1; // Bắt đầu điều kiện stop
}   
void I2C_Master_Write(unsigned data) // Ghi dữ liệu
{
    I2C_Master_Wait();
    SSPBUF = data; // Ghi dữ liệu vào SSPBUF
    while(SSPSTATbits.BF);  // Chờ cho đến khi quá trình truyền hoàn tất
    I2C_Master_Wait();
    if (SSPCON2bits.ACKSTAT)// Kiểm tra phản hồi từ slave
    {
        // Slave không phản hồi
    }

}
unsigned short I2C_Master_Read(unsigned short a)
{
    unsigned short temp;
    I2C_Master_Wait();
    RCEN = 1;   // Bắt đầu chế độ nhận dữ liệu
    I2C_Master_Wait();
    temp = SSPBUF; // Đọc dữ liệu từ SSPBUF
    I2C_Master_Wait();  
    ACKDT = (a)?1:0; // Thiết lập bit ACK hoặc NACK dựa trên giá trị của a khi a=0: ACK, a=1: NACK ; ACK đọc tiếp, NACK kết thúc
    ACKEN = 1; // Bắt đầu chuỗi ACK
    return temp;
}
void I2C_Slave_Init(short address) 
{
    SSPSTAT = 0x80;    // Cấu hình chế độ slew rate
    SSPADD = address; // Thiết lập địa chỉ
    SSPCON = 0x36; // Cấu hình chế độ slave
    SSPCON2 = 0x01;// Enable clock stretching
    TRISC3 = 1; // Cấu hình SCL và SDA là đầu vào
    TRISC4 = 1;
    GIE = 1; // Kích hoạt ngắt toàn cục
    PEIE = 1; // Kích hoạt ngắt ngoại vi
    SSPIF = 0; // Xóa cờ ngắt
    SSPIE = 1; // Kích hoạt ngắt I2C
}