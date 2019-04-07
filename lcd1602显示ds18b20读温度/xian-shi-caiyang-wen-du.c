#include<reg51.h>
#define uint unsigned int 
#define uchar unsigned char

sbit lcdrs=P3^0;
sbit lcdrw=P3^1;
sbit lcden=P3^2;

sbit DQ=P1^7;
uint value;
uchar code table1[]={0x28,0x30,0xc5,0xb8,0x00,0x00,0x00,0x8e};//rom1       ��õ����к�
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
	lcdrs=0;			//rs=0ʱ����ָ��
	P2=com;
	delay(5);
	lcden=1;			//en=1ʱ��ȡ��Ϣ
	delay(5);
	lcden=0;			//1 -> 0 ִ��ָ��
}


void write_dat(uchar dat)
{
	lcdrs=1;			 //rs=1ʱ��������
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
	write_com(0x38);	  //8λ���ݣ�˫�У�5*7����
	write_com(0x0c);	 //������ʾ�����ع�꣬��겻��˸
	write_com(0x06);	 //��ʾ��ַ��������дһ�����ݺ���ʾλ������һλ
	write_com(0x01);

}

void delay_us(uchar t)//΢����ʱ
{
	while(t--);
}
void init_ds18b20()	//���ݳ�ʼ��
{
	DQ=1;
	delay_us(4);
	DQ=0;
	delay_us(80);
	DQ=1;
	delay_us(200);
}
void write_ds18b20(uchar dat) //д����
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
uchar read_ds18b20()  //������
{
	uchar i=0,readat=0;
	for(i=0;i<8;i++)
	{
		DQ=0;
		readat=readat>>1;      //��λȫ������0��������һλ����Ȼȫ��0
		DQ=1;
		if(DQ)                //���DQΪ1��readat���л����㣬���Ϊ0��������䲻ִ�У�ֱ��forѭ����ʱ�ɼ���������0
			readat=readat|0x80;//��ʱ���λΪ1	 ��Ȼ���ٽ���forѭ�� ���λ��Ϊ����λ����������
		delay_us(10);
	}
	return readat;
}

void check_rom(uchar a)		   //ƥ�����к�
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
	
	init_ds18b20();	      //��ʼ��
	write_ds18b20(0xcc);  //����
	init_ds18b20();		  
	if(z==1)
	{
		check_rom(1);	   //	 ƥ��rom1
	}
	if(z==2)
	{
		check_rom(2);	   //	  ƥ��rom2
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

	write_ds18b20(0xbe);  //�������ݴ��������ڲ�ram��9�ֽ��¶�����
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