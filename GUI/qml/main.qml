import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import BilibiliBot 1.0

ApplicationWindow {
    id: window
    width: 1200
    height: 800
    minimumWidth: 800
    minimumHeight: 600
    visible: true
    title: qsTr("B站弹幕机器人")

    property bool isLoggedIn: true // 登录状态

    Component.onCompleted: {
        // 连接单例信号
        BilibiliApi.loginSuccess.connect(function () {
            window.isLoggedIn = true
            DanmakuModel.startSimulation()
        })

        BilibiliApi.loginStatusChanged.connect(function () {
            window.isLoggedIn = BilibiliApi.isLoggedIn
        })

        BilibiliApi.danmakuReceived.connect(function (user, message, type) {
            DanmakuModel.addMessage(user, message, type)
            if (SettingsManager.ttsEnabled) {
                TTSManager.speak(user + "说：" + message)
            }
        })

        // 同步TTS设置
        TTSManager.setIsEnabled(SettingsManager.ttsEnabled)
        TTSManager.setVolume(SettingsManager.ttsVolume)
        TTSManager.setSpeed(SettingsManager.ttsSpeed)
        TTSManager.setVoice(SettingsManager.ttsVoice)
    }

    // 主内容
    Loader {
        id: mainLoader
        anchors.fill: parent
        source: window.isLoggedIn ? "MainPage.qml" : "LoginPage.qml"

        onLoaded: {
            if (item && window.isLoggedIn) {
                // 单例模式下不需要传递任何属性
                DanmakuModel.startSimulation()
            }
        }
    }
}
