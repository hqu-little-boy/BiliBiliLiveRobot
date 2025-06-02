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

        Text {
            text: "系统设置"
            font.pixelSize: 24
            font.weight: Font.Medium
            color: "#333333"
        }

        // 连接设置卡片
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 250
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

                Row {
                    spacing: 10
                    Text {
                        text: "🔗"
                        font.pixelSize: 18
                    }
                    Text {
                        text: "直播间连接设置"
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

                Column {
                    Layout.fillWidth: true
                    spacing: 8

                    Text {
                        text: "直播间房间号"
                        font.pixelSize: 14
                        color: "#666666"
                    }

                    RowLayout {
                        Layout.fillWidth: true
                        spacing: 10

                        TextField {
                            Layout.fillWidth: true
                            text: SettingsManager.roomId()
                            placeholderText: "请输入直播间房间号..."
                            font.pixelSize: 14

                            background: Rectangle {
                                radius: 6
                                color: "#fcfcfc"
                                border.width: 1
                                border.color: parent.activeFocus ? "#e8a798" : "#e0e0e0"
                            }

                            onTextChanged: {
                                if (SettingsManager) {
                                    SettingsManager.roomId = text
                                }
                            }
                        }

                        Button {
                            text: BilibiliApi.isConnected() ? "断开连接" : "连接房间"

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

                            onClicked: {
                                if (BilibiliApi) {
                                    if (BilibiliApi.isConnected()) {
                                        BilibiliApi.disconnectFromRoom()
                                    } else {
                                        BilibiliApi.connectToRoom(SettingsManager ? (SettingsManager.roomId || "") : "")
                                    }
                                }
                            }
                        }
                    }
                }

                RowLayout {
                    Layout.fillWidth: true

                    ToggleSwitch {
                        checked: SettingsManager.autoConnect()
                        onCheckedChanged: {
                            if (SettingsManager) {
                                SettingsManager.setAutoConnect(checked)
                            }
                        }
                    }

                    Text {
                        text: "启动时自动连接直播间"
                        font.pixelSize: 14
                        color: "#333333"
                    }
                }

                Row {
                    spacing: 8
                    Text {
                        text: "连接状态："
                        font.pixelSize: 14
                        color: "#666666"
                    }
                    Text {
                        text: BilibiliApi.isConnected() ? "已连接" : "未连接"
                        font.pixelSize: 14
                        color: BilibiliApi.isConnected() ? "#4caf50" : "#f44336"
                        font.weight: Font.Medium
                    }
                }
            }
        }

        // 应用设置卡片
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 300
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

                Row {
                    spacing: 10
                    Text {
                        text: "⚙️"
                        font.pixelSize: 18
                    }
                    Text {
                        text: "应用设置"
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

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 20

                    // Button {
                    //     text: "💾 保存所有设置"
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
                    //         if (SettingsManager) {
                    //             SettingsManager.saveSettings()
                    //         }
                    //     }
                    // }

                    Button {
                        text: "🔄 重置为默认"

                        background: Rectangle {
                            radius: 6
                            color: "#f0f0f0"
                            border.width: 1
                            border.color: "#e0e0e0"
                        }

                        contentItem: Text {
                            text: parent.text
                            font.pixelSize: 14
                            color: "#666666"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                        }

                        onClicked: {
                            if (SettingsManager) {
                                SettingsManager.resetToDefaults()
                            }
                        }
                    }

                    // Button {
                    //     text: "🚪 切换账号"
                    //
                    //     background: Rectangle {
                    //         radius: 6
                    //         color: parent.pressed ? "#d32f2f" : "#f44336"
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
                    //         if (BilibiliApi) {
                    //             BilibiliApi.logout()
                    //         }
                    //     }
                    // }
                }

                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 120
                    radius: 6
                    color: "#f9f9f9"
                    border.width: 1
                    border.color: "#e0e0e0"

                    Column {
                        anchors.centerIn: parent
                        spacing: 8

                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: "📋 应用信息"
                            font.pixelSize: 14
                            color: "#666666"
                            font.weight: Font.Medium
                        }

                        Text {
                            anchors.horizontalCenter: parent.horizontalCenter
                            text: "B站弹幕机器人 v1.0.0"
                            font.pixelSize: 12
                            color: "#999999"
                        }
                    }
                }
            }
        }
    }
}
