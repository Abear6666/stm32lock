[toc]



#  整体结构

![image](https://raw.githubusercontent.com/Abear6666/imge/main/images/s1_1.png)

## 硬件清单

### 核心板

#### STM32F103C8T6

### WIFI模块

#### ESP8266-01s

### 传感器

#### DHT11温湿度传感器

#### BH1750光照传感器

### 其他

#### LED

#### OLED

#### 4脚按键开关

#### CP2102（USB转TTL模块）

#### 有源蜂鸣器（低电平触发）

#### 金属膜电阻器

#### 面包板（或PCB）

#### 电源和导线

## 采购记录



| 项目              | 数量 | 已购 | 价格 |
| ----------------- | ---- | ---- | ---- |
| STM32F103C8T6     | 1    |      |      |
| ESP8266-01s       | 1    |      |      |
| DHT11温湿度传感器 | 1    |      |      |
| BH1750光照传感器  | 1    |      |      |
| LED               | 1    |      |      |
| OLED              | 1    |      |      |
| 4脚按键开关       | 2    |      |      |
| CP2102            | 1    |      |      |
| 有源蜂鸣器        | 1    |      |      |
| 金属膜电阻器      | 1    |      |      |
| 面包板/PCB        | 1    |      |      |
| 电源和导线        | 1    |      |      |
| **合计**          |      |      |      |

## 实物制作：

使用嘉立创绘制的原理图：

![image-20221208101255057](https://raw.githubusercontent.com/Abear6666/imge/main/images/image-20221208101255057.png)



![image-20221208101733687](https://raw.githubusercontent.com/Abear6666/imge/main/images/image-20221208101733687.png)

这个pcb布局有点随意 有几个IO口对应不对  第一次绘制没注意太多细节...仅供参考 后边会优化好 



实物效果：

![image-20221208103150723](https://raw.githubusercontent.com/Abear6666/imge/main/images/image-20221208103150723.png)



WiFi模块引脚弄错就接成杜邦线。。 将就看看吧哈哈

小程序：![image-20221208110549601](https://raw.githubusercontent.com/Abear6666/imge/main/images/image-20221208110549601.png)





# stm32开发：

# 

### DHT11温湿度传感器

- 主要头文件：`dht11.h`

- 主要源文件：`dht11.c`

- 主要函数：

  ```
  void DH11_Init(void);
  u8 DHT11_Read_Data(u8 *humiH, u8 *humiL, u8 *tempH, u8 *tempL);
  ```

### BH1750光照传感器

- 主要头文件：`bh1750.h`

- 主要源文件：`bh1750.c`

- 主要函数：

  ```
  void BH1750_Init(void);
  float Light_Int
  ```

###  蜂鸣器报警的手动控制与手动/自动控制权的协调

**采用了外部中断的方法实现**

- 主要头文件：`key.h`

- 主要源文件：`key.c`

- 主要函数：

  ```c
  void NVIC_PriorityGroupConfig(u8 NVIC_PriorityGroup_x); // 设置中断优先级分组
  void EXITX_Init(); // 外部中断初始化
  void TIMx_IRQHandler(void); // TIMx中断处理
  void KEY_Init(); // 按键
  ```



### ESP8266-WIFI模块：

- 主要头文件：`esp8266.h`

- 主要源文件：`esp8266.c`

- 主要函数：

  ```c
  void ESP8266_Init(void); // 初始化
  void ESP8266_SendData(unsigned char *data, unsigned short len);//发送数据
  unsigned char *ESP8266_GetIPD(unsigned short timeOut);//获取平台返回的数据
  void USART2_IRQHandler(void); // 串口2收发中断
  ```



### mqtt协议接入（onenet）：

- 主要头文件：`onenet.h`
- 主要源文件：`onenet.c`
- 主要函数：

```c
_Bool OneNet_DevLink(void)//与onenet创建连接
void OneNet_Subscribe(const char *topics[], unsigned char topic_cnt);//订阅
void OneNet_Publish(const char *topic, const char *msg);//发布
void OneNet_RevPro(unsigned char *cmd);//平台返回数据检测
```



### 主函数：

```c
 int main(void)
 {	
	unsigned short timeCount = 0;	//发送间隔变量
	unsigned char *dataPtr = NULL;
	static u8 lineNow;
	Usart1_Init(115200);//debug串口
		DEBUG_LOG("\r\n");
		DEBUG_LOG("UART1初始化			[OK]");
	delay_init();	    	 //延时函数初始化	
		DEBUG_LOG("延时函数初始化			[OK]");
	OLED_Init();
	OLED_ColorTurn(0);//0正常显示，1 反色显示
  OLED_DisplayTurn(0);//0正常显示 1 屏幕翻转显示
	OLED_Clear();
	 	DEBUG_LOG("OLED1初始化			[OK]");
		OLED_Refresh_Line("OLED");

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);// 设置中断优先级分组2
		DEBUG_LOG("中断优先初始化			[OK]");
		OLED_Refresh_Line("NVIC");
	LED_Init();		  	//初始化与LED连接的硬件接口
		DEBUG_LOG("LED初始化			[OK]");
		OLED_Refresh_Line("Led");
	KEY_Init();          	//初始化与按键连接的硬件接口
		DEBUG_LOG("按键初始化			[OK]");
		OLED_Refresh_Line("Key");
	EXTIX_Init();		//外部中断初始化
		DEBUG_LOG("外部中断初始化			[OK]");
		OLED_Refresh_Line("EXIT");
	BEEP_Init();
		DEBUG_LOG("蜂鸣器初始化			[OK]");
		OLED_Refresh_Line("Beep");
	DHT11_Init();
		DEBUG_LOG("DHT11初始化			[OK]");
		OLED_Refresh_Line("DHT11");
  BH1750_Init();
		DEBUG_LOG("BH1750初始化			[OK]");
		OLED_Refresh_Line("BH1750");
	Usart2_Init(115200);//stm32-8266通讯串口
		DEBUG_LOG("UART2初始化			[OK]");
		OLED_Refresh_Line("Uart2");
	
		DEBUG_LOG("硬件初始化			[OK]");
		
	DEBUG_LOG("初始化ESP8266 WIFI模块...");
	if(!ESP8266_INIT_OK){
		OLED_Clear();
		sprintf(oledBuf,"Waiting For");
		OLED_ShowString(16,0,(u8*)oledBuf,16);//8*16 “ABC”
		sprintf(oledBuf,"WiFi");
		OLED_ShowString(48,18,(u8*)oledBuf,16);//8*16 “ABC”
		sprintf(oledBuf,"Connection");
		OLED_ShowString(24,36,(u8*)oledBuf,16);//8*16 “ABC”
		OLED_Refresh();
	}
	ESP8266_Init();					//初始化ESP8266
	OLED_Clear();
	sprintf(oledBuf,"Waiting For");
	OLED_ShowString(16,0,(u8*)oledBuf,16);//8*16 “ABC”
	sprintf(oledBuf,"MQTT Server");
	OLED_ShowString(16,18,(u8*)oledBuf,16);//8*16 “ABC”
	sprintf(oledBuf,"Connection");
	OLED_ShowString(24,36,(u8*)oledBuf,16);//8*16 “ABC”
	OLED_Refresh();	
	while(OneNet_DevLink()){//接入OneNET
		delay_ms(500);
	}		
	
	OLED_Clear();	
	
	TIM2_Int_Init(4999,7199);
	TIM3_Int_Init(2499,7199);
	
	BEEP = 0;//鸣叫提示接入成功
	delay_ms(250);
	BEEP = 1;
	
	OneNet_Subscribe(devSubTopic, 1);
	
	while(1)
	{
		if(timeCount % 40 == 0)//1000ms / 25 = 40 一秒执行一次
		{
			/********** 温湿度传感器获取数据**************/
			DHT11_Read_Data(&humidityH,&humidityL,&temperatureH,&temperatureL);
			
			/********** 光照度传感器获取数据**************/
			if (!i2c_CheckDevice(BH1750_Addr))
			{
				Light = LIght_Intensity();
			}
			
			/********** 读取LED0的状态 **************/
			Led_Status = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4);
			
			/********** 报警逻辑 **************/
			if(alarm_is_free == 10)//报警器控制权是否空闲 alarm_is_free == 10 初始值为10
			{
				if((humidityH < 80) && (temperatureH < 30) && (Light < 1000))alarmFlag = 0;
				else alarmFlag = 1;
			}
			if(alarm_is_free < 10)alarm_is_free++;
//			DEBUG_LOG("alarm_is_free = %d", alarm_is_free);
//			DEBUG_LOG("alarmFlag = %d\r\n", alarmFlag);

			
			/********** 输出调试信息 **************/
			DEBUG_LOG(" | 湿度：%d.%d C | 温度：%d.%d %% | 光照度：%.1f lx | 指示灯：%s | 报警器：%s | ",humidityH,humidityL,temperatureH,temperatureL,Light,Led_Status?"关闭":"【启动】",alarmFlag?"【启动】":"停止");
		}
		if(++timeCount >= 150)	// 5000ms / 25 = 200 发送间隔5000ms
		{
			Led_Status = GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4);//读取LED0的状态
			DEBUG_LOG("==================================================================================");
			DEBUG_LOG("发布数据 ----- OneNet_Publish");
			sprintf(PUB_BUF,"{\"Hum\":%d.%d,\"Temp\":%d.%d,\"Light\":%.1f,\"Led\":%d,\"Beep\":%d}",
				humidityH,humidityL,temperatureH,temperatureL,Light,Led_Status?0:1,alarmFlag);
			OneNet_Publish(devPubTopic, PUB_BUF);
			DEBUG_LOG("==================================================================================");
			timeCount = 0;
			ESP8266_Clear();
		}
		
		dataPtr = ESP8266_GetIPD(3);
		if(dataPtr != NULL)
			OneNet_RevPro(dataPtr);
		delay_ms(10);
	}
}
```



# 小程序开发：

## 1.工具下载：

1.[下载 Node.js (p2hp.com)](http://nodejs.p2hp.com/download/)

![image-20221201095700530](https://raw.githubusercontent.com/Abear6666/imge/main/images/image-20221201095700530.png)

2.[微信开发者工具下载地址与更新日志 | 微信开放文档 (qq.com)](https://developers.weixin.qq.com/miniprogram/dev/devtools/download.html)

![image-20221201095810050](https://raw.githubusercontent.com/Abear6666/imge/main/images/image-20221201095810050.png)



3.安装VUE 

```
npm install -g @vue/cli --registry https://registry.npm.taobao.org
```

![image-20221201102808134](https://raw.githubusercontent.com/Abear6666/imge/main/images/image-20221201102808134.png)





4.安装vscode插件：（为了代码提示和语法报错）

```
WXML - Language Service
VEtur
SCSS Formatter

```



咱用的小程序开发是基于vue框架：所以可以花10秒钟学了下

mpvue快速入门：

```
mpvue （github 地址请参见）是一个使用 Vue.js 开发小程序的前端框架。框架基于 Vue.js 核心，mpvue 修改了 Vue.js 的 runtime 和 compiler 实现，使其可以运行在小程序环境中，从而为小程序开发引入了整套 Vue.js 开发体验。

#实践案例
```

[快速上手 | mpvue.com](http://mpvue.com/mpvue/quickstart.html)



## 2.源码编写：

**源码创建：**

1.vscode打开个文件夹 然后新建终端输入

```bash
vue init mpvue/mpvue-quickstart my-project
```



出现问题：

![image-20221201105229784](https://raw.githubusercontent.com/Abear6666/imge/main/images/image-20221201105229784.png)



解决：

![image-20221201105251164](https://raw.githubusercontent.com/Abear6666/imge/main/images/image-20221201105251164.png)

填写信息：随便填

![image-20221201105602863](https://raw.githubusercontent.com/Abear6666/imge/main/images/image-20221201105602863.png)



2.为了实现vscode编写代码更新到小程序端

```
cd my-project
npm install
npm run dev //跑起代码监听 
```



使用wx助手使用测试号 不使用云开发 打开刚才的文件夹即可

![image-20221201111109194](https://raw.githubusercontent.com/Abear6666/imge/main/images/image-20221201111109194.png)





![image-20221201142013171](https://raw.githubusercontent.com/Abear6666/imge/main/images/image-20221201142013171.png)



3.没出错接着安装vue插件



```
npm install sass-loader@5.0.0 --save-dev
npm install node-sass@8.0.0 --save-dev
npm install mqtt@3.0.0 --save

基本后边跑监听代码功能 出错的话 解决方法：
基本问题是mqtt和sass版本不兼容 把原本的卸载重新安装即可

或者查看https://www.programminghunter.com/article/25802380535/


```



查看有无安装好：

![image-20221202111414562](https://raw.githubusercontent.com/Abear6666/imge/main/images/image-20221202111414562.png)

![image-20221202111355312](https://raw.githubusercontent.com/Abear6666/imge/main/images/image-20221202111355312.png)

![image-20221201142337530](https://raw.githubusercontent.com/Abear6666/imge/main/images/image-20221201142337530.png)



**4.源码修改：**



修改小程序最底下的图标流程：

[iconfont-阿里巴巴矢量图标库](https://www.iconfont.cn/) 

1.在src里边的app.json 修改图标和文字



2.改完代码提示错误：解决方法（就是上边的安装vue插件的问题）：

[Module build failed: Error: Cannot find module 'node-sass'报错问题_夕阳下美了剪影的博客-CSDN博客_module build failed](https://blog.csdn.net/qq_38401285/article/details/86483278?spm=1001.2101.3001.6650.1&utm_medium=distribute.pc_relevant.none-task-blog-2~default~CTRLIST~Rate-1-86483278-blog-113393561.pc_relevant_3mothn_strategy_and_data_recovery&depth_1-utm_source=distribute.pc_relevant.none-task-blog-2~default~CTRLIST~Rate-1-86483278-blog-113393561.pc_relevant_3mothn_strategy_and_data_recovery&utm_relevant_index=2)



3.修改源码 （最后能把主页替换我写的 保存没问题就代码编写的环境没问题了 ）

主界面的页面修改：

```
src/pages/index/index.vue
路径

```

```vue
<template>
  <div class="wrapper">
    <div class="header-wrapper">
      <div class="header-title">
        <span>空气质量-良好</span>
        <span>深圳市</span>
      </div>
      <div class="header-text">
        <span>55</span>
        <span>阴</span>
      </div>
      <div class="weather-advice">空气质量良好，出门记得带好口罩！</div>
    </div>

    <div class="body-wrapper">
      <div class="data-wrapper">
        <div class="data">
          <img class="data-logo" src="/static/images/wendu.png" />
          <div class="data-text">
            <div class="data-title">温度</div>
            <div class="data-value">{{ Temp }}℃</div>
          </div>
        </div>
        <div class="data">
          <img class="data-logo" src="/static/images/shidu.png" />
          <div class="data-text">
            <div class="data-title">湿度</div>
            <div class="data-value">{{ Hum }}%</div>
          </div>
        </div>
      </div>

      <div class="data-wrapper">
        <div class="data">
          <img class="data-logo" src="/static/images/guangzhao.png" />
          <div class="data-text">
            <div class="data-title">光照度</div>
            <div class="data-value">{{ Light }}Lx</div>
          </div>
        </div>
        <div class="data">
          <img class="data-logo" src="/static/images/led.png" />
          <div class="data-text">
            <div class="data-title">客厅灯</div>
            <div class="data-value">
              <switch @change="onLedChange" :checked="Led" color="#3d7ef9" />
            </div>
          </div>
        </div>
      </div>

      <div class="data-wrapper">
        <div class="data">
          <img class="data-logo" src="/static/images/baojin.png" />
          <div class="data-text">
            <div class="data-title">报警器</div>
            <switch @change="onBeepChange" :checked="Beep" color="#3d7ef9" />
          </div>
        </div>
      </div>
    </div>
    <div class="body"></div>
  </div>
</template>

<script>
import { connect } from "mqtt/dist/mqtt.js";


const mqttUrl = "wxs://mqtt.bear321.cn:8084/mqtt";

export default {
  data() {
    return {
      client: {},
      Temp: 0,
      Hum: 0,
      Light: 0,
      Led: false,
      Beep: false,
    };
  },

  components: {},

  methods: {

    onLedChange(event){
      var that = this
      console.log(event.mp.detail)
      let sw = event.mp.detail.value
      that.Led = sw
      if(sw){
        that.client.publish('/mysmarthome/sub','{"target":"LED","value":1}',function(err){
          if(!err)
          {
            console.log("成功下发命令---开灯")
          }
        })
      }else{
        that.client.publish('/mysmarthome/sub','{"target":"LED","value":0}',function(err){
          if(!err)
          {
            console.log("成功下发命令---关灯")
          }
        })       
      }

    },

    onBeepChange(event){
      var that = this
      console.log(event.mp.detail)
      let sw = event.mp.detail.value
      that.Beep = sw
      if(sw){
        that.client.publish('/mysmarthome/sub','{"target":"BEEP","value":1}',function(err){
          if(!err)
          {
            console.log("成功下发命令---打开警报器")
          }
        })
      }else{
        that.client.publish('/mysmarthome/sub','{"target":"BEEP","value":0}',function(err){
          if(!err)
          {
            console.log("成功下发命令---关闭警报器")
          }
        })       
      }

    }
  },

  // created () {
  //   // let app = getApp()
  // }
  onShow() {
    var that = this;
    that.client = connect(mqttUrl);
    that.client.on("connect", function () {
      console.log("成功连接mqtt服务器！");
      that.client.subscribe("/mysmarthome/pub", function (err) {
        if (!err) {
          console.log("成功订阅设备上行数据Topic！");
        }
      });
    });
    that.client.on("message", function (topic, message) {
     // console.log(message);
      let dataFromDev = {};
      dataFromDev = JSON.parse(message);
      console.log(dataFromDev);
      that.Temp = dataFromDev.Temp;
      that.Hum = dataFromDev.Hum;
      that.Light = dataFromDev.Light;
      that.Led = dataFromDev.Led;
      that.Beep = dataFromDev.Beep;

    });
  },
};
</script>

<style lang="scss" scoped>
.header-wrapper {
  padding: 15px;
  background-color: #7d7dd7; //背景颜色
  border-radius: 20px; //圆角
  color: rgba(242, 246, 242, 0.867);
  box-shadow: #7d7d 0px 0px 0px;
  padding: 15px 30px; //文字上下边距
  .header-title {
    display: flex;
    justify-content: space-between;
  }
  .header-text {
    font-size: 32px;
    font-weight: 400;
    display: flex;
    justify-content: space-between;
  }
  .weather-advice {
    margin-top: 20px; //间距
    font-size: 14px;
  }
}
.data-wrapper {
  margin-top: 20px;
  display: flex;
  justify-content: space-between;
  .data {
    background-color: #fff;
    width: 150px;
    height: 80px;
    border-radius: 20px;
    display: flex;
    justify-content: space-around;
    padding: 0 8px;
    box-shadow: #d6d6d6;
    box-shadow: #d6d6d6 0px 0px 5px;
    .data-logo {
      height: 36px;
      width: 36px;
      margin-top: 15px;
    }
    .data-text {
      margin-top: 15px;
      color: #7f7f7f;
      .data-value {
        font-size: 26px;
      }
    }
  }
}
</style>

```



## 3.搭建mqtt 配置域名ssl



### 1、配置EMQX APT源

```sh
curl -s https://assets.emqx.com/scripts/install-emqx-deb.sh | sudo bash

# 安装emqx
sudo apt-get install emqx

# 启动emqx
sudo emqx start
```

emqx相关端口，使用`netstat -tunlp`确认打开

![image-20221202142041603](https://raw.githubusercontent.com/Abear6666/imge/main/images/image-20221202142041603-1670235562053-3.png)





### 2、**访问 Dashboard**

EMQX 提供了 Dashboard，以方便用户通过 Web 页面管理、监控 EMQX 并配置所需的功能。EMQX 成功启动之后可以通过浏览器打开 `http://localhost:18083/`（将 localhost 替换为实际 IP 地址）访问 Dashboard。

Dashboard 的默认用户名为 `admin`，密码为 `public`，第一次登录成功后会提示修改密码。密码修改完成后，我们也可以在 Settings 页面将 Dahshboard 的语言改为 `简体中文`。



### 3、连接测试(TCP协议端口)

![image-20221202143550371](https://raw.githubusercontent.com/Abear6666/imge/main/images/image-20221202143550371-1670235562053-4.png)

至此，我们已完成了 MQTT 服务器的搭建及连接测试，但是该服务器仅仅只能用于测试，若要部署生产环境下可用的 MQTT 服务器，我们还需要进行最重要的认证配置。



### 4、配置认证

默认情况下，EMQX 将允许任何客户端连接，直到用户创建了认证器。认证器将根据客户端提供的认证信息对其进行身份验证，只有认证通过，客户端才能成功连接。接下来我们将演示如何使用 EMQX 内置的数据库进行用户名、密码认证。

![image-20221202145847473](https://raw.githubusercontent.com/Abear6666/imge/main/images/image-20221202145847473.png)

认证创建成功后如下图。接下来我们点击 `用户管理`添加用户。

![image-20221202145934255](https://raw.githubusercontent.com/Abear6666/imge/main/images/image-20221202145934255.png)



### 5、测试认证

接下来我们使用 Dashboard 提供的 Websocket 工具来测试认证是否已配置成功。在连接配置里输入刚才创建的用户名与密码，然后点击`连接`。

![image-20221202150045445](https://raw.githubusercontent.com/Abear6666/imge/main/images/image-20221202150045445.png)



### 6、为服务器添加证书，提高安全性(SSL协议端口8883，WSS协议端口8084)

#### 6.1、SSL自签名证书(自行百度)

可参考以下：

[EMQX 启用双向 SSL/TLS 安全连接 | EMQ](https://www.emqx.com/zh/blog/enable-two-way-ssl-for-emqx)

[(72条消息) emqx配置ssl/tsl实现双向认证_luffy5459的博客-CSDN博客_emqx ssl](https://blog.csdn.net/feinifi/article/details/122614536)

#### 6.2、购买服务器证书

腾讯云搜索ssl

![image-20221205182418238](https://raw.githubusercontent.com/Abear6666/imge/main/images/image-20221205182418238.png)

![image-20221202123659964](https://raw.githubusercontent.com/Abear6666/imge/main/images/image-20221202123659964.png)



下载key文件和pem文件，然后修改`/etc/emqx/emqx.conf`文件

![image-20221202151028169](https://raw.githubusercontent.com/Abear6666/imge/main/images/image-20221202151028169.png)

重启emqx

```sh
service emqx restart
```

进入面板测试

![image-20221206171913800](https://raw.githubusercontent.com/Abear6666/imge/main/images/image-20221206171913800.png)

### MQTT 协议设计原则

MQTT 必须简单容易实现，必须支持 QoS(设备网络环境复杂)，必须轻量且省带宽(因为那时候带宽很贵)，必须数据无关(不关心 Payload 数据格式)，必须有持续地会话感知能力(时刻知道设备是否在线)。下面将介绍 MQTT (3.1.1 版本) 的几个核心特色，分别对应了这几个设计原则的实现

### 灵活的发布订阅和主题设计

发布订阅模式是传统 Client/Server 模式的一种解耦方案。发布者通过 Broker 与消费者之间通信，Broker 的作用是将收到的消息通过某种过滤规则，正确地发送给消费者。发布/订阅模式 相对于 客户端/服务器模式 的好处在于：
● 发布者和消费者之间不必预先知道对方的存在，比如不需要预先沟通对方的 IP Address 和 Port
● 发布者和消费者之间不必同时运行。因为 Broker 是一直运行的。
在 MQTT 协议里， 过滤规则 是 Topic。比如：所有发布到 news 这个 Topic 的消息，都会被 Broker 转发给已经订阅了 news 的订阅者

MQTT 的 Topic 有层级结构，并且支持通配符 + 和 #:
● + 是匹配单层的通配符。比如 news/+ 可以匹配 news/sports，news/+/basketball 可匹配到 news/sports/basketball。
● # 是一到多层的通配符。比如 news/# 可以匹配 news、 news/sports、news/sports/basketball 以及 news/sports/basketball/x 等等。
MQTT 的主题是不要预先创建的，发布者发送消息到某个主题、或者订阅者订阅某个主题的时候，Broker 就会自动创建这个主题

### 带宽消耗最小化

MQTT 协议将协议本身占用的额外消耗最小化，消息头部最小只需要占用 2 个字节。
MQTT 的消息格式分三部分：

>固定长度头部(Fixed Header)，2 个字节，所有消息类型里都有
>
>可变长度头部(Variable Header)，只有某些消息类型里有 
>
>Payload，只有某些消息类型里有

MQTT 的主要消息类型有：
● CONNECT / CONNACK
● PUBLISH / PUBACK
● SUBSCRIBE / SUBACK
● UNSUBSCRIBE / UNSUBACK
● PINGREQ / PINGRESP
● DISCONNECT
其中 PINGREQ / PINGRESP 和 DISCONNECT 报文是不需要可变头部的，也没有 Payload，也就是说它们的报文大小仅仅消耗 2 个字节。
在 CONNECT 报文的可变长度头部里，有个 Protocol Version 的字段。为了节省空间，只有一个字节。所以版本号不是按照字符串 “3.1.1” 存放的，而是使用数字 4 来表示 3.1.1 版本。

### 三个可选的QoS等级

为适应设备不同的网络环境，MQTT 设计了 3 个 QoS 等级，0, 1, 2:
● At most once (0)
● At least once (1)
● Exactly once (2)
QoS 0 是一种 “fire and forget” 的消息发送模式：Sender (可能是 Publisher 或者 Broker) 发送一条消息之后，就不再关心它有没有发送到对方，也不设置任何重发机制。
QoS 1 包含了简单的重发机制，Sender 发送消息之后等待接收者的 ACK，如果没收到 ACK 则重新发送消息。这种模式能保证消息至少能到达一次，但无法保证消息重复。
QoS 2 设计了略微复杂的重发和重复消息发现机制，保证消息到达对方并且严格只到达一次。

> QoS（Quality of Service，服务质量）指一个网络能够利用各种基础技术，为指定的网络通信提供更好的服务能力，
> 是网络的一种安全机制， 是用来解决网络延迟和阻塞等问题的一种技术。QoS的保证对于容量有限的网络来说是十分重要的，
> 特别是对于流多媒体应用，例如VoIP和IPTV等，因为这些应用常常需要固定的传输率，对延时也比较敏感。

### 会话保持

MQTT 没有假设设备或 Broker 使用TCP 的保活机制，而是设计了协议层的保活机制：在 CONNECT 报文里可设置 Keepalive 字段，来设置保活心跳包 PINGREQ/PINGRESP 的发送时间间隔。当长时间无法收到设备的 PINGREQ 的时候，Broker 就会认为设备已经下线
总的来说，Keepalive 有两个作用：
● 发现对端死亡或者网络中断
● 在长时间无消息交互的情况下，保持连接不被网络设备断开
对于那些想要在重新上线后，重新收到离线期间错过的消息的设备，MQTT 设计了持久化连接：在 CONNECT 报文里可设置 CleanSession 字段为 False，则 Broker 会为终端存储：
● 设备所有的订阅
● 还未被设备确认的 QoS1 和 QoS 消息
● 设备离线时错过的消息

### 在线状态感知

MQTT 设计了遗愿(Last Will) 消息，让 Broker 在发现设备异常下线的情况下，帮助设备发布一条遗愿消息到指定的主题。
实际上在某些 MQTT 服务器的实现里 (比如 EMQX)，设备上线或下线的时候 Broker 会通过某些系统主题发布设备状态更新，更符合实际应用场景



服务器终端安装mqtt broker(负责转发数据，client发布、订阅的数据通过其转发)

```sh
# 安装mosqtuitto和客户端
sudo apt install mosquitto mosquitto-clients

# 添加配置文件
sudo vim /etc/mosquitto/conf.d/myconf.conf
#########添加以下内容#########
allow_anonymous false

password_file /etc/mosquitto/pwfile
############################

# 创建用户并设置密码
mosquitto_passwd -c /etc/mosquitto/pwfile <username>  # 之后创建用户去掉-c，否则会覆盖

# 启动服务
sudo service mosquitto start     # stop停止；status查看状态

# 设置开机启动(可选)
sudo systemctl enable mosquitto.service 	# disable关闭
```

设置好mqtt broker之后，修改stm32代码

初始化esp8266

![image-20221201101225643](https://raw.githubusercontent.com/Abear6666/imge/main/images/image-20221201101225643.png)

`esp8266.h`

```c
// 修改wifi ssid和password
// 使用上面服务器的公网ip或者域名，端口默认
void ESP8266_Init(void) 使用AT指令初始化模块，与服务器的1883端口建立tcp连接
```

与平台建立连接，实际上就是使用用户名和密码登陆

![image-20221201102603737](https://raw.githubusercontent.com/Abear6666/imge/main/images/image-20221201102603737.png)

`MQTT_PacketConnect`将用户密码等信息打包成`连接消息组包`，通过`ESP8266_SendData`发送出去

![image-20221201103137121](https://raw.githubusercontent.com/Abear6666/imge/main/images/image-20221201103137121.png)

```c
//==========================================================
//	函数名称：	MQTT_PacketConnect
//
//	函数功能：	连接消息组包
//
//	入口参数：	user：用户名：产品ID
//				password：密码：鉴权信息或apikey
//				devid：设备ID
//				cTime：连接保持时间
//				clean_session：离线消息清除标志
//				qos：重发标志
//				will_topic：异常离线topic
//				will_msg：异常离线消息
//				will_retain：消息推送标志
//				mqttPacket：包指针
//
//	返回参数：	0-成功		其他-失败
//
//	说明：		
//==========================================================
uint8 MQTT_PacketConnect(const int8 *user, const int8 *password, const int8 *devid,
						uint16 cTime, uint1 clean_session, uint1 qos,
						const int8 *will_topic, const int8 *will_msg, int32 will_retain,
						MQTT_PACKET_STRUCTURE *mqttPacket)

// 这里的user和password要和前面	mosquitto_passwd -c /etc/mosquitto/pwfile <username> 中使用的用户名和密码一样
```



## 4.小程序连接mqtt



服务器搭建mqtt并且域名证书配置完毕后：

在设置 项目设置里边记得关掉

![image-20221208094610207](https://raw.githubusercontent.com/Abear6666/imge/main/images/image-20221208094610207.png)



```js
<script>

 import { connect } from "mqtt/dist/mqtt.js";//声明使用mqtt的connect函数
 const mqttUrl = 'wxs://mqtt.bear321.cn:8084/mqtt' 
 

export default {
  data () {
    return {
      client:{},
      Temp:40,
      Hum:0,
      Light:0,
      Led:false,
      Beep:false,
    };
  },

  components: {},

  methods: {
  },

  // created () {
  //   // let app = getApp()
  // }
  onShow(){
    var that = this
    that.client = connect(mqttUrl);//连接服务器
    that.client.on('connect',function (){
      console.log("成功连接mqtt服务器！")
      that.client.subscribe("/mysmarthome/pub",function (err){

        if(!err){
          console.log("成功订阅设备上行数据Topic！")
        }

      })
    })
    that.client.on('message',function (topic,message){
      console.log(topic)
      console.log(message)
      let dataFromDev = {}
      dataFromDev = JSON.parse(message)
      console.log(dataFromDev)
    })
  }
}
</script>
```

成功显示：

![image-20221202150155977](https://raw.githubusercontent.com/Abear6666/imge/main/images/image-20221202150155977.png)



## 5.bug解决：



### 1.（真机调试不了）

```
在Network调试里看见wss://mqtt.xxxxxx??.xyz:8084/mqtt被当成XHR的Type了。开发工具里面是WS的Type开发工具里是能连接上8084站点的但是在手机调试里就连接错误所以根本没订阅那个pub和sub

解决方法：mqtt版本的问题 npm uninstall mqtt然后npm install mqtt@4.1.0 --save
或者：
【安卓设备】（一般发生在安卓设备上）无法加密连接，但是其他例如【苹果设备、小程序模拟器、浏览器】等可以正常加密连接的，则需要将/etc/emqx文件夹下的emqx.conf文件中的【listener.ssl.external.cacertfile】这项配置注释取消，并配置成与【listener.ssl.external.certfile】一样的证书路径；同理【listener.wss.external.cacertfile】也要取消注释并配置成与【listener.wss.external.certfile】一样的证书路径！
```

### 2.（小程序连接不了mqtt）

```
最近由于mqtt这个npm包更新了，所以按照教程进行npm install安装项目依赖的时候mqtt会被自动安装成最新版本，导致出现【xxx.createConnection is not a function】错误，删除mqtt包安装低版本的即可。在项目目录下命令行输入【 npm uninstall mqtt】卸载mqtt包然后输入【 npm install mqtt@3.0.0 --save】重新安装
```

### 3.（Powershell中提示管理员权限）

```
如果在Powershell中运行命令的时候出现“xxxxxx，因为在此系统上禁止运行脚本 xxxxxx”红字错误，尝试参考 【https://blog.csdn.net/larpland/article/details/101349586】解决
```

```
export INSTALL_PATH=/mnt/data/install
export LD_LIBRARY_PATH="/lib:/lib/arm-linux-gnueabihf:/usr/lib:/usr/local/lib:/mnt/system/lib:/mnt/system/usr/lib:/mnt/system/usr/lib/3rd:$INSTALL_PATH/lib"
export PATH="/usr/local/bin:/usr/bin:/bin:/usr/local/sbin:/usr/sbin:/sbin:/mnt/system/usr/bin:/mnt/system/usr/sbin:/mnt/data/bin:/mnt/data/sbin"
export HASPUSER_PREFIX=/mnt/data/auth


insmod /mnt/system/ko/cfbcopyarea.ko
insmod /mnt/system/ko/cfbfillrect.ko
insmod /mnt/system/ko/cfbimgblt.ko
insmod /mnt/system/ko/cvi_fb.ko
insmod /mnt/system/ko/3rd/gt9xx.ko


./sample_dsi # 使用LCD接口
devmem 0x0a088000 32 0xC0
./sac_application.bin &

```

### 4.（安装sass-loader@5.0.0出现Fix the upstream）

![image-20221205221312496](https://raw.githubusercontent.com/Abear6666/imge/main/images/image-20221205221312496.png)

解决方法：

```
 npm install npm@6.14.8 -g
```

