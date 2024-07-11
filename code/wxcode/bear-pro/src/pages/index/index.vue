<template>
  <div class="wrapper">
    <div class="header-wrapper">
      <div class="header-title">
        <span>日志输出:{{ Log }}</span>
        <span></span>
      </div>
      <div class="header-text">
        <span></span>
        <span></span>
      </div>
      <div class="weather-advice">{{ Msg }}</div>
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
            <div class="data-value">{{ Hum }} %</div>
          </div>
        </div>
      </div>

      <div class="data-wrapper">
        <div class="data">
          <img class="data-logo" src="/static/images/guangzhao.png" />
          <div class="data-text">
            <div class="data-title">门禁</div>
            <div class="data-value">
              <switch @change="onDoorChange" :checked="Door" color="#3d7ef9" />
            </div>
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
        <div class="data">
          <img class="data-logo" src="/static/images/baojin.png" />
          <div class="data-text">
            <div class="data-title">门禁电源</div>
            <switch @change="onPowerChange" :checked="Power" color="#3d7ef9" />
          </div>
        </div>
      </div>

      <div class="data-wrapper">
        <button @click="addfin">添加指纹</button>
        <button @click="rmfin">删除指纹</button>
        <button @click="addcard">添加卡号</button>
        <button @click="rmcard">删除卡号</button>
      </div>
    </div>
    <div class="body">
      <!-- 弹框内容 -->
      <div class="vueMdelBox">
        <div :hidden="!vueShowModel" class="vueContant">
          <div class="vueTitle">报警提示！</div>
          <div class="vueDetail">触发报警，有人尝试爆破门禁！</div>
          <!-- 确定取消按钮 -->
          <div class="vueBtn">
            <p @click="vueShowModel = false">取消</p>
            <p @click="close_power">关闭电源</p>
          </div>
        </div>
        <!-- 背景黑色蒙版 -->
        <div
          class="vueBgdCol"
          :hidden="!vueShowModel"
          @click="vueShowModel = false"
        ></div>
      </div>

      <button @click="Get_card">查询卡号</button>
      <div class="vueMdelBox">
        <div :hidden="!vueShowModel1" class="vueContant">
          <div class="vueTitle">卡号查询成功！</div>
          <div class="vueDetail">{{ Card_Msg }}</div>
          <!-- 确定取消按钮 -->
          <div class="vueBtn">
            <p @click="vueShowModel1 = false">取消</p>
            <p @click="confirm">确定</p>
          </div>
        </div>
        <!-- 背景黑色蒙版 -->
        <div
          class="vueBgdCol"
          :hidden="!vueShowModel1"
          @click="vueShowModel1 = false"
        ></div>
      </div>

      <button @click="Get_fin">查询指纹</button>
      <div class="vueMdelBox">
        <div :hidden="!vueShowModel2" class="vueContant">
          <div class="vueTitle">指纹查询成功！</div>
          <div class="vueDetail">当前指纹存储的数量为：{{ Fin_Msg }}</div>
          <!-- 确定取消按钮 -->
          <div class="vueBtn">
            <p @click="vueShowModel2 = false">取消</p>
            <p @click="confirm">确定</p>
          </div>
        </div>
        <!-- 背景黑色蒙版 -->
        <div
          class="vueBgdCol"
          :hidden="!vueShowModel2"
          @click="vueShowModel2 = false"
        ></div>
      </div>
    </div>
  </div>
</template>

<script>
import { connect } from "mqtt/dist/mqtt.js";

const mqttUrl = "wxs://mqtt.bear321.cn:8084/mqtt";
const sub = "/myhome/sub";
const pub = "/myhome/pub";

export default {
  data() {
    return {
      client: {},
      Temp: 0,
      Hum: 0,
      Light: 0,
      Led: false,
      Beep: false,
      Door: false,
      Power: false,
      Msg: "",
      Log: "",
      Card_Msg: "", //卡号信息
      Fin_Msg: "", //指纹信息
      vueShowModel: false, //默认不显示 警报提示
      vueShowModel1: false, //默认不显示 卡号查询提示
      vueShowModel2: false, //默认不显示 指纹查询显示
    };
  },

  components: {},

  methods: {
    // 点击按钮打开弹框
    Get_card() {
      var that = this;

      this.client.publish(
        sub,
        '{"target":"Get_card","value":1}',
        function (err) {
          if (!err) {
            console.log("成功下发命令---查询卡号");
            that.Log = "成功下发命令---查询卡号";
          }
        }
      );
    },
    Get_fin() {
      var that = this;

      this.client.publish(
        sub,
        '{"target":"Get_fin","value":1}',
        function (err) {
          if (!err) {
            console.log("成功下发命令---查询指纹");
            that.Log = "成功下发命令---查询指纹";
          }
        }
      );
    },
    // 关闭电源
    close_power() {
      var that = this;
      this.vueShowModel = false;
      this.client.publish(sub, '{"target":"Power","value":0}', function (err) {
        if (!err) {
          console.log("成功下发命令---关闭电源");
          that.Log = "成功下发命令---关闭电源";
        }
      });
    },
    // 点击确定按钮模态框消失
    confirm() {
      this.vueShowModel1 = false;
      this.vueShowModel2 = false;
    },
    onDoorChange(event) {
      console.log("666！");
      var that = this;
      console.log(event.mp.detail);
      let sw = event.mp.detail.value;
      that.Door = sw;
      if (sw) {
        that.client.publish(sub, '{"target":"Door","value":1}', function (err) {
          if (!err) {
            console.log("成功下发命令---开门");

            that.Log = "成功下发命令---开门";
          }
        });
      } else {
        that.client.publish(sub, '{"target":"Door","value":0}', function (err) {
          if (!err) {
            console.log("成功下发命令---关门");
            that.Log = "成功下发命令---关门";
          }
        });
      }
    },
    onLedChange(event) {
      var that = this;
      console.log(event.mp.detail);
      let sw = event.mp.detail.value;
      that.Led = sw;
      if (sw) {
        that.client.publish(sub, '{"target":"LED","value":1}', function (err) {
          if (!err) {
            console.log("成功下发命令---开灯");
            that.Log = "成功下发命令---开灯";
          }
        });
      } else {
        that.client.publish(sub, '{"target":"LED","value":0}', function (err) {
          if (!err) {
            console.log("成功下发命令---关灯");
            that.Log = "成功下发命令---关灯";
          }
        });
      }
    },
    onBeepChange(event) {
      var that = this;
      console.log(event.mp.detail);
      let sw = event.mp.detail.value;
      that.Beep = sw;
      if (sw) {
        that.client.publish(sub, '{"target":"BEEP","value":1}', function (err) {
          if (!err) {
            console.log("成功下发命令---打开警报器");
            that.Log = "成功下发命令---打开警报器";
          }
        });
      } else {
        that.client.publish(sub, '{"target":"BEEP","value":0}', function (err) {
          if (!err) {
            console.log("成功下发命令---关闭警报器");
            that.Log = "成功下发命令---关闭警报器";
          }
        });
      }
    },
    addfin(event) {
      var that = this;
      this.client.publish(sub, '{"target":"Addfin","value":1}', function (err) {
        if (!err) {
          console.log("成功下发命令---添加指纹");
          that.Log = "成功下发命令---添加指纹";
        }
      });
    },
    rmfin(event) {
      this.client.publish(sub, '{"target":"Rmfin","value":1}', function (err) {
        if (!err) {
          console.log("成功下发命令---删除指纹");
          that.Log = "成功下发命令---删除指纹";
        }
      });
    },
    addcard(event) {
      var that = this;

      this.client.publish(
        sub,
        '{"target":"Addcard","value":1}',
        function (err) {
          if (!err) {
            console.log("成功下发命令---添加卡号");
            that.Log = "成功下发命令---添加卡号";
          }
        }
      );
    },
    rmcard(event) {
      this.client.publish(sub, '{"target":"Rmcard","value":1}', function (err) {
        if (!err) {
          console.log("成功下发命令---删除卡号");
          that.Log = "成功下发命令---删除卡号";
        }
      });
    },
    onPowerChange(event) {
      var that = this;
      console.log(event.mp.detail);
      let sw = event.mp.detail.value;
      that.Power = sw;
      if (sw) {
        that.client.publish(
          sub,
          '{"target":"Power","value":1}',
          function (err) {
            if (!err) {
              console.log("成功下发命令---打开门禁电源");
              that.Log = "成功下发命令---打开门禁电源";
            }
          }
        );
      } else {
        that.client.publish(
          sub,
          '{"target":"Power","value":0}',
          function (err) {
            if (!err) {
              console.log("成功下发命令---关闭门禁电源");
              that.Log = "成功下发命令---关闭门禁电源";
            }
          }
        );
      }
    },
  },

  // created () {
  //   // let app = getApp()
  // }
  onShow() {
    var that = this;
    that.client = connect(mqttUrl);
    that.client.on("connect", function () {
      console.log("成功连接mqtt服务器！");
      that.client.subscribe("/myhome/pub", function (err) {
        if (!err) {
          console.log("成功订阅设备上行数据Topic！");
          that.Msg = "服务器连接成功！";
        } else {
          console.log("订阅设备上行数据失败！");
          that.Msg = "服务器连接失败！";
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
      that.Door = dataFromDev.Door;
      that.Power = dataFromDev.Power;
      if (dataFromDev.Warning == 1) {
        that.vueShowModel = true;
      }
      if (dataFromDev.value == 1) {
        that.Card_Msg = dataFromDev.Card_Msg;

        that.vueShowModel1 = true;
      }
      console.log(dataFromDev.Fin_Msg);
      if (dataFromDev.Fin_Msg >= 0) {
        that.Fin_Msg = dataFromDev.Fin_Msg;
        that.vueShowModel2 = true;
      }
    });
  },
};
</script>


<style lang="scss" scoped>
.vueBgdCol {
  position: fixed;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  background-color: black;
  z-index: 99;
  -moz-opacity: 0.6;
  opacity: 0.6;
  filter: alpha(opacity=88);
}

.vueContant {
  padding: 10px 0px 0px 0px;
  position: fixed;
  top: 36%;
  left: 50%;
  width: 80%;
  margin-left: -40%;
  background-color: white;
  z-index: 100;
  overflow: auto;
  border-radius: 10px;
}

.vueTitle {
  display: flex;
  justify-content: center;
  font-weight: bold;
}

.vueDetail {
  font-size: 14px;
  color: #646566;
  display: flex;
  justify-content: center;
  padding: 16px;
}

.vueBtn {
  display: flex;
}

.vueBtn p {
  width: 50%;
  padding: 5% 0%;
  font-weight: bold;
  text-align: center;
  border-top: 1px solid #ebedf0;
}

.vueBtn p:last-child {
  color: #ee0a24;
  border-left: 1px solid #ebedf0;
}

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
.button-sp-area {
  margin: 0 auto;
}
</style>
