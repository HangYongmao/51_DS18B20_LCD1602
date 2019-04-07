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
uchar read_ds18b20() //������
{
	uchar i=0,readat=0;
	for(i=0;i<8;i++)
	{
		DQ=0;
		readat=readat>>1;   //��λȫ������0��������һλ����Ȼȫ��0
		DQ=1;
		if(DQ)             //���DQΪ1��readat���л����㣬���Ϊ0��������䲻ִ�У�ֱ��forѭ����ʱ�ɼ���������0
			readat=readat|0x80;//��ʱ���λΪ1	 ��Ȼ���ٽ���forѭ�� ���λ��Ϊ����λ����������
		delay_us(10);
	}
	return readat;
}

uchar change_ds18b20()
{
	uchar tl,th;
	init_ds18b20();	      //��ʼ��
	write_ds18b20(0xcc);  //����rom
	write_ds18b20(0x44);  //����ds18b20�����¶�ת�����������9�ֽ�ram��

	init_ds18b20();		  //��  ȱ�ٲ���ִ��
	write_ds18b20(0xcc);
	write_ds18b20(0xbe);  //�������ݴ��������ڲ�ram��9�ֽ��¶�����
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

	write_ds18b20(0x33);	             //��rom   ��rom�еı���
	for(i=0;i<8;i++)
	{
		save_number[i]=read_ds18b20();  //�������к�
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
	   dis_readnumber();    //��ʾ���к�
	}
}
	 