/******************************************************************
						OLED��������
�������ǻ���IICЭ���OLED����������һ����ά����u8 OLED_GRAM[128][8]
��������OLED����ĸ�����������Ƚ���ʾ�����ݴ���������У�Ȼ��һ��
����OLED��ʾ��

														�����ˣ���˫
														ʱ�䣺2017-1-7
*******************************************************************/

#include "oled.h"
#include "oledfont.h"
#include "delay.h"
#include "string.h"
#include "sys.h"

/*
�������ܣ���ʼ��GPIO��OLED
	��������
  ����ֵ����
*/
void OLED_Init1(void)
{
	//GPIO�ĳ�ʼ��
	GPIO_InitTypeDef GPIO_InitStruture;
	
#if OLED_mood == 0
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��GPIO��ʱ��
	
	GPIO_InitStruture.GPIO_Pin = GPIO_Pin_3;//B8
	GPIO_InitStruture.GPIO_Speed =GPIO_Speed_2MHz;	
	GPIO_InitStruture.GPIO_Mode = GPIO_Mode_Out_PP;
	
	
	GPIO_Init(GPIOA,&GPIO_InitStruture);
	//OLED�ĳ�ʼ��

#else 

	//�˿ڳ�ʼ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE, ENABLE);//ʹ��GPIO��ʱ��
	
	GPIO_InitStruture.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_8 | GPIO_Pin_11;
	GPIO_InitStruture.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruture.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruture.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruture.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOC, &GPIO_InitStruture);
	
	GPIO_InitStruture.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_Init(GPIOD, &GPIO_InitStruture);
	
	GPIO_InitStruture.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOE, &GPIO_InitStruture);
	
	OLED_SPI_VCC = 1;
	OLED_SPI_RES = 1;
	OLED_SPI_CS = 1;
	
#endif

	OLED_Write_Cmd(0xAE);//�ر���ʾ
	OLED_Write_Cmd(0x00);//����ʱ�ӷ�Ƶ���ӣ���Ƶ��
	OLED_Write_Cmd(0x10);//[3:0]��Ƶ���ӣ�[7:4]��Ƶ��
	OLED_Write_Cmd(0x40);
	OLED_Write_Cmd(0xB0);
	OLED_Write_Cmd(0x81);
	OLED_Write_Cmd(0xFF);
		
	OLED_Write_Cmd(0xA1);//��������·��
	OLED_Write_Cmd(0xA6);
	OLED_Write_Cmd(0xA8);
	OLED_Write_Cmd(0x3F);//Ĭ��0x3F
	OLED_Write_Cmd(0xC8);//������ʾƫ��
	OLED_Write_Cmd(0xD3);
	OLED_Write_Cmd(0x00);//Ĭ��Ϊ0
	
	OLED_Write_Cmd(0xD5);//������ʾ��ʼ������
	OLED_Write_Cmd(0x80);
	
	OLED_Write_Cmd(0xD8);
	OLED_Write_Cmd(0x05);
	
	OLED_Write_Cmd(0xD9);//����Ԥ�������
	OLED_Write_Cmd(0xF1);//[3:0]phase 1;[7:4] phase 2
	OLED_Write_Cmd(0xDA);//����VCOMH��ѹ����
	OLED_Write_Cmd(0x12);//[6:4] 000 0.65*vcc;001 0.77*vcc; 011 0.83*vcc
	
	OLED_Write_Cmd(0xDB);//ȫ����ʾ������bit0:1������0�رգ�����/������
	OLED_Write_Cmd(0x31);//������ʾ��ʽ��bit0:1������ʾ��0������ʾ
	
	OLED_Write_Cmd(0x8D);
	OLED_Write_Cmd(0x14);
	OLED_Write_Cmd(0xAF);//������ʾ







//	OLED_Write_Cmd(0x00);//�е�ַ  �� 00-0F
//	OLED_Write_Cmd(0x10);//�е�ַ  �� 10-1F
//	
//	OLED_Write_Cmd(0x40);//��ʾ��ʼ��  
//	OLED_Write_Cmd(0xB0);//--����ҳ��ַ B0-B7
//	
//	OLED_Write_Cmd(0x81); // �Աȶ�  00~FF Խ��Խ��Ŷ
//	OLED_Write_Cmd(0xFF);//--128   
//	
//	OLED_Write_Cmd(0xA1);//���ض�������
//	OLED_Write_Cmd(0xA6);//����A6/����A7 ��ʾģʽ
//	
//	OLED_Write_Cmd(0xA8);//��������·�� 1-64
//	OLED_Write_Cmd(0x3F);//--1/32 duty -----63
//	
//	OLED_Write_Cmd(0xC8);//ɨ�跽ʽ 0xC8 0xC0

//	OLED_Write_Cmd(0xD3);//��ʾƫ��offset
//	OLED_Write_Cmd(0x00);//
//	
//	OLED_Write_Cmd(0xD5);//����ʱ�ӷ�Ƶ����Ƶ��
//	OLED_Write_Cmd(0x80);//[3:0]��Ƶ [7:4]��Ƶ��
//	
//	OLED_Write_Cmd(0xD8);//set area color mode off
//	OLED_Write_Cmd(0x05);//
//	
//	OLED_Write_Cmd(0xD9);//Ԥ�����
//	OLED_Write_Cmd(0xF1);//
//	
//	OLED_Write_Cmd(0xDA);//��������
//	OLED_Write_Cmd(0x12);//��ѡ�����š�������ӳ��
//	
//	OLED_Write_Cmd(0xDB);//set Vcomh(û�Թ�)
//	OLED_Write_Cmd(0x30);//0.83*Vcc  0x20: 0.77*Vcc  0x00: 0.65Vcc
//	
//	OLED_Write_Cmd(0x8D);//��ɱ�
//	OLED_Write_Cmd(0x14);//����  0x10�ر�
//	//delay_ms(100);
//	OLED_Write_Cmd(0xAF);//--turn on oled panel 
//		
//	OLED_Clear();	
//	OLED_SetPos(0,0);
	
	DelayMs(100);
	OLED_Clear();//����
}

#if OLED_mood == 0		//IICģʽ

/*�������ܣ�����������OLED_SDAΪ���ģʽ
	��������
 ����ֵ����
*/
void OLED_SDA_out(void)
{
	GPIO_InitTypeDef GPIO_InitStruture;
	
	GPIO_InitStruture.GPIO_Pin = GPIO_Pin_2;	
	GPIO_InitStruture.GPIO_Mode = GPIO_Mode_Out_PP;//���ģʽ
	GPIO_InitStruture.GPIO_Speed =GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruture);	//PB9��ʼ��
}

/*�������ܣ�����������OLED_SDAΪ����ģʽ
	��������
  ����ֵ����
*/
void OLED_SDA_in(void)
{
	GPIO_InitTypeDef GPIO_InitStruture;
	
	GPIO_InitStruture.GPIO_Pin = GPIO_Pin_2;	
	GPIO_InitStruture.GPIO_Mode =GPIO_Mode_IN_FLOATING ;//����ģʽ
	GPIO_InitStruture.GPIO_Speed =GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruture);	//PB9��ʼ��
}

/*�������ܣ���ʼ����
	��������
  ����ֵ����
*/
void OLED_SDA_Start(void)
{
	OLED_SDA_out();//����OLED_SDAΪ���
	
	OLED_SDA = 1;
	DelayUs(1);
	OLED_SCL = 1;
	DelayUs(1);
	OLED_SDA = 0;
	DelayUs(1);;
	OLED_SCL = 0;
}

/*�������ܣ���������
	��������
  ����ֵ����
*/
void OLED_SDA_Stop(void)
{
	OLED_SDA_out();//����OLED_SDAΪ���
	
	OLED_SDA = 0;
	DelayUs(1);;
	OLED_SCL = 1;
	DelayUs(1);
	OLED_SDA = 1;
	DelayUs(1);
	OLED_SCL = 0;
}

/*
�������ܣ�������һ���ֽں󣬵ȴ�OLED��Ӧ�𣬱�ʾOLED����
	��������
  ����ֵ��0����ʾӦ����ȷ��1��ʾӦ�����
*/
u8 OLED_Ask(void)
{
	OLED_SDA_in();//����GPIOB8Ϊ����
	OLED_SCL = 1;
	DelayUs(1);
	OLED_SCL = 0;
	
	return 0;
}

/*�������ܣ�OLED_SDA����һ���ֽ�
	������data:������ֽ�����
  ����ֵ����
*/
void OLED_WriteByte(u8 data)
{
	u8 i;
	
	OLED_SDA_out();
	for (i = 0; i < 8; i++)//����ÿһλ������
	{
		OLED_SCL = 0;
		DelayUs(1);
		if (data & 0x80)	//�ȴ���λ
		{
			OLED_SDA = 1;
		}
		else
		{
			OLED_SDA = 0;
		}
		
		DelayUs(1);
		OLED_SCL = 1;
		DelayUs(1);;
		data = data << 1;
	}
	
	OLED_SCL = 0;
}

/*�������ܣ�д����
	������cmd:��������
  ����ֵ����
*/
void OLED_Write_Cmd(u8 cmd)
{
	OLED_SDA_Start();
	OLED_WriteByte(0x78);//OLED�ĵ�ַ
	OLED_Ask();
	OLED_WriteByte(0x00);//����Ĵ����ĵ�ַ
	OLED_Ask();
	OLED_WriteByte(cmd);//����
	OLED_Ask();
	OLED_SDA_Stop();	
}

/*
�������ܣ�OLEDд������
	������data����
  ����ֵ����
*/
void OLED_Write_Data(u8 data)
{
	OLED_SDA_Start();
	OLED_WriteByte(0x78);//OLED�ĵ�ַ
	OLED_Ask();
	OLED_WriteByte(0x40);//���ݼĴ����ĵ�ַ
	OLED_Ask();
	OLED_WriteByte(data);//��������
	OLED_Ask();
	OLED_SDA_Stop();	
}

#else

/*�������ܣ�OLED_SDA����һ���ֽ�
	������data:������ֽ�����
  ����ֵ����
*/
void OLED_WriteByte(u8 data)
{
	u8 i;
	
	for (i = 0; i < 8; i++)//����ÿһλ������
	{
		OLED_SPI_SCLK = 0;
		DelayUs(1);
		if (data & 0x80)	//�ȴ���λ
		{
			OLED_SPI_SDA = 1;
		}
		else
		{
			OLED_SPI_SDA = 0;
		}
		
		DelayUs(1);
		OLED_SPI_SCLK = 1;
		DelayUs(1);
		data = data << 1;
	}
	
	OLED_SPI_SCLK = 0;
}

/*�������ܣ�д����
	������cmd:��������
  ����ֵ����
*/
void OLED_Write_Cmd(u8 cmd)
{
	OLED_SPI_DC = 0;//������
	DelayUs(1);
	OLED_SPI_CS = 0;//Ƭѡ
	DelayUs(1);
	OLED_WriteByte(cmd);	
	OLED_SPI_CS = 1;
	DelayUs(1);
}
	
/*
�������ܣ�OLEDд������
	������data����
  ����ֵ����
*/	
void OLED_Write_Data(u8 data)
{
	OLED_SPI_CS = 0;//Ƭѡ
	DelayUs(1);
	OLED_SPI_DC = 1;//������
	DelayUs(1);
	OLED_WriteByte(data);		
	OLED_SPI_CS = 1;
	DelayUs(1);
}
#endif

/*
�������ܣ�����OLED��ʾ�����겢���õ����/Ϩ�𡣻���
	����������(x,y),mode:1����0Ϩ��
  ����ֵ����
*/
void OLED_SetPos(u8 x, u8 y)
{
	if (x > 127 || y > 63)	//������ʾ��Χ
	{
		return;
	}
	y = y / 8;
	
	OLED_Write_Cmd(0xb0+y);//����ҳ����
	OLED_Write_Cmd(((0xf0&x)>>4)+0x10);//������ʾλ�õ��иߵ�ַ
	OLED_Write_Cmd((0x0f&x)+ 0x00);//������ʾλ�õ��е͵�ַ
}

/*
�������ܣ���OLED������OLED�������л���
	��������
  ����ֵ����
*/
void OLED_On(void)
{
	OLED_Write_Cmd(0X8D);//���õ�ɱ�
	OLED_Write_Cmd(0X14);//������ɱ�
	OLED_Write_Cmd(0XAF);//OLED����
}

/*
�������ܣ��ر�OLED��ʾ
	��������
  ����ֵ����
*/
void OLED_Off(void)
{
	OLED_Write_Cmd(0X8D);//���õ�ɱ�
	OLED_Write_Cmd(0X10);//�رյ�ɱ�
	OLED_Write_Cmd(0XAE);//OLED�ر�
}

/*
�������ܣ��ڶԽ�(x1,y1)��(x2,y2)��������ȫ������ȫ��
	�������Խ����꣬mode:1����0��
  ����ֵ����
*/
void OLED_Fill1(u8 x1, u8 y1, u8 x2, u8 y2, u8 mode)
{
	u16 i, flag;
	u8 x, y, flag1;
	
	x = x2 -x1;
	y = y2-y1;
	flag1 = y / 8;
	flag = flag1 * x;
	 
	if (mode)
	{
		mode = 0xff;
	}	
	
	for (i = 0; i < flag; i++)
	{
		OLED_SetPos(x1+(i/flag1),y1+8*(i%flag1));
		OLED_Write_Data(mode);
	}
}

/*
�������ܣ�����
	��������
  ����ֵ����
*/
void OLED_Clear(void)
{
	u8 i, j;

	for (i = 0; i < 8; i++)
	{
		OLED_Write_Cmd(0xb0 + i);//����ҳ����
		OLED_Write_Cmd(0x00);//������ʾλ�õ��е͵�ַ
		OLED_Write_Cmd(0x10);//������ʾλ�õ��иߵ�ַ
		for (j = 0; j < 128; j++)
		{
			OLED_Write_Data(0x00);
		}
	}
}

/*
�������ܣ���x��y����ʾ�ַ�
	������x��y�У�ch��ʾ���ַ���size�ַ��Ĵ�С
  ����ֵ����
*/
void OLED_Show_Char(u8 x, u8 y, u8 ch, u8 size)
{
	u8 temp,i;
	
	ch = ch - 32;	//�õ�ƫ�ƺ��ֵ
	
	if (size == 12)
	{
		for (i = 0; i < 12; i++)
		{
			temp = asc2_1206[ch][i];//����1206�����ֿ�
			OLED_SetPos(x+(i/2),y+8*(i%2));
			OLED_Write_Data(temp);
		}
	}
	else if (size == 16)
	{
		for (i = 0; i < 16; i++)
		{
			temp = asc2_1608[ch][i];//����1608�����ֿ�
			OLED_SetPos(x+(i/2),y+8*(i%2));
			OLED_Write_Data(temp);
		}
	}
	else if (size == 24)
	{
		for (i = 0; i< 36; i++)
		{
			temp = asc2_2412[ch][i];//����2412�����ֿ�
			OLED_SetPos(x+(i/3),y+8*(i%3));
			OLED_Write_Data(temp);
		}
	}
	else 
		return;
}


/*
�������ܣ�����m^n
*/
u32 Getpow(u8 m, u8 n)
{
	u8 i;
	u32 sum = 1;
	
	for (i = 0; i < n;i++)
	{
		sum *= m;
	}
	
	return sum;
}

/*
�������ܣ���ʾ�޷���int������
	������xy���꣬num��ʾ���֣�size���ִ�Сģʽ����ʾ������Ϊ0~4294967295
  ����ֵ����
*/
void OLED_Show_UnIntNum(u8 x, u8 y, u32 num, u8 figure, u8 size)
{
	u8 i = 0, len = 1, temp, length;
	u32 num1;
	int k;
	
	num1 = num;
	while(num1 > 9)	//��num��λ��
	{
		len++;
		num1 /= 10;
	}
	length = len;
	while (len)
	{
		temp = num / Getpow(10, len - 1);
		
		OLED_Show_Char(x+(size/2)*i, y, temp + '0', size);//ѭ����ʾÿλ�����ַ����Ӹ�λ��ʾ
		num = num % Getpow(10, len - 1);
		i++;
		len--;
	}
	//û�����ֵ�λ�ÿո���(�����������ʾ������ֺ���ʾ�Ƚ�С�����־Ͳ���ȥ�����������)
	i=0;
	k = figure - length;
	while(k>0)
	{
		OLED_Show_Char((x+(size/2)*length)+(size/2)*i, y, ' ', size);//ѭ����ʾ�ո񣬴Ӹ�λ��ʾ
		i++;
		k--;
	}
}

/*��ʾint������,������*/
void OLED_Show_IntNum(u8 x, u8 y, int num, u8 figure, u8 size)
{
	if (num < 0)
	{
		OLED_Show_Char(x, y, '-', size);
		num = 0-num;
		OLED_Show_UnIntNum(x+(size/2), y,num,figure+1,size);
		return;
	}
	OLED_Show_UnIntNum(x, y,num,figure,size);
}

/*
�������ܣ���ʾ�޷���float����
	������xy���꣬num��ʾ���֣�precise��ʾ���ֵľ��ȣ�size���ִ�Сģʽ
  ����ֵ����
*/
void OLED_Show_UnFloatNum(u8 x, u8 y, double num, int precise, u8 size)
{
	u8 i = 0, len = 1;
	double num1;
	u32 integer;
	double decimal;
	
	integer = (int)num;//��������
	decimal = (double)(num - integer);//С������
	
	num1 = num;
	while(num1 > 9)	//��num������λ��
	{
		len++;
		num1 /= 10;
	}
	OLED_Show_IntNum(x,y,integer,len,size);//��ʾ��������
	OLED_Show_Char(x+(size/2)*len,y,'.',size);//��ʾС����
	
	x = x+(size/2)*(len+1);
	while (precise)//��ʾ��λС��
	{
		decimal = decimal * 10;
		integer = (int)decimal;//ȡ��һλС��
		decimal = (double)(decimal - integer);
		OLED_Show_Char(x+(size/2)*i, y, integer + '0', size);//ѭ����ʾÿλ�����ַ����Ӹ�λ��ʾ
		i++;
		precise--;
	}
}

/*��ʾfloat������,������*/
void OLED_Show_FloatNum(u8 x, u8 y, double num, int precise, u8 size)
{
	if (num < 0)
	{
		OLED_Show_Char(x, y, '-', size);
		num = 0-num;
		OLED_Show_UnFloatNum(x+(size/2),y,num,precise,size);
		return;
	}
	OLED_Show_UnFloatNum(x,y,num,precise+1,size);
}

/*
�������ܣ���ʾ�ַ���
	������x��y�У�p��Ҫ��ʾ���ַ���ָ�룬TextSize��ģ�Ĵ�С
  ����ֵ����
*/
void OLED_Show_Str(u8 x, u8 y, const u8 *p, u8 TextSize)
{
	while((*p <= '~')&&(*p >= ' '))		//�ж��ǲ��ǷǷ��ַ�!
    {       
        if(x>(128-(TextSize/2)))//�ж��ַ�������ʾ��Χû
		{
			x=0;
			y+=TextSize;
		}
        if(y>(64-TextSize))//�ж��ַ�������ʾ��Χû
		{
			y=x=0;
			OLED_Clear();
		}
		OLED_Show_Char(x, y, *p, TextSize);//��ʾ����ַ�
        x += TextSize/2;
        p++;
    }  
}

/*
�������ܣ���ʾ��СΪ��12����12�ĺ���
	C��������ʼ��ʾ���꣬�����ַ���str
  ����ֵ����
*/
void OLED_Show_Font12X12(u8 x, u8 y, u8 str[])
{
	u8 i, k, j,temp;
	
	for (i = 0; str[i] != '\0'; i++)
	{
		for (k = 0; k < 25; k++)//���ֿ���Ѱ�Һ���
		{
			if ((str[i] == font12x12[k].Index[0]) && (str[++i] == font12x12[k].Index[1]))//�ֿ������ж�Ӧ�ĺ���
			{
				for (j = 0; j < 24; j++)
				{
					temp = font12x12[k].Msk[j];
					OLED_SetPos(x+(j/2),y+8*(j%2));
					OLED_Write_Data(temp);					
				}
			}
		}
	}
}


void OLED_Show_Many_Chinese12X12(unsigned char x, unsigned char y, unsigned char *chr)
{
	unsigned char j = 0, ch[2];
	while (chr[j]!='\0')
	{
		ch[0] = chr[j];
		ch[1] = chr[j + 1];
		OLED_Show_Font12X12(x,y,ch);
			x+=12;
		if(x>200){x=0;y+=2;}
			j+=2;
	}
}



/*
�������ܣ���ʾ��СΪ��12����16�ĺ���
	C��������ʼ��ʾ���꣬�����ַ���str
  ����ֵ����
*/
void OLED_Show_Font12X16(u8 x, u8 y, u8 *str)
{
	u8 i, k, j, temp;
	
	for (i = 0; str[i] != '\0'; i++)
	{
		for (k = 0; k < 25; k++)//���ֿ���Ѱ�Һ���
		{
			if ((str[i] == font12x16[k].Index[0]) && (str[++i] == font12x16[k].Index[1]))//�ֿ������ж�Ӧ�ĺ���
			{
				for (j = 0; j < 24; j++)
				{
					temp = font12x16[k].Msk[j];
					OLED_SetPos(x+(j/2),y+8*(j%2));
					OLED_Write_Data(temp);					
				}
			}
		}
	}
}

/*
�������ܣ���ʾ��СΪ��16����16�ĺ���
	C��������ʼ��ʾ���꣬�����ַ���str
  ����ֵ����
*/
void OLED_Show_Font16X16(u8 x, u8 y, u8 *str)
{
	u8 i, k, j,temp;
	
	for (i = 0; str[i] != '\0'; i+2)
	{
		for (k = 0; k < 25; k++)//���ֿ���Ѱ�Һ���
		{
			if ((str[i] == font16x16[k].Index[0]) && (str[i+1] == font16x16[k].Index[1]))//�ֿ������ж�Ӧ�ĺ���
			{
				for (j = 0; j < 32; j++)
				{
					temp = font16x16[k].Msk[j];
					OLED_SetPos(x+(j/2),y+8*(j%2));
					OLED_Write_Data(temp);			
				}
				x+=8;
			}
		}
	}
}

/*
�������ܣ���ʾͼƬ
	����������(x,y),ͼƬ���picture
*/
void OLED_Show_Picture(u8 x, u8 y, u8 picture)
{
	u8 j, k, temp;
	int i;
	
	for (k = 0; k < 25; k++)
	{
		if (picture == PICTURE[k].mark)
		{	
			for (i = 0; i < 288; i++)
			{
				temp = PICTURE[k].Msk[i];
				OLED_SetPos(x+(j/2),y+8*(j%2));
				OLED_Write_Data(temp);
			}
		}
	}
}





