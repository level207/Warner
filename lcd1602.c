

/*******************************�����ַ�����********************************/

unsigned char code dis1[] = {"www.hificat.com"};
unsigned char code dis2[] = {"0571-85956028"};

/*****************************************************************************
�������ƣ�lcd_bz
�������ܣ����lcd��ʾ�Ƿ�æ
��ڲ���: ��    
���ڲ�����result��æΪ1����Ϊ0
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
�������ƣ�lcd_wcmd
�������ܣ�lcdдָ��
��ڲ���: cmd    
���ڲ�������
*****************************************************************************/

void lcd_wcmd(uchar cmd)
{
	while(lcd_bz());//�ж�LCD�Ƿ�æµ
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
�������ƣ�lcd_wdat
�������ܣ�lcdд����
��ڲ���: dat    
���ڲ�������
*****************************************************************************/

void lcd_wdat(uchar dat)
{
	while(lcd_bz());//�ж�LCD�Ƿ�æµ
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
�������ƣ�lcd_disp_char
�������ܣ�lcd��ʾһ���ַ�
��ڲ���: x��y��dat   
���ڲ�������
*****************************************************************************/

void lcd_disp_char(uchar x,uchar y,uchar dat)	   // yΪ1ʱ���ǵ�һ�У�����Ϊ�ڶ���

{
	uchar address ;
	if(y==1)
	{
		address=0x80+x ;  //��ʾ�ڵ�һ�ŵ�ʱ��x�ĵ�ַ
	}
	else
	{
		address=0xc0+x ;   //��ʾ�ڵڶ��ŵ�ʱ��x�ĵ�ַ
	}

	lcd_wcmd(address);
	lcd_wdat(dat);

}

/*****************************************************************************
�������ƣ�lcd_disp_str
�������ܣ�lcd��ʾһ���ַ���
��ڲ���: column��line��*str   
���ڲ�������
*****************************************************************************/

void LCD_disp_str(uchar column,uchar line,uchar *str)
{
	uchar n=0;

	while(*str!='\0')
	{
		lcd_disp_char(column++,line,*str++);
	}

}

/*****************************************************************************
�������ƣ�lcd_init
�������ܣ�lcd��ʼ��
��ڲ���: ��    
���ڲ�������
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