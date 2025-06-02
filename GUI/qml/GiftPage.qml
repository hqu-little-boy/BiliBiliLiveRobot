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
            text: "礼物答谢设置"
            font.pixelSize: 24
            font.weight: Font.Medium
            color: "#333333"
        }

        // 礼物设置卡片
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
                        text: "🎁"
                        font.pixelSize: 18
                    }
                    Text {
                        text: "自定义礼物答谢词和延迟设置"
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
                        id: enableGiftThanksToggle
                        checked: SettingsManager.giftEnabled()
                        onCheckedChanged: {
                            SettingsManager.setGiftEnabled(checked)
                        }
                    }
                    Text {
                        text: "启用礼物答谢功能"
                        font.pixelSize: 14
                        color: "#333333"
                    }
                }

                // 答谢延迟
                Column {
                    Layout.fillWidth: true
                    spacing: 8

                    Text {
                        text: "礼物答谢延迟 (秒)"
                        font.pixelSize: 14
                        color: "#666666"
                    }
                    SpinBox {
                        id: giftThanksDelaySpinBox
                        from: 0
                        to: 10
                        value: SettingsManager.giftThanksDelay()
                        editable: true
                        font.pixelSize: 14

                        background: Rectangle {
                            radius: 6
                            color: "#fcfcfc"
                            border.width: 1
                            border.color: parent.activeFocus ? "#e8a798" : "#e0e0e0"
                        }
                        onValueChanged: {
                            SettingsManager.setGiftThanksDelay(value)
                        }
                    }
                }

                // 默认答谢词
                Column {
                    Layout.fillWidth: true
                    spacing: 8

                    Text {
                        text: "默认答谢词"
                        font.pixelSize: 14
                        color: "#666666"
                    }
                    TextField {
                        id: defaultThanksMessageInput
                        width: parent.width
                        placeholderText: "输入默认答谢词..."
                        text: "感谢 {用户名} 赠送的 {礼物名}！"
                        font.pixelSize: 14

                        background: Rectangle {
                            radius: 6
                            color: "#fcfcfc"
                            border.width: 1
                            border.color: parent.activeFocus ? "#e8a798" : "#e0e0e0"
                        }
                    }
                }

                // 礼物价值阈值
                Column {
                    Layout.fillWidth: true
                    spacing: 8

                    Text {
                        text: "礼物价值阈值"
                        font.pixelSize: 14
                        color: "#666666"
                    }
                    RowLayout {
                        Layout.fillWidth: true
                        Slider {
                            id: giftValueThresholdSlider
                            Layout.fillWidth: true
                            from: 0
                            to: 100
                            value: 50
                            stepSize: 1

                            background: Rectangle {
                                x: giftValueThresholdSlider.leftPadding
                                y: giftValueThresholdSlider.topPadding + giftValueThresholdSlider.availableHeight / 2 - height / 2
                                implicitWidth: 200
                                implicitHeight: 4
                                width: giftValueThresholdSlider.availableWidth
                                height: implicitHeight
                                radius: 2
                                color: "#e0e0e0"

                                Rectangle {
                                    width: giftValueThresholdSlider.visualPosition * parent.width
                                    height: parent.height
                                    color: "#e8a798"
                                    radius: 2
                                }
                            }

                            handle: Rectangle {
                                x: giftValueThresholdSlider.leftPadding + giftValueThresholdSlider.visualPosition * (giftValueThresholdSlider.availableWidth - width)
                                y: giftValueThresholdSlider.topPadding + giftValueThresholdSlider.availableHeight / 2 - height / 2
                                implicitWidth: 20
                                implicitHeight: 20
                                radius: 10
                                color: giftValueThresholdSlider.pressed ? "#d89788" : "#e8a798"
                                border.color: "#ffffff"
                                border.width: 2
                            }
                        }
                        Text {
                            text: giftValueThresholdSlider.value + " 电池"
                            font.pixelSize: 14
                            color: "#666666"
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
                //         console.log("礼物答谢设置已保存")
                //     }
                // }
            }
        }
    }
}
