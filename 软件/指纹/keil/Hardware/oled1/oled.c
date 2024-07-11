/******************************************************************
						OLED驱动程序
本驱动是基于IIC协议的OLED驱动，定义一个二维数组u8 OLED_GRAM[128][8]
来存整个OLED点阵的各个点的亮灭。先将显示的数据存入该数组中，然后一起
传到OLED显示。

														制作人：胡双
														时间：2017-1-7
*******************************************************************/

#include "oled.h"
#include "oledfont.h"
#include "delay.h"
#include "string.h"
#include "sys.h"

/*
函数功能：初始化GPIO、OLED
	参数：无
  返回值：无
*/
void OLED_Init1(void)
{
	//GPIO的初始化
	GPIO_InitTypeDef GPIO_InitStruture;
	
#if OLED_mood == 0
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能GPIO的时钟
	
	GPIO_InitStruture.GPIO_Pin = GPIO_Pin_3;//B8
	GPIO_InitStruture.GPIO_Speed =GPIO_Speed_2MHz;	
	GPIO_InitStruture.GPIO_Mode = GPIO_Mode_Out_PP;
	
	
	GPIO_Init(GPIOA,&GPIO_InitStruture);
	//OLED的初始化

#else 

	//端口初始化
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC|RCC_AHB1Periph_GPIOD|RCC_AHB1Periph_GPIOE, ENABLE);//使能GPIO的时钟
	
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

	OLED_Write_Cmd(0xAE);//关闭显示
	OLED_Write_Cmd(0x00);//设置时钟分频因子，震荡频率
	OLED_Write_Cmd(0x10);//[3:0]分频因子；[7:4]震荡频率
	OLED_Write_Cmd(0x40);
	OLED_Write_Cmd(0xB0);
	OLED_Write_Cmd(0x81);
	OLED_Write_Cmd(0xFF);
		
	OLED_Write_Cmd(0xA1);//设置驱动路数
	OLED_Write_Cmd(0xA6);
	OLED_Write_Cmd(0xA8);
	OLED_Write_Cmd(0x3F);//默认0x3F
	OLED_Write_Cmd(0xC8);//设置显示偏移
	OLED_Write_Cmd(0xD3);
	OLED_Write_Cmd(0x00);//默认为0
	
	OLED_Write_Cmd(0xD5);//设置显示开始的行数
	OLED_Write_Cmd(0x80);
	
	OLED_Write_Cmd(0xD8);
	OLED_Write_Cmd(0x05);
	
	OLED_Write_Cmd(0xD9);//设置预充电周期
	OLED_Write_Cmd(0xF1);//[3:0]phase 1;[7:4] phase 2
	OLED_Write_Cmd(0xDA);//设置VCOMH电压倍率
	OLED_Write_Cmd(0x12);//[6:4] 000 0.65*vcc;001 0.77*vcc; 011 0.83*vcc
	
	OLED_Write_Cmd(0xDB);//全局显示开启；bit0:1开启；0关闭（黑屏/白屏）
	OLED_Write_Cmd(0x31);//设置显示方式；bit0:1反相显示；0正常显示
	
	OLED_Write_Cmd(0x8D);
	OLED_Write_Cmd(0x14);
	OLED_Write_Cmd(0xAF);//开启显示







//	OLED_Write_Cmd(0x00);//列地址  低 00-0F
//	OLED_Write_Cmd(0x10);//列地址  高 10-1F
//	
//	OLED_Write_Cmd(0x40);//显示起始行  
//	OLED_Write_Cmd(0xB0);//--设置页地址 B0-B7
//	
//	OLED_Write_Cmd(0x81); // 对比度  00~FF 越大越亮哦
//	OLED_Write_Cmd(0xFF);//--128   
//	
//	OLED_Write_Cmd(0xA1);//段重定义设置
//	OLED_Write_Cmd(0xA6);//正常A6/反向A7 显示模式
//	
//	OLED_Write_Cmd(0xA8);//设置驱动路数 1-64
//	OLED_Write_Cmd(0x3F);//--1/32 duty -----63
//	
//	OLED_Write_Cmd(0xC8);//扫描方式 0xC8 0xC0

//	OLED_Write_Cmd(0xD3);//显示偏移offset
//	OLED_Write_Cmd(0x00);//
//	
//	OLED_Write_Cmd(0xD5);//设置时钟分频、震荡频率
//	OLED_Write_Cmd(0x80);//[3:0]分频 [7:4]震荡频率
//	
//	OLED_Write_Cmd(0xD8);//set area color mode off
//	OLED_Write_Cmd(0x05);//
//	
//	OLED_Write_Cmd(0xD9);//预充电期
//	OLED_Write_Cmd(0xF1);//
//	
//	OLED_Write_Cmd(0xDA);//引脚配置
//	OLED_Write_Cmd(0x12);//可选的引脚、不可重映射
//	
//	OLED_Write_Cmd(0xDB);//set Vcomh(没试过)
//	OLED_Write_Cmd(0x30);//0.83*Vcc  0x20: 0.77*Vcc  0x00: 0.65Vcc
//	
//	OLED_Write_Cmd(0x8D);//电荷泵
//	OLED_Write_Cmd(0x14);//开启  0x10关闭
//	//delay_ms(100);
//	OLED_Write_Cmd(0xAF);//--turn on oled panel 
//		
//	OLED_Clear();	
//	OLED_SetPos(0,0);
	
	DelayMs(100);
	OLED_Clear();//清屏
}

#if OLED_mood == 0		//IIC模式

/*函数功能：设置数据线OLED_SDA为输出模式
	参数：无
 返回值：无
*/
void OLED_SDA_out(void)
{
	GPIO_InitTypeDef GPIO_InitStruture;
	
	GPIO_InitStruture.GPIO_Pin = GPIO_Pin_2;	
	GPIO_InitStruture.GPIO_Mode = GPIO_Mode_Out_PP;//输出模式
	GPIO_InitStruture.GPIO_Speed =GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruture);	//PB9初始化
}

/*函数功能：设置数据线OLED_SDA为输入模式
	参数：无
  返回值：无
*/
void OLED_SDA_in(void)
{
	GPIO_InitTypeDef GPIO_InitStruture;
	
	GPIO_InitStruture.GPIO_Pin = GPIO_Pin_2;	
	GPIO_InitStruture.GPIO_Mode =GPIO_Mode_IN_FLOATING ;//输入模式
	GPIO_InitStruture.GPIO_Speed =GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruture);	//PB9初始化
}

/*函数功能：开始传输
	参数：无
  返回值：无
*/
void OLED_SDA_Start(void)
{
	OLED_SDA_out();//设置OLED_SDA为输出
	
	OLED_SDA = 1;
	DelayUs(1);
	OLED_SCL = 1;
	DelayUs(1);
	OLED_SDA = 0;
	DelayUs(1);;
	OLED_SCL = 0;
}

/*函数功能：结束传输
	参数：无
  返回值：无
*/
void OLED_SDA_Stop(void)
{
	OLED_SDA_out();//设置OLED_SDA为输出
	
	OLED_SDA = 0;
	DelayUs(1);;
	OLED_SCL = 1;
	DelayUs(1);
	OLED_SDA = 1;
	DelayUs(1);
	OLED_SCL = 0;
}

/*
函数功能：传输完一个字节后，等待OLED的应答，表示OLED在线
	参数：无
  返回值：0：表示应答正确，1表示应答错误
*/
u8 OLED_Ask(void)
{
	OLED_SDA_in();//设置GPIOB8为输入
	OLED_SCL = 1;
	DelayUs(1);
	OLED_SCL = 0;
	
	return 0;
}

/*函数功能：OLED_SDA传输一个字节
	参数：data:传输的字节数据
  返回值：无
*/
void OLED_WriteByte(u8 data)
{
	u8 i;
	
	OLED_SDA_out();
	for (i = 0; i < 8; i++)//发送每一位的数据
	{
		OLED_SCL = 0;
		DelayUs(1);
		if (data & 0x80)	//先传高位
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

/*函数功能：写命令
	参数：cmd:命令数据
  返回值：无
*/
void OLED_Write_Cmd(u8 cmd)
{
	OLED_SDA_Start();
	OLED_WriteByte(0x78);//OLED的地址
	OLED_Ask();
	OLED_WriteByte(0x00);//命令寄存器的地址
	OLED_Ask();
	OLED_WriteByte(cmd);//命令
	OLED_Ask();
	OLED_SDA_Stop();	
}

/*
函数功能：OLED写入数据
	参数：data数据
  返回值：无
*/
void OLED_Write_Data(u8 data)
{
	OLED_SDA_Start();
	OLED_WriteByte(0x78);//OLED的地址
	OLED_Ask();
	OLED_WriteByte(0x40);//数据寄存器的地址
	OLED_Ask();
	OLED_WriteByte(data);//传输数据
	OLED_Ask();
	OLED_SDA_Stop();	
}

#else

/*函数功能：OLED_SDA传输一个字节
	参数：data:传输的字节数据
  返回值：无
*/
void OLED_WriteByte(u8 data)
{
	u8 i;
	
	for (i = 0; i < 8; i++)//发送每一位的数据
	{
		OLED_SPI_SCLK = 0;
		DelayUs(1);
		if (data & 0x80)	//先传高位
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

/*函数功能：写命令
	参数：cmd:命令数据
  返回值：无
*/
void OLED_Write_Cmd(u8 cmd)
{
	OLED_SPI_DC = 0;//传命令
	DelayUs(1);
	OLED_SPI_CS = 0;//片选
	DelayUs(1);
	OLED_WriteByte(cmd);	
	OLED_SPI_CS = 1;
	DelayUs(1);
}
	
/*
函数功能：OLED写入数据
	参数：data数据
  返回值：无
*/	
void OLED_Write_Data(u8 data)
{
	OLED_SPI_CS = 0;//片选
	DelayUs(1);
	OLED_SPI_DC = 1;//传数据
	DelayUs(1);
	OLED_WriteByte(data);		
	OLED_SPI_CS = 1;
	DelayUs(1);
}
#endif

/*
函数功能：设置OLED显示的坐标并将该点点亮/熄灭。画点
	参数：坐标(x,y),mode:1点亮0熄灭
  返回值：无
*/
void OLED_SetPos(u8 x, u8 y)
{
	if (x > 127 || y > 63)	//超出显示范围
	{
		return;
	}
	y = y / 8;
	
	OLED_Write_Cmd(0xb0+y);//设置页坐标
	OLED_Write_Cmd(((0xf0&x)>>4)+0x10);//设置显示位置的列高地址
	OLED_Write_Cmd((0x0f&x)+ 0x00);//设置显示位置的列低地址
}

/*
函数功能：打开OLED，即将OLED从休眠中唤醒
	参数：无
  返回值：无
*/
void OLED_On(void)
{
	OLED_Write_Cmd(0X8D);//设置电荷泵
	OLED_Write_Cmd(0X14);//开启电荷泵
	OLED_Write_Cmd(0XAF);//OLED唤醒
}

/*
函数功能：关闭OLED显示
	参数：无
  返回值：无
*/
void OLED_Off(void)
{
	OLED_Write_Cmd(0X8D);//设置电荷泵
	OLED_Write_Cmd(0X10);//关闭电荷泵
	OLED_Write_Cmd(0XAE);//OLED关闭
}

/*
函数功能：在对角(x1,y1)、(x2,y2)的区域内全亮或者全灭
	参数：对角坐标，mode:1亮，0灭
  返回值：无
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
函数功能：清屏
	参数：无
  返回值：无
*/
void OLED_Clear(void)
{
	u8 i, j;

	for (i = 0; i < 8; i++)
	{
		OLED_Write_Cmd(0xb0 + i);//设置页坐标
		OLED_Write_Cmd(0x00);//设置显示位置的列低地址
		OLED_Write_Cmd(0x10);//设置显示位置的列高地址
		for (j = 0; j < 128; j++)
		{
			OLED_Write_Data(0x00);
		}
	}
}

/*
函数功能：在x行y列显示字符
	参数：x列y行，ch显示的字符，size字符的大小
  返回值：无
*/
void OLED_Show_Char(u8 x, u8 y, u8 ch, u8 size)
{
	u8 temp,i;
	
	ch = ch - 32;	//得到偏移后的值
	
	if (size == 12)
	{
		for (i = 0; i < 12; i++)
		{
			temp = asc2_1206[ch][i];//调用1206字体字库
			OLED_SetPos(x+(i/2),y+8*(i%2));
			OLED_Write_Data(temp);
		}
	}
	else if (size == 16)
	{
		for (i = 0; i < 16; i++)
		{
			temp = asc2_1608[ch][i];//调用1608字体字库
			OLED_SetPos(x+(i/2),y+8*(i%2));
			OLED_Write_Data(temp);
		}
	}
	else if (size == 24)
	{
		for (i = 0; i< 36; i++)
		{
			temp = asc2_2412[ch][i];//调用2412字体字库
			OLED_SetPos(x+(i/3),y+8*(i%3));
			OLED_Write_Data(temp);
		}
	}
	else 
		return;
}


/*
函数功能：计算m^n
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
函数功能：显示无符号int型数字
	参数：xy坐标，num显示数字，size数字大小模式。显示的数字为0~4294967295
  返回值：无
*/
void OLED_Show_UnIntNum(u8 x, u8 y, u32 num, u8 figure, u8 size)
{
	u8 i = 0, len = 1, temp, length;
	u32 num1;
	int k;
	
	num1 = num;
	while(num1 > 9)	//求num的位数
	{
		len++;
		num1 /= 10;
	}
	length = len;
	while (len)
	{
		temp = num / Getpow(10, len - 1);
		
		OLED_Show_Char(x+(size/2)*i, y, temp + '0', size);//循环显示每位数字字符，从高位显示
		num = num % Getpow(10, len - 1);
		i++;
		len--;
	}
	//没有数字的位用空格填(避免出现先显示大的数字后显示比较小的数字就不能去除后面的数字)
	i=0;
	k = figure - length;
	while(k>0)
	{
		OLED_Show_Char((x+(size/2)*length)+(size/2)*i, y, ' ', size);//循环显示空格，从高位显示
		i++;
		k--;
	}
}

/*显示int型数据,含负数*/
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
函数功能：显示无符号float数字
	参数：xy坐标，num显示数字，precise显示数字的精度，size数字大小模式
  返回值：无
*/
void OLED_Show_UnFloatNum(u8 x, u8 y, double num, int precise, u8 size)
{
	u8 i = 0, len = 1;
	double num1;
	u32 integer;
	double decimal;
	
	integer = (int)num;//整数部分
	decimal = (double)(num - integer);//小数部分
	
	num1 = num;
	while(num1 > 9)	//求num整数的位数
	{
		len++;
		num1 /= 10;
	}
	OLED_Show_IntNum(x,y,integer,len,size);//显示整数部分
	OLED_Show_Char(x+(size/2)*len,y,'.',size);//显示小数点
	
	x = x+(size/2)*(len+1);
	while (precise)//显示几位小数
	{
		decimal = decimal * 10;
		integer = (int)decimal;//取出一位小数
		decimal = (double)(decimal - integer);
		OLED_Show_Char(x+(size/2)*i, y, integer + '0', size);//循环显示每位数字字符，从高位显示
		i++;
		precise--;
	}
}

/*显示float型数据,含负数*/
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
函数功能：显示字符串
	参数：x列y行，p需要显示的字符串指针，TextSize字模的大小
  返回值：无
*/
void OLED_Show_Str(u8 x, u8 y, const u8 *p, u8 TextSize)
{
	while((*p <= '~')&&(*p >= ' '))		//判断是不是非法字符!
    {       
        if(x>(128-(TextSize/2)))//判断字符超出显示范围没
		{
			x=0;
			y+=TextSize;
		}
        if(y>(64-TextSize))//判断字符超出显示范围没
		{
			y=x=0;
			OLED_Clear();
		}
		OLED_Show_Char(x, y, *p, TextSize);//显示这个字符
        x += TextSize/2;
        p++;
    }  
}

/*
函数功能：显示大小为宽12，高12的汉字
	C参数：起始显示坐标，汉字字符串str
  返回值：无
*/
void OLED_Show_Font12X12(u8 x, u8 y, u8 str[])
{
	u8 i, k, j,temp;
	
	for (i = 0; str[i] != '\0'; i++)
	{
		for (k = 0; k < 25; k++)//在字库中寻找汉字
		{
			if ((str[i] == font12x12[k].Index[0]) && (str[++i] == font12x12[k].Index[1]))//字库里面有对应的汉字
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
函数功能：显示大小为宽12，高16的汉字
	C参数：起始显示坐标，汉字字符串str
  返回值：无
*/
void OLED_Show_Font12X16(u8 x, u8 y, u8 *str)
{
	u8 i, k, j, temp;
	
	for (i = 0; str[i] != '\0'; i++)
	{
		for (k = 0; k < 25; k++)//在字库中寻找汉字
		{
			if ((str[i] == font12x16[k].Index[0]) && (str[++i] == font12x16[k].Index[1]))//字库里面有对应的汉字
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
函数功能：显示大小为宽16，高16的汉字
	C参数：起始显示坐标，汉字字符串str
  返回值：无
*/
void OLED_Show_Font16X16(u8 x, u8 y, u8 *str)
{
	u8 i, k, j,temp;
	
	for (i = 0; str[i] != '\0'; i+2)
	{
		for (k = 0; k < 25; k++)//在字库中寻找汉字
		{
			if ((str[i] == font16x16[k].Index[0]) && (str[i+1] == font16x16[k].Index[1]))//字库里面有对应的汉字
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
函数功能：显示图片
	参数：坐标(x,y),图片标号picture
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





