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
            text: "弹幕欢迎设置"
            font.pixelSize: 24
            font.weight: Font.Medium
            color: "#333333"
        }

        // 欢迎设置卡片
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 700
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
                        text: "👋"
                        font.pixelSize: 18
                    }
                    Text {
                        text: "欢迎设置"
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

                // 启用开关
                RowLayout {
                    Layout.fillWidth: true

                    ToggleSwitch {
                        checked: SettingsManager.welcomeEnabled()
                        onCheckedChanged: {
                            SettingsManager.setWelcomeEnabled(checked)
                        }
                    }

                    Text {
                        text: "启用弹幕欢迎功能"
                        font.pixelSize: 14
                        color: "#333333"
                    }
                }

                // 普通用户欢迎词
                Column {
                    Layout.fillWidth: true
                    spacing: 8

                    Text {
                        text: "普通用户欢迎词"
                        font.pixelSize: 14
                        color: "#666666"
                    }

                    TextField {
                        id: normalUserTextField
                        width: parent.width
                        placeholderText: "输入普通用户欢迎词..."
                        font.pixelSize: 14

                        background: Rectangle {
                            radius: 6
                            color: "#fcfcfc"
                            border.width: 1
                            border.color: parent.activeFocus ? "#e8a798" : "#e0e0e0"
                        }

                        Component.onCompleted: {
                            text = SettingsManager.normalUserWelcome()
                        }

                        onTextChanged: {
                            if (!normalUserTextField.activeFocus) {
                                return // 避免在程序设置时触发
                            }
                            SettingsManager.normalUserWelcome = text
                        }
                    }

                    Text {
                        text: "提示：{用户名} 将被替换为实际的用户名"
                        font.pixelSize: 12
                        color: "#999999"
                    }
                }

                // 舰长用户欢迎词
                Column {
                    Layout.fillWidth: true
                    spacing: 8

                    Text {
                        text: "舰长用户欢迎词"
                        font.pixelSize: 14
                        color: "#666666"
                    }

                    TextField {
                        id: captainUserTextField
                        width: parent.width
                        placeholderText: "输入舰长用户欢迎词..."
                        font.pixelSize: 14

                        background: Rectangle {
                            radius: 6
                            color: "#fcfcfc"
                            border.width: 1
                            border.color: parent.activeFocus ? "#e8a798" : "#e0e0e0"
                        }

                        Component.onCompleted: {
                            text = SettingsManager.captainUserWelcome()
                        }

                        onTextChanged: {
                            if (!captainUserTextField.activeFocus) {
                                return // 避免在程序设置时触发
                            }
                            SettingsManager.setCaptainUserWelcome(text)
                            // SettingsManager.captainUserWelcome = text
                        }
                    }
                }

                // 指定用户欢迎设置
                Column {
                    Layout.fillWidth: true
                    spacing: 8

                    Text {
                        text: "指定用户欢迎设置"
                        font.pixelSize: 14
                        color: "#666666"
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 10

                        TextField {
                            Layout.preferredWidth: 150
                            placeholderText: "输入用户名..."
                            font.pixelSize: 14

                            background: Rectangle {
                                radius: 6
                                color: "#fcfcfc"
                                border.width: 1
                                border.color: parent.activeFocus ? "#e8a798" : "#e0e0e0"
                            }
                        }

                        TextField {
                            Layout.preferredWidth: 200
                            placeholderText: "自定义欢迎词..."
                            font.pixelSize: 14

                            background: Rectangle {
                                radius: 6
                                color: "#fcfcfc"
                                border.width: 1
                                border.color: parent.activeFocus ? "#e8a798" : "#e0e0e0"
                            }
                        }

                        Button {
                            text: "添加"

                            background: Rectangle {
                                radius: 6
                                color: parent.pressed ? "#d89788" : "#e8a798"
                            }

                            contentItem: Text {
                                text: parent.text
                                font.pixelSize: 14
                                color: "#ffffff"
                                horizontalAlignment: Text.AlignHCenter
                                verticalAlignment: Text.AlignVCenter
                            }
                        }
                    }

                    Text {
                        text: "可以为特定用户设置专属的欢迎词"
                        font.pixelSize: 12
                        color: "#999999"
                    }
                }

                // 已添加的用户列表占位
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 120
                    radius: 6
                    color: "#f9f9f9"
                    border.width: 1
                    border.color: "#e0e0e0"

                    Text {
                        anchors.centerIn: parent
                        text: "👥 指定用户列表（待实现）"
                        font.pixelSize: 14
                        color: "#999999"
                    }
                }

                // 保存按钮
                // Button {
                //     Layout.alignment: Qt.AlignLeft
                //     text: "💾 保存设置"
                //
                //     background: Rectangle {
                //         radius: 6
                //         color: parent.pressed ? "#d89788" : "#e8a798"
                //     }
                //
                //     contentItem: Text {
                //         text: parent.text
                //         font.pixelSize: 14
                //         color: "#ffffff"
                //         horizontalAlignment: Text.AlignHCenter
                //         verticalAlignment: Text.AlignVCenter
                //     }
                //
                //     onClicked: {
                //         SettingsManager.saveSettings()
                //     }
                // }
            }
        }
    }
}
