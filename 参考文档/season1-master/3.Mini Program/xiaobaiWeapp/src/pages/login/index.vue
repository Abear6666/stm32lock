 <!-- 页面视图 -->
<template>
  <div>
    <!-- 顶部header -->
    <div class="header">
      <div v-if="isLogin">
        <div class="header-title">请登录</div>
        <div class="header-info">Please Login Your Account</div>
      </div>
      <div v-else>
        <div class="header-title">请注册</div>
        <div class="header-info">Please Register Your Account</div>
      </div>
    </div>
    <!-- 主体body -->
    <div class="body">
      <!-- 登陆表单 -->
      <div class="login-form">
        <van-field placeholder="请输入用户名" :value="inputUserName" @change="onUserNameChange" />
        <van-field
          type="password"
          placeholder="请输入密码"
          :value="inputPassWord"
          @change="onPassWordChange"
        />
        <!-- 绑定手机号输入框 -->
        <div v-if="!isLogin">
          <van-field placeholder="请输入找回密码绑定手机号" :value="inputContect" @change="onContectChange" />
        </div>
      </div>
      <!-- 登陆/注册按钮 -->
      <van-button slot="button" round block color="#3d7ef9" @click="onClick">{{isLogin?'登陆':'注册'}}</van-button>
      <!-- 切换按钮和忘记密码按钮 -->
      <div class="other-option">
        <div @click="onOptionClick">
          <span>{{isLogin?'注册账户':'登陆账户'}}</span>
        </div>
        <span style="margin: 0 30px">|</span>
        <div @click="onForgetClick">
          <span>忘记密码</span>
        </div>
      </div>
      <!-- 版权 -->
      <div class="copyright-wrapper">
        <span class="copyright">Power By HUAQING</span>
      </div>
      <!-- 安全校验提示框 -->
      <van-dialog
        use-slot
        title="找回密码校验"
        :show="showFindPW"
        show-cancel-button
        transition="fade"
        @confirm="onFindPWConfirm"
        @cancel="onFindPWCancel"
      >
        <van-field
          label="手机号"
          title-width="60px"
          placeholder="请输入找回密码绑定手机号"
          :value="inputContect"
          @change="onContectChange"
        />
      </van-dialog>
      <!-- 重置密码提示框 -->
      <van-dialog
        use-slot
        title="重置密码"
        :show="showResetPW"
        show-cancel-button
        transition="fade"
        @confirm="onResetPWConfirm"
        @cancel="onResetPWCancel"
      >
        <van-field label="用户名" title-width="60px" :value="inputUserName" readonly />
        <van-field
          label="新密码"
          type="password"
          title-width="60px"
          placeholder="请输入新密码"
          :value="inputPassWord"
          @change="onPassWordChange"
        />
      </van-dialog>
      <!-- 轻提示 -->
      <van-toast id="van-toast" />
    </div>
  </div>
</template>

<!-- 页面逻辑 -->
<script>
import Toast from "vant-weapp/dist/toast/toast";
export default {
  data() {
    return {
      isLogin: true,
      inputUserName: "",
      inputPassWord: "",
      inputContect: "",
      showFindPW: false,
      showResetPW: false
    };
  },
  methods: {
    // 当用户名输入框中的数据发生变化时的回调函数onUserNameChange
    onUserNameChange(event) {
      var that = this;
      that.$set(that, "inputUserName", event.mp.detail);
      console.log("当前输入的用户名 = ", event.mp.detail);
    },
    // 当密码输入框中的数据发生变化时的回调函数onPassWordChange
    onPassWordChange(event) {
      var that = this;
      that.$set(that, "inputPassWord", event.mp.detail);
      console.log("当前输入的密码 = ", event.mp.detail);
    },
    // 当找回密码联系电话输入框中的数据发生变化时的回调函数onContectChange
    onContectChange(event) {
      var that = this;
      that.$set(that, "inputContect", event.mp.detail);
      console.log("当前输入的手机号 = ", event.mp.detail);
    },
    // 登陆/注册按钮点击触发回调函数onClick
    onClick(event) {
      var that = this;
      Toast.loading({
        duration: 0, //持续展示Toast
        forbidClick: true, //禁止背景能够被点击
        message: that.isLogin ? "登陆中..." : "注册中..."
      });
      //模拟请求服务器延时1000ms
      setTimeout(() => {
        //箭头函数！不懂的同学百度一下JS的ES6语法
        if (!that.isLogin) {
          wx.setStorage({
            key: "user",
            data: {
              username: that.inputUserName,
              password: that.inputPassWord,
              contect: that.inputContect
            },
            success(res) {
              console.log(res);
              console.log("注册成功！");
              Toast.success("注册成功");
            },
            fail(res) {
              console.log(res);
              console.log("注册失败！");
              Toast.success("注册失败");
            }
          });
        } else {
          wx.getStorage({
            key: "user",
            success(res) {
              console.log(res.data);
              if (
                that.inputUserName === res.data.username &&
                that.inputPassWord === res.data.password
              ) {
                Toast.success("登陆成功");
                //500ms后跳转到首页
                setTimeout(() => {
                  wx.switchTab({
                    url: "/pages/index/main"
                  });
                }, 500);
              } else {
                Toast.fail("用户名或密码错误");
              }
            },
            fail(res) {
              console.log(res);
              Toast.fail("数据库暂无注册用户");
            }
          });
        }
      }, 1000);
    },
    // 切换按钮点击触发回调函数onOptionClick
    onOptionClick(event) {
      var that = this;
      that.isLogin = !that.isLogin;
      console.log(
        `切换注册/登陆按钮被点击了！当前处于${
          that.isLogin ? "登陆" : "注册"
        }状态！`
      );
    },
    // 忘记密码按钮点击回调函数onForgetClick
    onForgetClick(event) {
      var that = this;
      that.showFindPW = true;
    },
    // 安全校验提示框确定按钮回调函数onFindPWConfirm
    onFindPWConfirm(event) {
      var that = this;
      wx.getStorage({
        key: "user",
        success(res) {
          console.log(res.data);
          if (that.inputContect === res.data.contect) {
            that.inputUserName = res.data.username;
            that.showResetPW = true;
            console.log("找到用户 ：", res.data.username);
          } else {
            Toast.fail("无该用户信息");
            that.inputContect = "";
          }
        },
        fail(res) {
          console.log(res);
          Toast.fail("数据库暂无注册用户");
          that.inputContect = "";
        }
      });
    },
    // 安全校验提示框取消按钮回调函数onFindPWCancel
    onFindPWCancel(event) {
      var that = this;
      that.showFindPW = false
      that.inputContect = "";
    },
    // 重置密码提示框确定按钮回调函数onResetPWConfirm
    onResetPWConfirm(event) {
      var that = this;
      wx.setStorage({
        key: "user",
        data: {
          username: that.inputUserName,
          password: that.inputPassWord,
          contect: that.inputContect
        },
        success(res) {
          console.log(res);
          console.log("密码重置成功！");
          Toast.success("密码重置成功");
        },
        fail(res) {
          console.log(res);
          console.log("密码重置失败！");
          Toast.success("密码重置失败");
        }
      });
    },
    // 重置密码提示框取消按钮回调函数onResetPWCancel
    onResetPWCancel(event) {
      var that = this;
      that.showResetPW = false
      that.inputUserName = "";
      that.inputPassWord = "";
      that.inputContect = "";
    }
  }
};
</script>

<!-- 页面样式 -->
<style lang="scss" scoped>
.header {
  height: 100px;
  padding: 25px 0;
  background-color: #3d7ef9;
  .header-title {
    font-size: 28px;
    font-weight: 500;
    color: #fff;
    margin-left: 20px;
  }
  .header-info {
    font-size: 14px;
    color: #fff;
    margin-left: 20px;
  }
}
.body {
  padding: 20px;
  margin-top: -20px;
  border-radius: 15px 15px 0 0;
  background-color: #fff;
  .login-form {
    margin-bottom: 30px;
  }
  .other-option {
    display: flex;
    justify-content: center;
    margin-top: 20px;
    color: #9f9f9f;
  }
  .copyright-wrapper {
    display: flex;
    justify-content: center;
    .copyright {
      color: #d6d6d6;
      position: fixed;
      bottom: 50px;
    }
  }
}
</style>
