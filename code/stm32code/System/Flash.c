#include "stm32f10x.h"                  // Device header
//  Attention: flash擦写次数为10万次，故不可在死循环中反复调用flash函数  //

/**
  * @brief   flash写入数据 
  * @param   add 32位flash地址
  * @param	 dat 16位数据
  * @retval  无
  */
void FLASH_W(u32 add,u8 dat1,u8 dat2,u8 dat3,u8 dat4)
{
	 FLASH_Unlock(); //解锁FLASH编程擦除控制器
     FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);//清除标志位
     FLASH_ErasePage(add);    //擦除指定地址页
     FLASH_ProgramHalfWord(add,dat1); //从指定页的addr地址开始写
	 FLASH_ProgramHalfWord(add+2,dat2);
	 FLASH_ProgramHalfWord(add+4,dat3);
	 FLASH_ProgramHalfWord(add+6,dat4);
     FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);//清除标志位
     FLASH_Lock();    //锁定FLASH编程擦除控制器
}


/**
  * @brief    FLASH读出数据
  * @param    add 32位读出FLASH地址
  * @retval   16位数据
  */
u16 FLASH_R(u32 add)
{
	u16 a;
    a = *(u16*)add;
	return a;
}

/**
  * @brief    擦除指定FLASH地址页内的内容
  * @param    add 32位FLASH地址
  * @retval   无
  */
void FLASH_Clear(u32 add)
{
	FLASH_Unlock(); //解锁FLASH编程擦除控制器
    FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);//清除标志位
	FLASH_ErasePage(add);    //擦除指定地址页
    FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);//清除标志位
    FLASH_Lock(); 
}
