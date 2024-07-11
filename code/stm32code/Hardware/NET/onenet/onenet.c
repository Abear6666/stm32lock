/**
	************************************************************
	************************************************************
	************************************************************
	*	�ļ����� 	onenet.c
	*
	*	���ߣ� 		�ż���
	*
	*	���ڣ� 		2017-05-08
	*
	*	�汾�� 		V1.1
	*
	*	˵���� 		��onenetƽ̨�����ݽ����ӿڲ�
	*
	*	�޸ļ�¼��	V1.0��Э���װ�������ж϶���ͬһ���ļ������Ҳ�ͬЭ��ӿڲ�ͬ��
	*				V1.1���ṩͳһ�ӿڹ�Ӧ�ò�ʹ�ã����ݲ�ͬЭ���ļ�����װЭ����ص����ݡ�
	************************************************************
	************************************************************
	************************************************************
**/

//��Ƭ��ͷ�ļ�
#include "stm32f10x.h"

//�����豸
#include "esp8266.h"

//Э���ļ�
#include "onenet.h"
#include "mqttkit.h"

//Ӳ������
#include "usart.h"
#include "delay.h"

//C��
#include <string.h>
#include <stdio.h>
#include "cJSON.h"

#include "led.h"
#include "BEEP.h"
#include "servo.h"
#include "OLED.H"
#include "as608_use.h"
#include "rc522.h"
#include "servo.h"

#define PROID		"mysmarthome"

#define AUTH_INFO	"huaqing_smarthome_zui_jiandan"

#define DEVID		"mysmarthome"


extern unsigned char esp8266_buf[128];

extern void unlock_interface(void);  //��������
extern void main_interface(void);  //����������
extern void main1_interface(void);  //����������
extern u8 lock;
//extern u8 alarmFlag;//�Ƿ񱨾��ı�־
extern u8 Beep_Status; //������������־λ
//extern u8 alarm_is_free;  {"target":"LED","value":0}

u8 alarmFlag;//�Ƿ񱨾��ı�־
u8 alarm_is_free;
extern u8 Power_Status;
extern u8 upload_card_number_flag; //���Ϳ��Ÿ�С�����־
extern u8 upload_fin_number_flag;//����ָ�Ƹ�����С�����־
extern u16 ValidN;										/*ģ������Чָ�Ƹ���*/

//==========================================================
//	�������ƣ�	OneNet_DevLink
//
//	�������ܣ�	��onenet��������
//
//	��ڲ�����	��
//
//	���ز�����	1-�ɹ�	0-ʧ��
//
//	˵����		��onenetƽ̨��������
//==========================================================
_Bool OneNet_DevLink(void)
{
	char ProductID[64];
	char Auth_Info[64];
	char DeviceID[64];
	
	u32 MCU_ID;
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};					//Э���

	unsigned char *dataPtr;
	
	_Bool status = 1;
	
	MCU_ID = *(vu32*)(0x1FFFF7E8);	//��ȡMCU��Ψһ��ʶ
	sprintf(ProductID,"%s_%X",PROID,MCU_ID);
	sprintf(Auth_Info,"%s_%X",AUTH_INFO,MCU_ID);
	sprintf(DeviceID,"%s_%X",DEVID,MCU_ID);
	
	
	UsartPrintf(USART_DEBUG,"OneNet_DevLink\r\n"
							"PROID: %s,	AUIF: %s,	DEVID:%s	--- ������....\r\n"
                        ,ProductID, Auth_Info, DeviceID);
	
	if(MQTT_PacketConnect(ProductID, Auth_Info, DeviceID, 256, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);			//�ϴ�ƽ̨
		
		dataPtr = ESP8266_GetIPD(250);									//�ȴ�ƽ̨��Ӧ
		if(dataPtr != NULL)
		{
			if(MQTT_UnPacketRecv(dataPtr) == MQTT_PKT_CONNACK)
			{
				switch(MQTT_UnPacketConnectAck(dataPtr))
				{
					case 0:DEBUG_LOG("Tips:	���ӳɹ�\r\n");status = 0;break;
					
					case 1:DEBUG_LOG("WARN:	����ʧ�ܣ�Э�����\r\n");break;
					case 2:DEBUG_LOG("WARN:	����ʧ�ܣ��Ƿ���clientid\r\n");break;
					case 3:DEBUG_LOG("WARN:	����ʧ�ܣ�������ʧ��\r\n");break;
					case 4:DEBUG_LOG("WARN:	����ʧ�ܣ��û������������\r\n");break;
					case 5:DEBUG_LOG("WARN:	����ʧ�ܣ��Ƿ�����(����token�Ƿ�)\r\n");break;
					
					default:DEBUG_LOG("ERR:	����ʧ�ܣ�δ֪����\r\n");break;
				}
			}
		}
		
		MQTT_DeleteBuffer(&mqttPacket);								//ɾ��
	}
	else
		DEBUG_LOG("WARN:	MQTT_PacketConnect Failed\r\n");
	
	return status;
	
}

//==========================================================
//	�������ƣ�	OneNet_Subscribe
//
//	�������ܣ�	����
//
//	��ڲ�����	topics�����ĵ�topic
//				topic_cnt��topic����
//
//	���ز�����	SEND_TYPE_OK-�ɹ�	SEND_TYPE_SUBSCRIBE-��Ҫ�ط�
//
//	˵����		
//==========================================================
void OneNet_Subscribe(const char *topics[], unsigned char topic_cnt)
{
	
	unsigned char i = 0;
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//Э���
	
	for(; i < topic_cnt; i++)
		DEBUG_LOG("Subscribe Topic: %s\r\n", topics[i]);
	
	if(MQTT_PacketSubscribe(MQTT_SUBSCRIBE_ID, MQTT_QOS_LEVEL0, topics, topic_cnt, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);					//��ƽ̨���Ͷ�������
		
		MQTT_DeleteBuffer(&mqttPacket);											//ɾ��
	}

}

//==========================================================
//	�������ƣ�	OneNet_Publish
//
//	�������ܣ�	������Ϣ
//
//	��ڲ�����	topic������������
//				msg����Ϣ����
//
//	���ز�����	SEND_TYPE_OK-�ɹ�	SEND_TYPE_PUBLISH-��Ҫ����
//
//	˵����		
//==========================================================
void OneNet_Publish(const char *topic, const char *msg)
{

	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};							//Э���
	
	DEBUG_LOG("Publish Topic: %s, Msg: %s\r\n", topic, msg);
	
	if(MQTT_PacketPublish(MQTT_PUBLISH_ID, topic, msg, strlen(msg), MQTT_QOS_LEVEL0, 0, 1, &mqttPacket) == 0)
	{
		ESP8266_SendData(mqttPacket._data, mqttPacket._len);					//��ƽ̨���Ͷ�������
		
		MQTT_DeleteBuffer(&mqttPacket);											//ɾ��
	}

}

//==========================================================
//	�������ƣ�	OneNet_RevPro
//
//	�������ܣ�	ƽ̨�������ݼ��
//
//	��ڲ�����	dataPtr��ƽ̨���ص����� Ҳ����*cmd
//
//	���ز�����	��
//
//	˵����		
//==========================================================
void OneNet_RevPro(unsigned char *cmd)
{
	
	MQTT_PACKET_STRUCTURE mqttPacket = {NULL, 0, 0, 0};								//Э���
	
	char *req_payload = NULL;
	char *cmdid_topic = NULL;
	
	unsigned short topic_len = 0;
	unsigned short req_len = 0;
	
	unsigned char type = 0;
	unsigned char qos = 0;
	static unsigned short pkt_id = 0;
	
	short result = 0;

	char *dataPtr = NULL;
	char numBuf[10];
	int num = 0;
	
	cJSON *json , *json_value;
	
	type = MQTT_UnPacketRecv(cmd);
	switch(type)
	{
		case MQTT_PKT_CMD:															//�����·�
			
			result = MQTT_UnPacketCmd(cmd, &cmdid_topic, &req_payload, &req_len);	//���topic����Ϣ��
			if(result == 0)
			{
				DEBUG_LOG("cmdid: %s, req: %s, req_len: %d\r\n", cmdid_topic, req_payload, req_len);

				MQTT_DeleteBuffer(&mqttPacket);									//ɾ��
			}
		break;
			
		case MQTT_PKT_PUBLISH:														//���յ�Publish��Ϣ
		
			result = MQTT_UnPacketPublish(cmd, &cmdid_topic, &topic_len, &req_payload, &req_len, &qos, &pkt_id);
			if(result == 0)
			{
				DEBUG_LOG("topic: %s, topic_len: %d, payload: %s, payload_len: %d\r\n",
																	cmdid_topic, topic_len, req_payload, req_len);
				// �����ݰ�req_payload����JSON��ʽ����
				json = cJSON_Parse(req_payload);
				if (!json)UsartPrintf(USART_DEBUG,"Error before: [%s]\n",cJSON_GetErrorPtr());
				else
				{
					json_value = cJSON_GetObjectItem(json , "target");
					UsartPrintf(USART_DEBUG,"json_value: [%s]\n",json_value->string);
					UsartPrintf(USART_DEBUG,"json_value: [%s]\n",json_value->valuestring);
					if(strstr(json_value->valuestring,"LED") != NULL)
					{
						json_value = cJSON_GetObjectItem(json , "value");
						UsartPrintf(USART_DEBUG,"json_value: [%d]\n",json_value->valueint);
						if(json_value->valueint)LED1_ON();//����
						else LED1_OFF();//�ص�
					}
					else if(strstr(json_value->valuestring,"BEEP") != NULL)
					{
						json_value = cJSON_GetObjectItem(json , "value");
						if(json_value->valueint)Beep_Status = 1;//�򿪱����� BEEP_ON  alarmFlag = 1
						else Beep_Status = 0;//�رձ����� alarmFlag = 0
						//alarm_is_free=0;//��λ�����Ƹ��ֶ����ƾ���һ�������ȼ� �������Զ����ƣ����Ҫ��alarm_is_free ��0
					}
					else if(strstr(json_value->valuestring,"Door") != NULL)
					{
						json_value = cJSON_GetObjectItem(json , "value");
						if(json_value->valueint)
						{
							unlock_interface();
						
						}//�򿪱����� BEEP_ON  alarmFlag = 1
						else{
							lock = 0;
							close_door();
							OLED_Clear();
						
						} 
						//alarm_is_free=0;//��λ�����Ƹ��ֶ����ƾ���һ�������ȼ� �������Զ����ƣ����Ҫ��alarm_is_free ��0
					}
					else if(strstr(json_value->valuestring,"Addfin") != NULL) //����ָ��
					{
						json_value = cJSON_GetObjectItem(json , "value");
						if(json_value->valueint)
						{
									DEBUG_LOG("fingerprints_num:%d",ValidN);
						Add_FR(ValidN++);
						DEBUG_LOG("fingerprints_num:%d",ValidN);
							OLED_Clear();
							if(lock)
							{
								main_interface();	
							}				
						
						}

					}					
					else if(strstr(json_value->valuestring,"Rmfin") != NULL) //����ָ��
					{
						json_value = cJSON_GetObjectItem(json,"value");
						if(json_value->valueint)
						{
							DEBUG_LOG("fingerprints_num:%d",ValidN);
							Del_FR(--ValidN);
							DEBUG_LOG("fingerprints_num:%d",ValidN);
							OLED_Clear();
							if(lock)
							{
								main_interface();	
							}
												
							
						
						}

					}	
					else if(strstr(json_value->valuestring,"Addcard") != NULL) //���ӿ�Ƭ
					{
						json_value = cJSON_GetObjectItem(json , "value");
						if(json_value->valueint)
						{
							RC522_Add_card();
							OLED_Clear();
							if(lock)
							{
								main1_interface();	
							}							
						
						}

					}	
					else if(strstr(json_value->valuestring,"Rmcard") != NULL) //ɾ����Ƭ
					{
						json_value = cJSON_GetObjectItem(json , "value");
						if(json_value->valueint)
						{
							RC522_Rm_card();
							OLED_Clear();
							if(lock)
							{
								main1_interface();	
							}								
						
						}

					}					
					else if(strstr(json_value->valuestring,"Power") != NULL) //�رտ����Ž���Դ
					{
						json_value = cJSON_GetObjectItem(json,"value");
					
					
						if(json_value->valueint) // ������Դ
						{
							GPIO_SetBits(GPIOA, GPIO_Pin_8);
								DEBUG_LOG("������Դ");
							Power_Status = 1;
						
						}else{
						
							GPIO_ResetBits(GPIOA, GPIO_Pin_8);
								DEBUG_LOG("�رյ�Դ");
							Power_Status = 0;
						
						}

					}
					else if(strstr(json_value->valuestring,"Get_card") != NULL) //���Ϳ��ŵ�С����
					{
						json_value = cJSON_GetObjectItem(json,"value");
					
					
						if(json_value->valueint) // ���÷��ͺ���
						{
							DEBUG_LOG("���Ϳ���");
							upload_card_number_flag = 1;

						}


					}	
					else if(strstr(json_value->valuestring,"Get_fin") != NULL) //����ָ�Ƹ�����С����
					{
						json_value = cJSON_GetObjectItem(json,"value");
					
					
						if(json_value->valueint) // ���÷��ͺ���
						{
							DEBUG_LOG("����ָ�Ƹ���");
							upload_fin_number_flag = 1;

						}


					}					
				}

				cJSON_Delete(json);

			}

		break;
			
		case MQTT_PKT_PUBACK:														//����Publish��Ϣ��ƽ̨�ظ���Ack
		
			if(MQTT_UnPacketPublishAck(cmd) == 0)
				DEBUG_LOG("Tips:	MQTT Publish Send OK\r\n");
			
		break;
			
		case MQTT_PKT_PUBREC:														//����Publish��Ϣ��ƽ̨�ظ���Rec���豸��ظ�Rel��Ϣ
		
			if(MQTT_UnPacketPublishRec(cmd) == 0)
			{
				DEBUG_LOG("Tips:	Rev PublishRec\r\n");
				if(MQTT_PacketPublishRel(MQTT_PUBLISH_ID, &mqttPacket) == 0)
				{
					DEBUG_LOG("Tips:	Send PublishRel\r\n");
					ESP8266_SendData(mqttPacket._data, mqttPacket._len);
					MQTT_DeleteBuffer(&mqttPacket);
				}
			}
		
		break;
			
		case MQTT_PKT_PUBREL:														//�յ�Publish��Ϣ���豸�ظ�Rec��ƽ̨�ظ���Rel���豸���ٻظ�Comp
			
			if(MQTT_UnPacketPublishRel(cmd, pkt_id) == 0)
			{
				DEBUG_LOG("Tips:	Rev PublishRel\r\n");
				if(MQTT_PacketPublishComp(MQTT_PUBLISH_ID, &mqttPacket) == 0)
				{
					DEBUG_LOG("Tips:	Send PublishComp\r\n");
					ESP8266_SendData(mqttPacket._data, mqttPacket._len);
					MQTT_DeleteBuffer(&mqttPacket);
				}
			}
		
		break;
		
		case MQTT_PKT_PUBCOMP:														//����Publish��Ϣ��ƽ̨����Rec���豸�ظ�Rel��ƽ̨�ٷ��ص�Comp
		
			if(MQTT_UnPacketPublishComp(cmd) == 0)
			{
				DEBUG_LOG("Tips:	Rev PublishComp\r\n");
			}
		
		break;
			
		case MQTT_PKT_SUBACK:														//����Subscribe��Ϣ��Ack
		
			if(MQTT_UnPacketSubscribe(cmd) == 0)
				DEBUG_LOG("Tips:	MQTT Subscribe OK\r\n");
			else
				DEBUG_LOG("Tips:	MQTT Subscribe Err\r\n");
		
		break;
			
		case MQTT_PKT_UNSUBACK:														//����UnSubscribe��Ϣ��Ack
		
			if(MQTT_UnPacketUnSubscribe(cmd) == 0)
				DEBUG_LOG("Tips:	MQTT UnSubscribe OK\r\n");
			else
				DEBUG_LOG("Tips:	MQTT UnSubscribe Err\r\n");
		
		break;
		
		default:
			result = -1;
		break;
	}
	
	ESP8266_Clear();									//��ջ���
	
	if(result == -1)
		return;
	
	dataPtr = strchr(req_payload, '}');					//����'}'

	if(dataPtr != NULL && result != -1)					//����ҵ���
	{
		dataPtr++;
		
		while(*dataPtr >= '0' && *dataPtr <= '9')		//�ж��Ƿ����·��������������
		{
			numBuf[num++] = *dataPtr++;
		}
		
		num = atoi((const char *)numBuf);				//תΪ��ֵ��ʽ
		
	}

	if(type == MQTT_PKT_CMD || type == MQTT_PKT_PUBLISH)
	{
		MQTT_FreeBuffer(cmdid_topic);
		MQTT_FreeBuffer(req_payload);
	}

}
