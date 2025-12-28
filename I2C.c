#include "Piclb_byTV.h"
/*
Hàm chờ I2C bus rảnh
Sử dụng thanh ghi SSPSTAT và SSPCON2 để kiểm tra trạng thái bus
Các bit trong SSPCON2 từ bit 0 đến bit 4 sẽ được sử dụng để kiểm tra
trạng thái truyền
Bit R_nW trong SSPSTAT để kiểm tra chế độ đọc/ghi
Khi có điều bắt đầu truyền hoặc nhận dữ liệu, bit R_nW sẽ được đặt là 1
và các bit trong SSPCON2 từ bit 0 đến bit 4 sẽ được đặt là 1
Hàm sẽ chờ đến khi tất cả các bit này về 0, tức là bus rảnh
Ví dụ:
    Khi SSPCON2 = 0001 1111 (tức là đang trong quá trình truyền)
    và SSPSTATbits.R_nW = 1 (tức là đang trong chế độ đọc)
    thì hàm sẽ chờ đến khi SSPCON2 = 0000 0000 và SSPSTATbits.R_nW = 0    
*/
void I2C_Master_Wait(void) {
    //using SSPSTAT and SSPCON2 register
    //SSPCON2: in transmitting process will using bit 4 -> bit 0, all of them will be set as 1
    //SSPSTAT: using bit 2 R/W
    while((SSPCON2 & 0x1F) || (SSPSTATbits.R_nW));// wait until the I2C bus is idle
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
/*
Hàm khởi tạo I2C Master
Br: baud rate
Chuẩn baud rate = Fosc / (4 * (SSPADD + 1))
Chuyền br vào để tính SSPADD
Ví dụ: Fosc = 20MHz, br = 100kHz
SSPADD = (20000000 / (4 * 100000)) - 1 = 49
Cau hình các thanh ghi cần thiết để I2C hoạt động ở chế độ Master
*/
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
/*
Hàm bắt đầu truyền I2C
Điều kiện start sẽ được gửi khi SEN = 1
khi đó SCL và SDA sẽ được kéo xuống trước khi SDA chuyển từ cao xuống thấp
*/
void I2C_Master_Start()
{
    I2C_Master_Wait();
    SEN = 1; // Bắt đầu điều kiện start
}
/*
Hàm dừng truyền I2C
Điều kiện stop sẽ được gửi khi PEN = 1
khi đó SCL sẽ được kéo cao trong khi SDA chuyển từ thấp lên cao
*/
void I2C_Master_Stop()
{
    I2C_Master_Wait();
    PEN = 1; // Bắt đầu điều kiện stop
} 
/*
Hàm ghi dữ liệu vào I2C Slave
data: dữ liệu cần ghi
khi ghi xong sẽ kiểm tra tín hiệu ACK từ Slave khi slave phản hồi
nếu không có ACK sẽ xử lý lỗi
Ví dụ:
    SSPCON2bits.ACKSTAT = 0 --> Slave đã ACK
    SSPCON2bits.ACKSTAT = 1 --> Slave không ACK
*/  
void I2C_Master_Write(unsigned char data) // Ghi dữ liệu
{
    I2C_Master_Wait();     // Chờ bus rảnh
    SSPBUF = data;         // Ghi dữ liệu
    I2C_Master_Wait();     // Chờ truyền xong

    if (SSPCON2bits.ACKSTAT) // ACKSTAT = 1 → Slave không ACK
    {
        // xử lý lỗi (slave không phản hồi)
    }
}
/*
Hàm đọc dữ liệu từ I2C Slave 
biến temp lưu dữ liệu đọc được
a là biến để chọn ACK hoặc NACK
a = I2C_ACK  --> Gửi ACK
a= I2C_NACK --> Gửi NACK
Trả về giá trị đọc được từ Slave
Ví dụ:
    unsigned short receivedData;
    receivedData = I2C_Master_Read(I2C_ACK); // Đọc dữ liệu và gửi ACK
Ví dụ:
    unsigned short receivedData;
    receivedData = I2C_Master_Read(I2C_NACK); // Đọc dữ liệu và gửi NACK
    Việc gửi NACK thường được sử dụng để báo hiệu kết thúc việc đọc dữ liệu từ Slave
*/
unsigned short I2C_Master_Read(unsigned short a)// a = I2C_ACK or I2C_NACK
{
    unsigned short temp;
    I2C_Master_Wait();
    RCEN = 1;   // Bắt đầu chế độ nhận dữ liệu
    I2C_Master_Wait();
    temp = SSPBUF; // Đọc dữ liệu từ SSPBUF
    I2C_Master_Wait();  
    ACKDT = a; // Chọn ACK hoặc NACK voi biến a=I2C_ACK or I2C_NACK
    ACKEN = 1; // Bắt đầu chuỗi ACK
    return temp;
}
/*
Hàm khởi tạo I2C Slave
address: địa chỉ 7 bit của Slave
*/
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