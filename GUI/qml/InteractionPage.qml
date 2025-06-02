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
            text: "互动回复设置"
            font.pixelSize: 24
            font.weight: Font.Medium
            color: "#333333"
        }

        // 关注答谢设置卡片
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 350
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
                        text: "❤️"
                        font.pixelSize: 18
                    }
                    Text {
                        text: "关注答谢设置"
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
                        id: enableFollowThanksToggle
                        checked: SettingsManager.followThanksEnabled()
                        onCheckedChanged: {
                            SettingsManager.setFollowThanksEnabled(checked)
                        }
                    }
                    Text {
                        text: "启用关注答谢功能"
                        font.pixelSize: 14
                        color: "#333333"
                    }
                }

                // 关注答谢词
                Column {
                    Layout.fillWidth: true
                    spacing: 8

                    Text {
                        text: "关注答谢词"
                        font.pixelSize: 14
                        color: "#666666"
                    }
                    TextField {
                        id: followThanksMessageInput
                        width: parent.width
                        placeholderText: "请输入关注答谢词"
                        font.pixelSize: 14
                        text: SettingsManager.followThanksMessage()
                        background: Rectangle {
                            radius: 6
                            color: "#fcfcfc"
                            border.width: 1
                            border.color: parent.activeFocus ? "#e8a798" : "#e0e0e0"
                        }

                        onTextChanged: {
                            SettingsManager.setFollowThanksMessage(text)
                        }
                    }

                    Text {
                        text: "提示：{用户名} 将被替换为实际的用户名"
                        font.pixelSize: 12
                        color: "#999999"
                    }
                }

                // // 保存按钮
                // Button {
                //     Layout.alignment: Qt.AlignLeft
                //     text: "💾 保存关注设置"
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
                //         SettingsManager.saveFollowSettings()
                //         console.log("关注答谢设置已保存")
                //     }
                // }
            }
        }

        // 分享感谢设置卡片
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 350
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
                        text: "🔗"
                        font.pixelSize: 18
                    }
                    Text {
                        text: "分享感谢设置"
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
                        id: enableShareThanksToggle
                        checked: SettingsManager.shareThanksEnabled()
                        onCheckedChanged: {
                            SettingsManager.setShareThanksEnabled(checked)
                        }
                    }
                    Text {
                        text: "启用分享感谢功能"
                        font.pixelSize: 14
                        color: "#333333"
                    }
                }

                // 分享感谢词
                Column {
                    Layout.fillWidth: true
                    spacing: 8

                    Text {
                        text: "分享感谢词"
                        font.pixelSize: 14
                        color: "#666666"
                    }
                    TextField {
                        id: shareThanksMessageInput
                        width: parent.width
                        placeholderText: "请输入分享感谢词"
                        font.pixelSize: 14
                        text: SettingsManager.shareThanksMessage()
                        background: Rectangle {
                            radius: 6
                            color: "#fcfcfc"
                            border.width: 1
                            border.color: parent.activeFocus ? "#e8a798" : "#e0e0e0"
                        }

                        onTextChanged: {
                            SettingsManager.setShareThanksMessage(text)
                        }
                    }

                    Text {
                        text: "提示：{用户名} 将被替换为实际的用户名"
                        font.pixelSize: 12
                        color: "#999999"
                    }
                }

            }
        }
    }
}
