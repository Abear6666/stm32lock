#include "stm32f10x.h"                  // Device header
//  Attention: flash��д����Ϊ10��Σ��ʲ�������ѭ���з�������flash����  //

/**
  * @brief   flashд������ 
  * @param   add 32λflash��ַ
  * @param	 dat 16λ����
  * @retval  ��
  */
void FLASH_W(u32 add,u8 dat1,u8 dat2,u8 dat3,u8 dat4)
{
	 FLASH_Unlock(); //����FLASH��̲���������
     FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);//�����־λ
     FLASH_ErasePage(add);    //����ָ����ַҳ
     FLASH_ProgramHalfWord(add,dat1); //��ָ��ҳ��addr��ַ��ʼд
	 FLASH_ProgramHalfWord(add+2,dat2);
	 FLASH_ProgramHalfWord(add+4,dat3);
	 FLASH_ProgramHalfWord(add+6,dat4);
     FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);//�����־λ
     FLASH_Lock();    //����FLASH��̲���������
}


/**
  * @brief    FLASH��������
  * @param    add 32λ����FLASH��ַ
  * @retval   16λ����
  */
u16 FLASH_R(u32 add)
{
	u16 a;
    a = *(u16*)add;
	return a;
}

/**
  * @brief    ����ָ��FLASH��ַҳ�ڵ�����
  * @param    add 32λFLASH��ַ
  * @retval   ��
  */
void FLASH_Clear(u32 add)
{
	FLASH_Unlock(); //����FLASH��̲���������
    FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);//�����־λ
	FLASH_ErasePage(add);    //����ָ����ַҳ
    FLASH_ClearFlag(FLASH_FLAG_BSY|FLASH_FLAG_EOP|FLASH_FLAG_PGERR|FLASH_FLAG_WRPRTERR);//�����־λ
    FLASH_Lock(); 
}
