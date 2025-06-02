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
            text: "PK功能设置"
            font.pixelSize: 24
            font.weight: Font.Medium
            color: "#333333"
        }

        // PK设置卡片
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 650
            radius: 8
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

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 25
                spacing: 20

                // 卡片标题
                Row {
                    spacing: 10
                    Text {
                        text: "👊"
                        font.pixelSize: 18
                    }
                    Text {
                        text: "PK对手识别与相关设置"
                        font.pixelSize: 18
                        font.weight: Font.Medium
                        color: "#333333"
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    height: 1
                    color: "#f0f0f0"
                }

                // 启用PK输出对手信息开关
                RowLayout {
                    Layout.fillWidth: true

                    ToggleSwitch {
                        id: enablePKOpponentInfoToggle
                        checked: true
                    }
                    Text {
                        text: "启用 PK 输出对手信息"
                        font.pixelSize: 14
                        color: "#333333"
                    }
                }

                // 对方主播信息提示
                Column {
                    Layout.fillWidth: true
                    spacing: 8

                    Text {
                        text: "对方主播信息提示"
                        font.pixelSize: 14
                        color: "#666666"
                    }
                    TextField {
                        id: opponentInfoMessageInput
                        width: parent.width
                        text: "当前PK对手：{主播名}，粉丝数：{粉丝数}"
                        placeholderText: "请输入对方主播信息提示"
                        font.pixelSize: 14

                        background: Rectangle {
                            radius: 6
                            color: "#fcfcfc"
                            border.width: 1
                            border.color: parent.activeFocus ? "#e8a798" : "#e0e0e0"
                        }
                    }
                }

                // 启用PK输出对手用户串门识别
                RowLayout {
                    Layout.fillWidth: true

                    ToggleSwitch {
                        id: enablePKVisitorMessageToggle
                        checked: true
                    }
                    Text {
                        text: "启用 PK 对方用户串门识别"
                        font.pixelSize: 14
                        color: "#333333"
                    }
                }
                // 对方用户串门识别
                Column {
                    Layout.fillWidth: true
                    spacing: 8

                    Text {
                        text: "对方用户串门识别"
                        font.pixelSize: 14
                        color: "#666666"
                    }
                    TextField {
                        id: opponentVisitorMessageInput
                        width: parent.width
                        text: "欢迎来自{对方主播}直播间的{用户名}！"
                        placeholderText: "请输入对方用户串门识别"
                        font.pixelSize: 14

                        background: Rectangle {
                            radius: 6
                            color: "#fcfcfc"
                            border.width: 1
                            border.color: parent.activeFocus ? "#e8a798" : "#e0e0e0"
                        }
                    }
                }

                // 保存按钮
                // Button {
                //     text: "保存设置"
                //     Layout.preferredWidth: 120
                //     font.pixelSize: 14
                //
                //     background: Rectangle {
                //         color: parent.pressed ? "#d89788" : (parent.hovered ? "#e19590" : "#e8a798")
                //         radius: 6
                //     }
                //
                //     onClicked: {
                //         console.log("PK功能设置已保存")
                //     }
                // }
            }
        }
    }
}
