#include<reg51.h>
#define uint unsigned int 
#define uchar unsigned char

sbit lcdrs=P3^0;
sbit lcdrw=P3^1;
sbit lcden=P3^2;

sbit DQ=P1^7;
uint value;
uchar code table1[]={0x28,0x30,0xc5,0xb8,0x00,0x00,0x00,0x8e};//rom1       测得的序列号
uchar code table2[]={0x28,0x31,0xc5,0xb8,0x00,0x00,0x00,0xb9};//rom2
uchar code table3[]="Temple";
bit fg=0;
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

void delay_us(uchar t)//微妙延时
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
uchar read_ds18b20()  //读数据
{
	uchar i=0,readat=0;
	for(i=0;i<8;i++)
	{
		DQ=0;
		readat=readat>>1;      //八位全部都是0，向右移一位后，仍然全是0
		DQ=1;
		if(DQ)                //如果DQ为1，readat进行或运算，如果为0，则子语句不执行，直接for循环此时采集的数据是0
			readat=readat|0x80;//此时最高位为1	 ，然后再进行for循环 最高位成为第七位，依次往复
		delay_us(10);
	}
	return readat;
}

void check_rom(uchar a)		   //匹配序列号
{
	uchar j;

   	write_ds18b20(0x55);
	if(a==1)
	{
		for(j=0;j<8;j++)
		{
			write_ds18b20(table1[j]);
		}
	}
	if(a==2)
	{
		for(j=0;j<8;j++)
		{
			write_ds18b20(table2[j]);
		}
	}

}

uchar change_ds18b20(uchar z)
{
	uchar tl,th;
	
	init_ds18b20();	      //初始化
	write_ds18b20(0xcc);  //跳过
	init_ds18b20();		  
	if(z==1)
	{
		check_rom(1);	   //	 匹配rom1
	}
	if(z==2)
	{
		check_rom(2);	   //	  匹配rom2
	}

	write_ds18b20(0x44);
	init_ds18b20();
	write_ds18b20(0xcc);
	init_ds18b20();
	if(z==1)
	{
		check_rom(1);	   //
	}
	if(z==2)
	{
		check_rom(2);	   //
	}

	write_ds18b20(0xbe);  //启动读暂存器。读内部ram中9字节温度数据
	tl=read_ds18b20();
	th=read_ds18b20();
	value=th;
	value=value<<8;
	value=value|tl;
	if(th < 0x80)
	{	fg=0;  }

	
	if(th>= 0x80)
	{	fg=1;
	 	
		value=~value+1;
	}
		value=value*(0.0625*10);
	    return value;
	
}

void display_lcd1602(uchar z)
{	 
	 uchar i;
	 if(z==1)
	 {
		 write_com(0x80);
		 for(i=0;i<6;i++)
		 {
		 	write_dat(table3[i]);
			delay(3);
		 }
		 write_dat(0x2d);
		 write_dat(0x41);
		 write_dat(0x3a);
		 if(fg==1)
		 {
		 	write_dat(0xb0);
		 }
		 if(fg==0)
		 {
		 	write_dat(0x20);
		 }
		 write_dat(value/100+0x30);
		 write_dat(value%100/10+0x30);
		 write_dat(0x2e);
		 write_dat(value%10+0x30);
		 write_dat(0xdf);
		 write_dat(0x43);
	 }

	 if(z==2)
	 {
		 write_com(0x80+0x40);
		 for(i=0;i<6;i++)
		 {
		 	write_dat(table3[i]);
			delay(3);
		 }
		 write_dat(0x2d);
		 write_dat(0x42);
		 write_dat(0x3a);
		 if(fg==1)
		 {
		 	write_dat(0xb0);
		 }
		 if(fg==0)
		 {
		 	write_dat(0x20);
		 }  
		 write_dat(value/100+0x30);
		 write_dat(value%100/10+0x30);
		 write_dat(0x2e);
		 write_dat(value%10+0x30);
		 write_dat(0xdf);
		 write_dat(0x43);
	 }


}

void main()
{
	init_lcd();

	while(1)
	{
		change_ds18b20(1);
		display_lcd1602(1);
		
		change_ds18b20(2);
		display_lcd1602(2);
					
	}
}