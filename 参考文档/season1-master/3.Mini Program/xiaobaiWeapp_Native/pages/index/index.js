// index.js
// 获取应用实例

const { connect } = require("../../utils/mqtt4/mqtt");
/******************* 必须要改！！不然得不到天气！！ ******************/
const hefengKey = "8d90114ce9334ccbbcf6dec72f534bd6"; //  和风天气Web api的key
/******************* 可能需要你修改的部分 ******************/
const mqttHost = "broker.emqx.io"; //mqtt 服务器域名/IP
const mqttPort = 8084; //mqtt 服务器域名/IP

const hefengVIP = false; //  和风天气是免费的api（false）还是付费api（true）

const deviceSubTopic = "/mysmarthome/sub"; //  设备订阅topic（小程序发布命令topic）
const devicePubTopic = "/mysmarthome/pub"; //  设备发布topic（小程序订阅数据topic）

/********************* 一般不用动这些 ********************/

const mpSubTopic = devicePubTopic;
const mpPubTopic = deviceSubTopic;

const mqttUrl = `wxs://${mqttHost}:${mqttPort}/mqtt`; //  mqtt连接路径

const hefengApi = "https://api.qweather.com/v7"; //  和风天气付费API前缀
const hefengFreeApi = "https://devapi.qweather.com/v7"; //  和风天气免费API前缀

const hefengWeather = `${hefengVIP ? hefengApi : hefengFreeApi}/weather/now?`; //  和风天气实时天气api
const hefengAir = `${hefengVIP ? hefengApi : hefengFreeApi}/air/now?`; //  和风天气空气质量api

const geoApi = "https://geoapi.qweather.com/v2/city/lookup?" //  地理位置api（用来获取经纬度对应的城市/城区名字）

Page({
  data: {
    client: {},
    Temp: 0,
    Hum: 0,
    Light: 0,
    Led: false,
    Beep: false,
    area: "请求中", //城区
    city: "请求中", //城市
    airText: "请求中", //空气优良
    airValue: 0, //空气指数
    weather: "请求中", //天气
    weatherAdvice: "今天天气不错", //天气建议
  },
  onLedChange(event) {
    var that = this;
    console.log(event.detail);
    let sw = event.detail.value;
    that.setData({
      Led: sw
    })
    if (sw) {
      that.data.client.publish(mpPubTopic, '{"target":"LED","value":1}', function (err) {
        if (!err) {
          console.log("成功下发命令——开灯");
        }
      });
    } else {
      that.data.client.publish(mpPubTopic, '{"target":"LED","value":0}', function (err) {
        if (!err) {
          console.log("成功下发命令——关灯");
        }
      });
    }
  },
  onBeepChange(event) {
    var that = this;
    console.log(event.detail);
    let sw = event.detail.value;
    that.setData({
      Beep: sw
    })
    if (sw) {
      that.data.client.publish(mpPubTopic, '{"target":"BEEP","value":1}', function (err) {
        if (!err) {
          console.log("成功下发命令——打开报警器");
        }
      });
    } else {
      that.data.client.publish(mpPubTopic, '{"target":"BEEP","value":0}', function (err) {
        if (!err) {
          console.log("成功下发命令——关闭报警器");
        }
      });
    }
  },
  onShow() {
    var that = this;
    wx.showToast({
      title: "连接服务器....",
      icon: "loading",
      duration: 10000,
      mask: true,
    });
    let second = 10;
    var toastTimer = setInterval(() => {
      second--;
      if (second) {
        wx.showToast({
          title: `连接服务器...${second}`,
          icon: "loading",
          duration: 1000,
          mask: true,
        });
      } else {
        clearInterval(toastTimer);
        wx.showToast({
          title: "连接失败",
          icon: "error",
          mask: true,
        });
      }
    }, 1000);
    that.setData({
      client: connect(mqttUrl)
    })

    that.data.client.on("connect", function () {
      console.log("成功连接mqtt服务器！");
      clearInterval(toastTimer);
      wx.showToast({
        title: "连接成功",
        icon: "success",
        mask: true,
      });
      // 一秒后订阅主题
      setTimeout(() => {
        that.data.client.subscribe(mpSubTopic, function (err) {
          if (!err) {
            console.log("成功订阅设备上行数据Topic!");
            wx.showToast({
              title: "订阅成功",
              icon: "success",
              mask: true,
            });
          }
        });
      }, 1000);
    });
    that.data.client.on("message", function (topic, message) {
      console.log(topic);
      // message是16进制的Buffer字节流
      let dataFromDev = {};
      // 尝试进行JSON解析
      try {
        dataFromDev = JSON.parse(message);
        console.log(dataFromDev);
        that.setData({
          Temp: dataFromDev.Temp,
          Hum: dataFromDev.Hum,
          Light: dataFromDev.Light,
          Led: dataFromDev.Led,
          Beep: dataFromDev.Beep
        })
      } catch (error) {
        // 解析失败错误捕获并打印（错误捕获之后不会影响程序继续运行）
        console.log(error);
      }
    })

    // 获取天气相关数据
    wx.getLocation({
      type: "wgs84",
      success(res) {
        const latitude = res.latitude;
        const longitude = res.longitude;
        const key = hefengKey;
        wx.request({
          url: `${geoApi}location=${longitude},${latitude}&key=${key}`, //获取地理位置
          success(res) {
            console.log(res.data);
            if (res.data.code == "401") {
              console.error("HUAQING --- 请检查你的和风天气API或Key是否正确！");
              return;
            }
            try {
              const {
                location
              } = res.data;
              that.setData({
                area: location[0].name, //城区
                city: location[0].adm2 //城市
              })
            } catch (error) {
              console.error(error);
            }
          },
        });
        wx.request({
          url: `${hefengWeather}location=${longitude},${latitude}&key=${key}`, //获取实时天气数据
          success(res) {
            console.log(res.data);
            if (res.data.code == "401") {
              console.error("HUAQING --- 请检查你的和风天气API或Key是否正确！");
              return;
            }
            try {
              const {
                now
              } = res.data;
              that.setData({
                weather: now.text, // 天气
              })
            } catch (error) {
              console.error(error);
            }

          },
        });
        wx.request({
          url: `${hefengAir}location=${longitude},${latitude}&key=${key}`, //获取空气数据
          success(res) {
            console.log(res.data);
            if (res.data.code == "401") {
              console.error("HUAQING --- 请检查你的和风天气API或Key是否正确！");
              return;
            }
            try {
              const {
                now
              } = res.data;
              that.setData({
                airText: now.category, //空气质量
                airValue: now.aqi //空气指数
              })
            } catch (error) {
              console.error(error);
            }

          },
        });
      },
    });
  }
})