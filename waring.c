/**************************头文件、宏定义、函数声明*************************/

#include <reg52.h>
#include <intrins.h>
#include <stdio.h>
#include <string.h>

#define uint unsigned int
#define uchar unsigned char
#define MAXCHAR 81

void delay(uchar ms);
void Ini_UART(void);
void ser();
void clearBuff(void);
void Print_Char(uchar ch);
void Print_Str(uchar *str);
void led(int i);

void AT(void);
bit lcd_bz(void);
void lcd_wcmd(uchar cmd);
void lcd_disp_char(uchar x,uchar y,uchar dat);
void lcd_disp_str(uchar column,uchar line,uchar *str);
void lcd_wdat(uchar dat);
void lcd_init(void);
int get_mq9(void);

/*********************************定义数组**********************************/

uchar  aa[MAXCHAR];
uchar j=0;
code uchar ATE0[]="ATE0\r\n";			   //程序初始化AT部分首先关闭回显
code uchar CREG_CMD[]="AT+CREG?\r\n";	   //查询网络注册情况
code uchar SMS_send[]="AT+CMGS=23\r\n";	   //八位位组代码数目
code uchar ATCN[]="AT+CNMI=2,1\r\n";	   //短信存入SIM卡
code uchar CMGF[]="AT+CMGF=0\r\n";		   //消息格式为PDU,中文短信
code uchar CMGR[12]="AT+CMGR=1\r\n";	   //读取第一条消息
code uchar CMGD[12]="AT+CMGD=1\r\n";	   //删除第一条消息
code uchar CMGS[]="AT+CMGS=\"18604042139\"\r\n";
/*****************************************************************************
1.如果你的晶振是11.0592M
只需要修改下面的号码就可以了,给成你手上拿着的手机的号码


修改方法   在下面这段字符中找到 5129021411F5 
 
 其实5129021411F5 --> 15922041115
18622044083 8126924480F3
														   8106042431F9

 看明白了吗  电话是两位两位颠倒 将您手上的手机号码替换即可
*****************************************************************************/
uchar  code Sms2_Pdu[]="0891683108200405F011000D91688106894237F40008000862116210529F4E86";

/*********************************端口定义**********************************/

sbit bee= P1^1;
sbit d0 = P2^4;//mq9输入端口
sbit rs = P2^5;
sbit rw = P2^6;
sbit ep = P2^7;
sbit bee_off=P3^2;

/*****************************************************************************
函数名称：delay
函数功能: LCD延时子程序
入口参数: ms
出口参数: 无
*****************************************************************************/

void delay(uchar ms)
{
	uchar i;

	while(ms--)
	{
		for(i = 0; i< 250; i++)
		{
			_nop_();
			_nop_();
			_nop_();
			_nop_();

		}
	}
}

/*****************************************************************************
函数名称：Ini_UART
函数功能：串口初始化、定时器初始化
入口参数: 无     
出口参数：无
*****************************************************************************/

void Ini_UART(void)//串口初始化、定时器初始化
{
    SCON = 0x50 ;  //SCON: serail mode 1, 8-bit UART, enable ucvr
    //UART为模式1，8位数据，允许接收
    TMOD |= 0x20 ; //TMOD: timer 1, mode 2, 8-bit reload
    //定时器1为模式2,8位自动重装
    PCON |= 0x80 ; //SMOD=1;
    TH1 = 0xFA ;   //Baud:19200 fosc="11".0592MHz
    TL1=0xFA;
    IE |= 0x90 ;     //Enable Serial Interrupt
    TR1 = 1 ;       // timer 1 run
    TI=1;
    ES=1;
}

/*****************************************************************************
函数名称：ser() interrupt 4
函数功能：串口中断函数，串口有数据传输时进入此中断
入口参数: 无     
出口参数：无
*****************************************************************************/

void ser() interrupt 4
{

    if(RI==1)
    {  
		aa[j]=SBUF;//命令存到命令数组
		RI=0; //软件清除接收中断
        j++;

    }
}

/*****************************************************************************
函数名称：clearBuff
函数功能: 清楚串口缓冲区数据
入口参数: 无
出口参数: 无
*****************************************************************************/

void clearBuff(void)
{

    for(j=0;j<MAXCHAR;j++)
    {
        aa[j]=0x00;
    }
    j=0;
}

/*****************************************************************************
函数名称：Print_Char
函数功能：发送单个字符
入口参数: ch      
出口参数：无
*****************************************************************************/

void Print_Char(uchar ch)
{
    SBUF=ch; //送入缓冲区
    while(TI!=1); //等待发送完毕
    TI=0; //软件清零

}

/*****************************************************************************
函数名称：Print_Str
函数功能：发送字符串
入口参数: *str    
出口参数：无
*****************************************************************************/

void Print_Str(uchar *str)
{
	while(*str!='\0')
    {
        Print_Char(*str);
        delay(2);
        str++;

    }
}

/*****************************************************************************
函数名称：led
函数功能: 初始化的时候控制led亮灭看初始化是否成功
入口参数: i
出口参数: 无
*****************************************************************************/

void led(int i)
{
    P2 |= i;
    delay(20);
    P2 &= ~i;
    delay(20);
    P2 |= i;
    delay(20);
    P2 &= ~i;
}

/*****************************************************************************
函数名称：lcd_bz
函数功能：检测lcd显示是否繁忙
入口参数: 无    
出口参数：result，忙为1，闲为0
*****************************************************************************/

bit lcd_bz(void)
{
	bit result;
	rs = 0;
	rw = 1;
	ep = 1;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	result = (bit)(P0 & 0x80);
	ep = 0;
	return result;
}

/*****************************************************************************
函数名称：lcd_wcmd
函数功能：lcd写指令
入口参数: cmd    
出口参数：无
*****************************************************************************/

void lcd_wcmd(uchar cmd)
{
	while(lcd_bz());//判断LCD是否忙碌
	rs = 0;
	rw = 0;
	ep = 0;
	_nop_();
	_nop_();
	P0 = cmd;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	ep = 1;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	ep = 0;

}

/*****************************************************************************
函数名称：lcd_wdat
函数功能：lcd写数据
入口参数: dat    
出口参数：无
*****************************************************************************/

void lcd_wdat(uchar dat)
{
	while(lcd_bz());//判断LCD是否忙碌
	rs = 1;
	rw = 0;
	ep = 0;
	P0 = dat;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	ep = 1;
	_nop_();
	_nop_();
	_nop_();
	_nop_();
	ep = 0;

}

/*****************************************************************************
函数名称：lcd_disp_char
函数功能：lcd显示一个字符
入口参数: x，y，dat   
出口参数：无
*****************************************************************************/

void lcd_disp_char(uchar x,uchar y,uchar dat)	   // y为1时候是第一行，否则为第二行

{
	uchar address ;
	if(y==1)
	{
		address=0x80+x ;  //显示在第一排的时候x的地址
	}
	else
	{
		address=0xc0+x ;   //显示在第二排的时候x的地址
	}

	lcd_wcmd(address);
	lcd_wdat(dat);

}

/*****************************************************************************
函数名称：lcd_disp_str
函数功能：lcd显示一个字符串
入口参数: column，line，*str   
出口参数：无
*****************************************************************************/

void lcd_disp_str(uchar column,uchar line,uchar *str)
{
	uchar n=0;

	while(*str!='\0')
	{
		lcd_disp_char(column++,line,*str++);
	}

}

/*****************************************************************************
函数名称：lcd_init
函数功能：lcd初始化
入口参数: 无    
出口参数：无
*****************************************************************************/

void lcd_init(void)
{
	lcd_wcmd(0x38);
	delay(1);
	lcd_wcmd(0x0c);
	delay(1);
	lcd_wcmd(0x06);
	delay(1);
	lcd_wcmd(0x01);
	delay(1);

}

/*****************************************************************************
函数名称：AT
函数功能：初始化TC35模块
入口参数: 无    
出口参数：无
*****************************************************************************/

void AT(void)
{
	
	clearBuff();
    Print_Str(ATE0);//程序初始化AT部分首先关闭回显
    delay(50);

	while(strstr(aa,"OK")==NULL)
    {
        delay(50);
        led(0x01);
        clearBuff();
        Print_Str(ATE0);
    }
	led(0x02);

        Print_Str(ATCN);//短信存入SIM卡
        delay(50);

        Print_Str(CMGF);//消息格式为PDU,中文短信
        delay(100);

	while(1)
    {
        clearBuff();
        Print_Str(CREG_CMD);//查询网络注册情况
        delay(50);
        if(((aa[9]=='0')&&(aa[11]=='1'))||((aa[9]=='0')&&(aa[11]=='5')))
        {
            clearBuff();
            led(0x08);
            break;
        }
        else
        {
            clearBuff();
            led(0x04);
            delay(50);
        }
	}
}

/*****************************************************************************
函数名称：mq9_dat
函数功能：mq9传入数据
入口参数: 无    
出口参数：dat
*****************************************************************************/

int get_mq9(void)
{	 
	int mq9;

	mq9=(d0&1);
	delay(25);
	clearBuff();

	return  mq9;
}

/*****************************************************************************
函数名称：main
函数功能：主函数
入口参数: 无    
出口参数：无
*****************************************************************************/

void main()
{
	int mq9_dat=0,count=1;

	lcd_init();    //初始化LCD

	lcd_disp_str(0,1,"Init UART..."); //初始化串口
	Ini_UART();    //初始化串口
	clearBuff();
	delay(100);

	
	lcd_wcmd(0x01);
	lcd_disp_str(0,1,"Init TC35..."); //初始化TC35
	AT();		   //初始化TC35

	lcd_disp_str(0,1,"Init MQ-9..."); //初始化MQ-9 预热5秒
	delay(250);

	while(count)
	{	
		if((mq9_dat=get_mq9())==1)
		{	
			count=1;
			bee=1;
			lcd_wcmd(0x01);
			lcd_disp_str(0,1,"Gas-Detect:");		
			lcd_disp_str(0,2,"Normal   <100ppm");
			delay(250);	
		}

		if((mq9_dat=get_mq9())==0)
		{
			lcd_wcmd(0x01);
			lcd_disp_str(0,1,"Gas-Detect:");
			lcd_disp_str(0,2,"Warning >=100ppm");
			delay(250);
		}

		if((mq9_dat=get_mq9())==0)
		{
			lcd_wcmd(0x01);
			lcd_disp_str(0,1,"Gas-Detect:");
			lcd_disp_str(0,2,"Warning >=100ppm");
 			bee=0;


			lcd_disp_str(0,1,"Make Calling...");//打电话
			lcd_disp_str(0,2,"Warning >=100ppm");
			delay(10);
			Print_Str("ATD13898106410;\r\n"); 
			delay(250);
			delay(250);
			Print_Str("ATA\r\n");
			delay(20);

			lcd_wcmd(0x01);
			lcd_disp_str(0,1,"Send Message...");
			lcd_disp_str(0,2,"Warning >=100ppm");

			Print_Str("AT+CMGF=1\r\n");//发送短信
			delay(20);
			led(0x04);
			Print_Str("AT+CMGS=\"13898106410\"\r\n");
			delay(10);
			Print_Str("Warning! Gas leakage!!!");
			Print_Char(0x1A);
			Print_Str("\r\n");
			delay(250);
			delay(250);

			count--;			
	
		}
	}

	while(1)
	{
		if(bee_off==0)	bee=1;//有按键按下就关闭蜂鸣器
		lcd_wcmd(0x01);
		lcd_disp_str(0,1,"Detect Warning!!");
		lcd_disp_str(0,2," Gas Leakage!!!");
		delay(250);
	}
}


