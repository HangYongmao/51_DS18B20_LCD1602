#include<reg51.h>
#define uint unsigned int 
#define uchar unsigned char

sbit lcdrs=P3^0;
sbit lcdrw=P3^1;
sbit lcden=P3^2;

sbit DQ=P1^7;
uint value;
uchar save_number[8];

void delay(uint n)
{  uint x,y; 
   for(x=n;x>0;x--) 
       for(y=110;y>0;y--); 
} 

void write_com(uchar com)
{
	lcdrs=0;			//rs=0时输入指令
	P2=com;
	delay(5);
	lcden=1;			//en=1时读取信息
	delay(5);
	lcden=0;			//1 -> 0 执行指令
}


void write_dat(uchar dat)
{
	lcdrs=1;			 //rs=1时输入数据
	P2=dat;
	delay(5);
	lcden=1;
	delay(5);
	lcden=0;
}

void init_lcd()
{
	lcden=0;
	lcdrw=0;
	write_com(0x38);	  //8位数据，双列，5*7字形
	write_com(0x0c);	 //开启显示屏，关光标，光标不闪烁
	write_com(0x06);	 //显示地址递增，即写一个数据后，显示位置右移一位
	write_com(0x01);

}



void delay_us(uchar t)//微秒延时
{
	while(t--);
}
void init_ds18b20()	//数据初始化
{
	DQ=1;
	delay_us(4);
	DQ=0;
	delay_us(80);
	DQ=1;
	delay_us(200);
}
void write_ds18b20(uchar dat) //写数据
{
	uchar i;
	for(i=0;i<8;i++)
	{
		DQ=0;		
		DQ=dat&0x01;
		delay_us(15);
		DQ=1;
		dat=dat>>1;
	}
	delay_us(10);
}
uchar read_ds18b20() //读数据
{
	uchar i=0,readat=0;
	for(i=0;i<8;i++)
	{
		DQ=0;
		readat=readat>>1;   //八位全部都是0，向右移一位后，仍然全是0
		DQ=1;
		if(DQ)             //如果DQ为1，readat进行或运算，如果为0，则子语句不执行，直接for循环此时采集的数据是0
			readat=readat|0x80;//此时最高位为1	 ，然后再进行for循环 最高位成为第七位，依次往复
		delay_us(10);
	}
	return readat;
}

uchar change_ds18b20()
{
	uchar tl,th;
	init_ds18b20();	      //初始化
	write_ds18b20(0xcc);  //跳过rom
	write_ds18b20(0x44);  //启动ds18b20进行温度转换，结果存入9字节ram中

	init_ds18b20();		  //？  缺少不可执行
	write_ds18b20(0xcc);
	write_ds18b20(0xbe);  //启动读暂存器。读内部ram中9字节温度数据
	tl=read_ds18b20();
	th=read_ds18b20();
	value=th;
	value=value<<8;
	value=value|tl;
	value=value*(0.0625*10)+0.5;
	return value;
}


void dis_readnumber()					 
{
	uchar i,temp;

	init_ds18b20();

	write_ds18b20(0x33);	             //读rom   即rom中的编码
	for(i=0;i<8;i++)
	{
		save_number[i]=read_ds18b20();  //保存序列号
	}

    write_com(0x80);				    
	for(i=0;i<8;i++)
	{
		temp=save_number[i]>>4;
		if(temp<10)
		{
			write_dat(0x30+temp);
		}
		else
		{
			 write_dat(0x37+temp);
		}

		temp=save_number[i]&0x0f;
		if(temp<10)
		{
			write_dat(0x30+temp);
		}
		else
		{
			 write_dat(0x37+temp);
		}
	}

}
void main()
{
	init_lcd();
	while(1)
	{
	   dis_readnumber();    //显示序列号
	}
}
	 