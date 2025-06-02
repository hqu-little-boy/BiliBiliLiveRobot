import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import BilibiliBot 1.0
import "components"

ScrollView {
    id: root

    ColumnLayout {
        width: root.width
        spacing: 20

        // 页面标题
        Text {
            text: "账号登录信息"
            font.pixelSize: 24
            font.weight: Font.Medium
            color: "#333333"
        }

        // 登录信息卡片
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 450
            radius: 12
            color: "#ffffff"

            // 使用纯QML阴影效果替代DropShadow
            Rectangle {
                anchors.fill: parent
                anchors.topMargin: 2
                color: "#10000000"
                radius: parent.radius
                opacity: 0.1
                z: -1
            }

            // 装饰元素
            Rectangle {
                anchors.top: parent.top
                anchors.right: parent.right
                width: 120
                height: 120
                radius: 120
                color: "#f8e8a798"
                opacity: 0.05
            }

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 30
                spacing: 25

                // 用户头像区域
                Item {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 120

                    Rectangle {
                        id: avatarContainer
                        anchors.horizontalCenter: parent.horizontalCenter
                        width: 100
                        height: 100
                        radius: 50
                        color: "#f8e8a798"
                        border.width: 2
                        border.color: "#e8a798"

                        Text {
                            anchors.centerIn: parent
                            text: "👤"
                            font.pixelSize: 42
                            color: "#e8a798"
                        }
                    }
                }

                // 用户信息
                Column {
                    Layout.fillWidth: true
                    spacing: 15

                    // 用户名
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "设计达人"
                        font.pixelSize: 24
                        font.weight: Font.Medium
                        color: "#333333"
                    }

                    // 用户等级
                    Row {
                        anchors.horizontalCenter: parent.horizontalCenter
                        spacing: 8

                        Rectangle {
                            width: levelText.width + 20
                            height: 28
                            radius: 14
                            color: "#fff3e0"

                            Row {
                                anchors.centerIn: parent
                                spacing: 6

                                Text {
                                    text: "👑"
                                    font.pixelSize: 14
                                }

                                Text {
                                    id: levelText
                                    text: "高级会员"
                                    font.pixelSize: 14
                                    color: "#ffb11b"
                                    font.weight: Font.Medium
                                }
                            }
                        }
                    }

                    // 用户ID
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: "ID: 20230001"
                        font.pixelSize: 16
                        color: "#666666"
                    }

                    // 登录状态
                    Row {
                        anchors.horizontalCenter: parent.horizontalCenter
                        spacing: 8

                        Rectangle {
                            width: statusText.width + 20
                            height: 26
                            radius: 13
                            color: "#e8f5e8"

                            Row {
                                anchors.centerIn: parent
                                spacing: 6

                                Rectangle {
                                    width: 8
                                    height: 8
                                    radius: 4
                                    color: "#4caf50"
                                    anchors.verticalCenter: parent.verticalCenter
                                }

                                Text {
                                    id: statusText
                                    text: "已登录"
                                    font.pixelSize: 14
                                    color: "#4caf50"
                                    font.weight: Font.Medium
                                }
                            }
                        }
                    }
                }

                // 操作按钮
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 15

                    Button {
                        Layout.fillWidth: true
                        Layout.preferredHeight: 45
                        text: "🚪 切換账号"
                        font.pixelSize: 16
                        font.weight: Font.Medium

                        background: Rectangle {
                            radius: 8
                            color: "transparent"
                            border.width: 1
                            border.color: "#f44336"
                        }

                        contentItem: Text {
                            text: parent.text
                            font: parent.font
                            color: "#f44336"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }

                        onClicked: {
                            logoutDialog.open()
                            console.log("用户请求切换账号")
                        }
                    }
                }
            }
        }
    }

    // 退出登录确认对话框
    Dialog {
        id: logoutDialog
        anchors.centerIn: parent
        width: 300
        height: 150
        title: "退出登录"

        contentItem: Column {
            spacing: 20
            anchors.centerIn: parent

            Text {
                text: "确定要退出登录吗？"
                font.pixelSize: 16
                color: "#333333"
                anchors.horizontalCenter: parent.horizontalCenter
            }

            Row {
                spacing: 20
                anchors.horizontalCenter: parent.horizontalCenter

                Button {
                    text: "取消"
                    onClicked: logoutDialog.close()

                    background: Rectangle {
                        radius: 6
                        color: "#f0f0f0"
                    }
                }

                Button {
                    text: "确定"
                    onClicked: {
                        logoutDialog.close()
                        console.log("用户已退出登录")
                    }

                    background: Rectangle {
                        radius: 6
                        color: "#f44336"
                    }

                    contentItem: Text {
                        text: parent.text
                        color: "white"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                }
            }
        }
    }
}
