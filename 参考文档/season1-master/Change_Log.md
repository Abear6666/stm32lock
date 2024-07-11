<!--
 * @Author: Drswith
 * @Date: 2021-04-04 12:58:46
 * @LastEditors: Drswith
 * @LastEditTime: 2021-05-01 21:45:20
 * @FilePath: \season1\Change_Log.md
-->

# 更新日志
### 日期：2021-05-01

- #### 【usart.h】文件
  
  - 添加函数声明
    ```C
    void DEBUG_LOG(char *fmt,...);
    ```
    
- #### 【oled.c】文件

  - 添加函数 ***DEBUG_LOG*** ，由于之前使用 ***UsartPrintf(USART_DEBUG,"xxxxxxxxxxxx")*** 来输出调试信息我觉得太长了，所以把这个函数精简了一下：在打印的开头会输出“ [LOG] /> ”，并且会在最后自动换行

    ```C
    void DEBUG_LOG(char *fmt,...)
    {
      unsigned char UsartPrintfBuf[296];
      va_list ap;
      unsigned char *pStr = UsartPrintfBuf;
      
      va_start(ap, fmt);
      vsnprintf((char *)UsartPrintfBuf, sizeof(UsartPrintfBuf), fmt, ap);
      va_end(ap);
      UsartPrintf(USART_DEBUG, "[LOG] /> ");
      while(*pStr != 0)
      {
        USART_SendData(USART_DEBUG, *pStr++);
        while(USART_GetFlagStatus(USART_DEBUG, USART_FLAG_TC) == RESET);
      }
      UsartPrintf(USART_DEBUG, "\r\n");
    }
    ```  

- #### 【onenect.c】文件
  
  - 由于连接mqtt服务器的鉴权信息都是写死在本文件的宏定义中的，这就导致了许多同学拿到代码后修改wifi连接服务器的时候以相同的鉴权信息连接服务器互相踢下线的情况发生，因此在鉴权信息的后面连接了MCU的唯一标识，参见OneNet_DevLink函数

    ```C
    ...
    char ProductID[64];
    char Auth_Info[64];
    char DeviceID[64];
    ...
    
    MCU_ID = *(vu32*)(0x1FFFF7E8);  //获取MCU的唯一标识
    sprintf(ProductID,"%s_%X",PROID,MCU_ID);
    sprintf(Auth_Info,"%s_%X",AUTH_INFO,MCU_ID);
    sprintf(DeviceID,"%s_%X",DEVID,MCU_ID);
    
    
    UsartPrintf(USART_DEBUG,"OneNet_DevLink\r\n"
                "PROID: %s,	AUIF: %s,	DEVID:%s	--- 连接中....\r\n"
                          ,ProductID, Auth_Info, DeviceID);
    
    if(MQTT_PacketConnect(ProductID, Auth_Info, DeviceID, 256, 0, MQTT_QOS_LEVEL0, NULL, NULL, 0, &mqttPacket) == 0)
    {
      ...
    }
    ```

- #### 【main.c】文件
  
  - 进一步优化串口输出的调试信息，将 ***UsartPrintf(USART_DEBUG,"xxxxxxxxxxxx")*** 函数替换为 ***DEBUG_LOG*** 函数


- #### 微信小程序 【src/pages/index.vue】文件与原生微信小程序【/pages/index.js】文件
  - 针对请求天气数据API的时候由于API地址或者key不正确导致的请求返回失败做了简单的错误处理，如果返回的code=401则直接return，不往下执行代码，同时使用try catch进行代码错误捕捉，以健壮代码
  - 针对进行真机测试的时候不清楚怎么打开调试窗查看调试log，对连接服务器进行了异步提示处理：页面载入onShow的时候会弹出一个提示显示“连接服务器中”这个过程会倒计时10秒，10秒后超时连接失败，10秒内连接成功会将弹出提示改为“连接成功”，接着延迟一秒后进行订阅主题，订阅成功将会弹出提示“订阅成功”

### 日期：2021-04-30

- #### 【oled.h】文件
  
  - 添加函数声明
    ```C
    void OLED_ClearAndShowString(u8 x,u8 y,u8 *chr,u8 size1);
    ```
  - 添加函数声明
    ```C
    void OLED_Refresh_Line(char* ctx);
    ```  
- #### 【oled.c】文件
  
  - 添加函数 ***OLED_ClearAndShowString*** ，与ShowString函数的作用基本一致，区别仅是在showstring之前会将目标行清空显示

    ```C
    void OLED_ClearAndShowString(u8 x,u8 y,u8 *chr,u8 size1)
    {
      OLED_ShowString(x,y,(u8*)"                ",size1);
      OLED_ShowString(x,y,chr,size1);
    }
    ```
  - 添加函数 ***OLED_Refresh_Line*** ，它的作用主要是模拟程序启动时硬件自检画面自动向上垂直滚动的GUI效果

    ```C
    void OLED_Refresh_Line(char* ctx)
    {
      static u8 line;
      int randomDelay = 0;
      static char Display_line_0[16];
      static char Display_line_1[16];
      static char Display_line_2[16];
      static char Display_line_3[16];
      if(line < 4)
      {
        switch(line % 4)
        {
          case 0  :
              strcpy(Display_line_0, ctx);
              break; 
          case 1  :
              strcpy(Display_line_1, ctx);
              break; 
          case 2  :
              strcpy(Display_line_2, ctx);
              break; 
          case 3  :
              strcpy(Display_line_3, ctx);
              break; 
          default : 
              break;
        }
        OLED_ClearAndShowString(0,line % 4 *16,(u8*)ctx,16);//8*16 ��ABC��	
        OLED_Refresh();
        
        randomDelay = rand() % 100; 
        delay_ms(randomDelay);
        
        OLED_ShowString(11*8,line % 4 *16,(u8*)"[OK]",16);
        OLED_Refresh();
        
        randomDelay = rand() % 100; 
        delay_ms(randomDelay);
      }
      else
      {
        strcpy(Display_line_0, Display_line_1);
        OLED_ShowString(0,0*16,(u8*)Display_line_0,16);//8*16 ��ABC��

        strcpy(Display_line_1, Display_line_2);
        OLED_ShowString(0,1*16,(u8*)Display_line_1,16);//8*16 ��ABC��

        strcpy(Display_line_2, Display_line_3);
        OLED_ShowString(0,2*16,(u8*)Display_line_2,16);//8*16 ��ABC��

        strcpy(Display_line_3, ctx);
        OLED_ClearAndShowString(0,3*16,(u8*)ctx,16);//8*16 ��ABC��	
        OLED_Refresh();
        
        randomDelay = rand() % 100; 
        delay_ms(randomDelay);
        
        OLED_ShowString(11*8,3 *16,(u8*)"[OK]",16);
        OLED_Refresh();
        
        randomDelay = rand() % 100 ; 
        delay_ms(randomDelay);
      }
      
      line++;
    }
    ```  

- #### 【main.c】文件
  
  - 进一步优化采集传感器数据时的串口输出效果，具体参见代码以及烧录后的串口输出
  - 使用OLED_Refresh_Line函数实现模拟硬件自检画面内容垂直向上滚动GUI效果，具体参见代码以及烧录后OLED的显示效果

### 日期：2021-04-09

- #### 原生开发微信小程序来了！！！
  **针对【vue安装不上导致模板初始化失败】、【node-sass安装失败】、【MQTTjs版本不对（要用3.0.0）】导致小程序连接不上mqtt服务器等等问题，我使用【微信开发者工具】用原生的开发方式【重写】了这个小程序** 
  - 使用微信开发者工具开发
  - 完全按照微信小程序官方文档开发
  - 已经在【utils】目录下集成了mqttjs3.0.0
  - 已经在【@vant】目录下集成了vant-weapp 1.6.8最新版UI组件，请参照最新的1.6.8文档使用组件 [Vant Weapp文档地址](https://vant-contrib.gitee.io/vant-weapp/#/intro)
  - 欢迎使用欢迎投币欢迎喂食

### 日期：2021-04-07

- #### 微信小程序 【src/pages/index.vue】文件
  - 文件开头的部分对代码进行了大量注解和优化
  - 针对和风天气v7版本的api进行了一系列修改
  - 和风天气免费版api已经无法请求生活指数，所以写死了，有能力的同学可以自己修改
  
### 日期：2021-04-04

- #### 基础部分

  - 添加原理图的立创 EDA JSON 文件
  - 原理图中 USB-TTL 的 CP2102 模块的 DTR 硬件流控引脚去掉
  - 硬件代码中的 keil 工程文件由【LED.uvprojx】改为【xiaobaiSTM32.uvprojx】

- #### 【esp8266.c】文件

  - 【esp8266.c】文件中原本关于 wifi 名称密码的地方进行了修改

    ```C
    #define ESP8266_WIFI_INFO		"AT+CWJAP=\"spx24\",\"52xiaobai999\"\r\n"

    #define ESP8266_ONENET_INFO		"AT+CIPSTART=\"TCP\",\"mqtt.mqttssledu.xyz\",1883\r\n"
    ```

    分拆出了原有宏定义中的 wifi 名称、密码、服务器地址、端口，改为以下代码

    ```C
    #define WIFI_SSID 						"spx24"								//	WIFI的名称 必须用2.4G的wifi不能用5G的，且不能用中文、空格
    #define WIFI_PSWD 						"52xiaobai999"				        //	WIFI密码

    #define SERVER_HOST						"broker.emqx.io"	                //	MQTT服务器域名或IP
    #define SERVER_PORT						1883							    //	MQTT服务器端口（一般为1883不用改）


    #define ESP8266_WIFI_INFO			"AT+CWJAP=\"" WIFI_SSID "\",\"" WIFI_PSWD "\"\r\n"
    #define ESP8266_ONENET_INFO		    "AT+CIPSTART=\"TCP\",\"" SERVER_HOST "\"," SERVER_PORT "\r\n"

    ```

  - 针对 esp8266 的初始化加入了一个标志位 **ESP8266_INIT_OK**
  - 针对【esp8266.c】文件中 8266 初始化过程中 **ESP8266_Init** 函数(void)PC14 的使能进行了删除（没用到这个引脚复位 8266）
  - 针对【esp8266.c】文件中 8266 初始化 **ESP8266_Init** 函数的串口输出加了一些中文注释

- #### 【onenet.c】文件

  - 针对【onenet.c】文件中 **OneNet_DevLink** 函数的连接状态打印进行了一点修改

    ```C
      UsartPrintf(USART_DEBUG, "OneNet_DevLink\r\n"
                  "PROID: %s,	AUIF: %s,	DEVID:%s\r\n"
                        , PROID, AUTH_INFO, DEVID);
    ```

    改为，加入 连接中 三个字

    ```C
      UsartPrintf(USART_DEBUG, "OneNet_DevLink\r\n"
                  "PROID: %s,	AUIF: %s,	DEVID:%s	--- 连接中....\r\n"
                        , PROID, AUTH_INFO, DEVID);
    ```

- #### 【main.c】文件

  - 在【main.c】文件中，连接 wifi 和连接连接 mqtt 过程中的 OLED 显示内容进行了修改，连接 wifi 的时候会显示 Wating For Wifi Connection，连接 mqtt 服务器的时候会显示 Wating For Server Connection

  - 在【main.c】文件中，将 tim2 和 tim3 的初始化由连接 wifi 之前放在了连接服务器之后
  - 在【main.c】文件中，针对采集数据和发布数据的串口输出进行了优化，加入一些分隔符和空行，使得更容易阅读
