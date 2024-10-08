#include <REGX52.H>

/**
  * @brief  串口初始化4800bps@11.0592MHz
  * @param  无
  * @retval 
  */

void UART_Init()  //4800bps@11.0592MHz
{
	//4800bps@12.0000MHz
//	SCON = 0x40;
//	PCON |= 0x80;
//	TMOD &= 0x0F;		//设置定时器模式
//	TMOD |= 0x20;		//设置定时器模式
//	TL1 = 0xF3;		//设置定时初值
//	TH1 = 0xF3;		//设置定时初值
//	TR1 = 1;		//定时器0开始计时
//	ET1 = 0; //禁止中断
	PCON |= 0x80;		//使能波特率倍速位SMOD
	SCON = 0x40;		//8位数据,可变波特率
	TMOD &= 0x0F;		//清除定时器1模式位
	TMOD |= 0x20;		//设定定时器1为8位自动重装方式
	TL1 = 0xF4;		//设定定时初值
	TH1 = 0xF4;		//设定定时器重装值
	ET1 = 0;		//禁止定时器1中断
	TR1 = 1;		//启动定时器1

}

/**
  * @brief  写入发送的数据到SBUF，串口发送一个字节数据
* @param  发送的数据为Byte
  * @retval 
  */

void UART_SendByte(unsigned char Byte)
{
	SBUF = Byte;//数据写入SBUF
	while(TI == 0);
	TI = 0;
}