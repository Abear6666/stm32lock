// pages/login/index.js
import Toast from '../../@vant/weapp/toast/toast';
Page({
  /**
   * 页面的初始数据
   */
  data: {
    isLogin: true,
    inputUserName: "",
    inputPassWord: "",
    inputContect: "",
    showFindPW: false,
    showResetPW: false
  },

  /**
   * 页面相关事件处理函数--登陆/注册按钮点击触发回调函数onClick
   */
  onClick: function (event) {
    var that = this;
    Toast.loading({
      duration: 0, //持续展示Toast
      forbidClick: true, //禁止背景能够被点击
      message: that.data.isLogin ? "登陆中..." : "注册中..."
    });
    //模拟请求服务器延时1000ms
    setTimeout(() => {
      //箭头函数！不懂的同学百度一下JS的ES6语法
      if (!that.data.isLogin) {
        wx.setStorage({
          key: "user",
          data: {
            username: that.data.inputUserName,
            password: that.data.inputPassWord,
            contect: that.data.inputContect
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
              that.data.inputUserName === res.data.username &&
              that.data.inputPassWord === res.data.password
            ) {
              Toast.success("登陆成功");
              //500ms后跳转到首页
              setTimeout(() => {
                wx.switchTab({
                  url: "/pages/index/index"
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

  /**
   * 页面相关事件处理函数--切换按钮点击触发回调函数onOptionClick
   */
  onOptionClick: function (event) {
    this.setData({
      isLogin: !this.data.isLogin
    });
    console.log(
      `切换注册/登陆按钮被点击了！当前处于${
        this.data.isLogin ? "登陆" : "注册"
      }状态！`
    );
  },

  /**
   * 页面相关事件处理函数--忘记密码按钮点击回调函数onForgetClick
   */
  onForgetClick: function (event) {
    this.setData({
      showFindPW: true
    });
  },

  /**
   * 页面相关事件处理函数--安全校验提示框确定按钮回调函数onFindPWConfirm
   */
  onFindPWConfirm: function (event) {
    var that = this;
    wx.getStorage({
      key: "user",
      success(res) {
        console.log(res.data);
        if (that.data.inputContect === res.data.contect) {
          that.setData({
            showResetPW: true,
            inputUserName: res.data.username
          })
          console.log("找到用户 ：", res.data.username);
        } else {
          Toast.fail("无该用户信息");
          that.data.inputContect = "";
        }
      },
      fail(res) {
        console.log(res);
        Toast.fail("数据库暂无注册用户");
        that.data.inputContect = "";
      }
    });
  },

  /**
   * 页面相关事件处理函数--安全校验提示框取消按钮回调函数onFindPWCancel
   */
  onFindPWCancel: function (event) {
    this.setData({
      showFindPW: false,
      inputContect: ""
    });
  },

  /**
   * 页面相关事件处理函数--重置密码提示框确定按钮回调函数onResetPWConfirm
   */
  onResetPWConfirm: function (event) {
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

  /**
   * 页面相关事件处理函数--重置密码提示框取消按钮回调函数onResetPWCancel
   */
  onResetPWCancel: function (event) {
    this.setData({
      showResetPW: false,
      inputUserName: "",
      inputPassWord: "",
      inputContect: ""
    });
  }
})