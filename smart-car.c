#include<reg52.h>
#include<intrins.h>

#define uchar unsigned char	//�궨�峣���������ͣ�������д
#define uint unsigned int
#define ulong unsigned long

sbit yq=P3^4;	//��Ƭ��P34�ڽ�L298��in1
sbit yh=P3^5;	//��Ƭ��P35�ڽ�L298��in2
sbit zq=P3^6;	//��Ƭ��P36�ڽ�L298��in3
sbit zh=P3^7;	//��Ƭ��P37�ڽ�L298��in4
sbit RX=P3^2;	//��Ƭ��P32�ڽӳ�����ģ��ECH0
sbit TX=P3^3;	//��Ƭ��P33�ڽӳ�����ģ��TRIG
sbit pwm=P0^3;	//��Ƭ��P03�����PWM�źţ����ƶ��

uchar zd;

/******************��ʱ����**************************************/
void delay(uint z)
{
	uint x,y;
	for(x=z;x>0;x--)
		for(y=120;y>0;y--);
}

/******************������ƺ���**************************************/
void gostraight(uint t)	//С��ǰ��
{
	zq=1;
	yq=1;
	zh=0;
	yh=0;
	delay(t);
}

void back(uint t)	//С������
{
	zq=0;
	yq=0;
	zh=1;
	yh=1;
	delay(t);		  
}

 void turnleft(uint t)	//С����ת
{
	zq=0;
	yq=1;
	zh=1;
	yh=0;
	delay(t);
}
		  
void turnright(uint t)	//С����ת
{
	zq=1;
	yq=0;
	zh=0;
	yh=1;
	delay(t);
}

void stop(uint t)	//С��ֹͣ   
{
	zq=0;
	yq=0;
	zh=0;
	yh=0;
	delay(t);		  
}

/******************������ģ���⺯��**************************************/
uint time=0;
uint timer=0;
uchar posit=0;
ulong S=0;				    
bit flag=0;
unsigned char const	positon[3]={0xdf,0xef,0xf7};
uchar disbuff[4]={0,0,0,0};

ulong Conut(void)
{
	ulong c;
	time=TH0*256+TL0;
	TH0=0;
	TL0=0;
	
	S=(time*1.7)/100;	//�������CM
	if((S>=700)||flag==1)	//����������Χ��ʾ��-��
	{	 
		flag=0;
		disbuff[0]=10;	   //��-��
		disbuff[1]=10;	   //��-��
		disbuff[2]=10;	   //��-��
	}
	else
	{
		disbuff[0]=S%1000/100;
		disbuff[1]=S%1000%100/10;
		disbuff[2]=S%1000%10 %10;
	}
	
	c=S;
	return c;
}

void init(void)
{
	TMOD=0x11;	//��T0Ϊ��ʽ1��GATE=1��
	TH0=0;
	TL0=0;          
	TH1=0xf8;	//2MS��ʱ
	TL1=0x30;
	ET0=1;	//����T0�ж�
	ET1=1;	//����T1�ж�
	TR1=1;	//������ʱ��
	EA=1;	//�������ж�
}

ulong distance(void)
{
	ulong d;
	zd=0;
	init();
	delay(10);
	while(!RX);	//��RXΪ��ʱ�ȴ�
	TR0=1;	//��������
	while(RX);	//��RXΪ1�������ȴ�
	TR0=0;	//�رռ���
	d=Conut();	//����
	return d;
}

/******************������ƺ���**************************************/
uint pwm_value=1580;	//��ֵΪ1.5ms

void InitTimer(void)
{
	TMOD=0x11;	//����ʱ��0,1
	TH0=-20000/256;	//��ʱ20MS,20MSΪһ������
	TL0=-20000%256;
	TH1=-1500/256;	//��ʱ1.5MS,��ʱ�������0��
	TL1=-1500%256;
	EA=1;	//���ܶ�
	TR0=1;	//����ʱ��0
	ET0=1;
	TR1=1;	//����ʱ��1
	ET1=1;
}

void timer0(void) interrupt 1	//��ʱ��0�жϺ���
{
	if(zd==0)
	{
		flag=1;	//�ж������־
	}
    else
	{
		pwm=1;
		TH0=-20000/256;
		TL0=-20000%256;
		TR1=1;
	}
}

void timer1(void) interrupt 3	//��ʱ��1�жϺ���
{
	if(zd==0)
	{
		TH1=0xf8;
		TL1=0x30;
		timer++;
		if(timer>=400)
		{
			timer=0;
			TX=1;	//800MS  ����һ��ģ��
			_nop_(); 
			_nop_(); 
			_nop_(); 
			_nop_(); 
			_nop_(); 
			_nop_(); 
			_nop_(); 
			_nop_(); 
			_nop_(); 
			_nop_(); 
			_nop_(); 
			_nop_(); 
			_nop_(); 
			_nop_(); 
			_nop_(); 
			_nop_(); 
			_nop_();
			_nop_(); 
			_nop_(); 
			_nop_(); 
			_nop_();
			TX=0;
		} 
	}
	else
	{
		pwm=0;
		TH1=-pwm_value/256;
		TL1=-pwm_value%256;
		TR1=0;
	}
}

void duojistop(void)	//���������λ��
{
	uchar i;
	zd=1;
	InitTimer();
	delay(100);
	for(i=0;i<10;i++)
	{
		pwm_value=1580;	//�ı����Ƕ�
		delay(100);
	}	
}

void duojileft(void)	//�����ת
{
	uchar i;
	zd=1;
	InitTimer();
	delay(100);
	for(i=0;i<10;i++)
	{
		pwm_value=2000;
		delay(100);
	}
}

void duojiright(void)	//�����ת
{
	uchar i;
	zd=1;
	InitTimer();
	delay(100);
	for(i=0;i<10;i++)
	{
		pwm_value=1160;
		delay(100);
	}
}

/******************������**************************************/
int main()
{	   
	ulong d,dz,dy;
	duojistop();
	delay(1000);
	while(1)
	{
		d=distance();
		if(d>45)
		{
			gostraight(1);
		}
		else if(d>20) 
		{
			stop(1);
			duojileft();
			dz=distance();
			dz=distance();

			if(dz>45)
			{
				duojistop();
				turnleft(500);
				gostraight(1);
			}
			else
			{
				duojiright();
				dy=distance();
				dy=distance();
				if(dy>45)
				{
					duojistop();
					turnright(500);
					gostraight(1);
				}
				else
				{
					duojistop();
					turnright(2000);
					gostraight(1);
				}
			}
		}
		else if(d>10) 
		{
			stop(1);
			back(75);
			stop(1);
			duojileft();
			dz=distance();
			dz=distance();
	
			if(dz>45)
			{
				duojistop();
				turnleft(500);
				gostraight(1);
			}
			else
			{
				duojiright();
				dy=distance();
				dy=distance();
				if(dy>45)
				{
					duojistop();
					turnright(500);
					gostraight(1);
				}
				else
				{
					duojistop();
					turnright(2000);
					gostraight(1);
				}
			}
		}
		else
		{
			stop(1);
			back(150);
			stop(1);
			duojileft();
			dz=distance();
			dz=distance();

			if(dz>45)
			{
				duojistop();
				turnleft(500);
				gostraight(1);
			}
			else
			{
				duojiright();
				dy=distance();
				dy=distance();
				if(dy>45)
				{
					duojistop();
					turnright(500);
					gostraight(1);
				}
				else
				{
					duojistop();
					turnright(2000);
					gostraight(1);
				}
			}
		}
	}	
	return 0;	 
}		




