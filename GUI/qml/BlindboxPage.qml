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
            text: "盲盒统计增强版"
            font.pixelSize: 24
            font.weight: Font.Medium
            color: "#333333"
        }

        // 盲盒提示设置卡片
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 150
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
                        text: "🔔"
                        font.pixelSize: 18
                    }
                    Text {
                        text: "盲盒开启提示设置"
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
                        id: notificationToggle
                        checked: true
                        onCheckedChanged: {
                            console.log("盲盒开启提示功能" + (checked ? "已启用" : "已禁用"))
                        }
                    }

                    Text {
                        text: "启用盲盒开启提示功能"
                        font.pixelSize: 14
                        color: "#333333"
                    }
                }
                //
                // // 提示内容设置
                // Column {
                //     Layout.fillWidth: true
                //     spacing: 8
                //
                //     Text {
                //         text: "提示内容"
                //         font.pixelSize: 14
                //         color: "#666666"
                //     }
                //
                //     TextField {
                //         id: notificationInput
                //         width: parent.width
                //         text: "恭喜 {用户名} 开启了今日限定盲盒，获得了{奖品名}！"
                //         placeholderText: "输入盲盒开启提示内容..."
                //         font.pixelSize: 14
                //
                //         background: Rectangle {
                //             radius: 6
                //             color: "#fcfcfc"
                //             border.width: 1
                //             border.color: parent.activeFocus ? "#e8a798" : "#e0e0e0"
                //         }
                //     }
                //
                //     Text {
                //         text: "提示：可用变量 {用户名} {盲盒名} {奖品名}"
                //         font.pixelSize: 12
                //         color: "#999999"
                //     }
                // }
                //
                // // 可用变量标签
                // Column {
                //     Layout.fillWidth: true
                //     spacing: 8
                //
                //     Text {
                //         text: "快速插入变量"
                //         font.pixelSize: 14
                //         color: "#666666"
                //     }
                //
                //     Flow {
                //         width: parent.width
                //         spacing: 8
                //
                //         Repeater {
                //             model: ["{用户名}", "{盲盒名}", "{奖品名}", "{价值}"]
                //             delegate: Rectangle {
                //                 width: variableText.width + 16
                //                 height: 24
                //                 radius: 12
                //                 color: "#e8a798"
                //
                //                 Text {
                //                     id: variableText
                //                     anchors.centerIn: parent
                //                     text: modelData
                //                     font.pixelSize: 12
                //                     color: "#ffffff"
                //                 }
                //
                //                 MouseArea {
                //                     anchors.fill: parent
                //                     cursorShape: Qt.PointingHandCursor
                //                     onClicked: {
                //                         var cursorPos = notificationInput.cursorPosition
                //                         var currentText = notificationInput.text
                //                         var newText = currentText.substring(0, cursorPos) +
                //                                      modelData +
                //                                      currentText.substring(cursorPos)
                //                         notificationInput.text = newText
                //                         notificationInput.cursorPosition = cursorPos + modelData.length
                //                         notificationInput.focus = true
                //                     }
                //                 }
                //             }
                //         }
                //     }
                // }
            }
        }

        // 数据概览卡片
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 200
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
                        text: "📊"
                        font.pixelSize: 18
                    }
                    Text {
                        text: "数据概览"
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

                // 统计数据网格
                GridLayout {
                    Layout.fillWidth: true
                    columns: 4
                    rowSpacing: 10
                    columnSpacing: 10

                    Repeater {
                        model: [
                            {value: "74", label: "今日盲盒", trend: "+5", isPositive: true},
                            {value: "¥1,286", label: "总价值", trend: "+12%", isPositive: true},
                            {value: "+¥326", label: "今日盈亏", trend: "", isPositive: true},
                            {value: "18", label: "稀有盲盒", trend: "+2", isPositive: true}
                        ]

                        delegate: Rectangle {
                            Layout.fillWidth: true
                            Layout.preferredHeight: 90
                            radius: 6
                            color: "#f8f9fa"
                            border.width: 1
                            border.color: "#e0e0e0"

                            ColumnLayout {
                                anchors.centerIn: parent
                                spacing: 5

                                Text {
                                    text: modelData.value
                                    font.pixelSize: 20
                                    font.weight: Font.Bold
                                    color: modelData.isPositive ? "#4caf50" : "#f44336"
                                    horizontalAlignment: Text.AlignHCenter
                                    Layout.alignment: Qt.AlignHCenter
                                }

                                Text {
                                    text: modelData.label
                                    font.pixelSize: 12
                                    color: "#666666"
                                    horizontalAlignment: Text.AlignHCenter
                                    Layout.alignment: Qt.AlignHCenter
                                }

                                Text {
                                    text: modelData.trend
                                    font.pixelSize: 10
                                    color: "#999999"
                                    horizontalAlignment: Text.AlignHCenter
                                    Layout.alignment: Qt.AlignHCenter
                                    visible: modelData.trend !== ""
                                }
                            }
                        }
                    }
                }
            }
        }

        // 排行榜卡片
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 450
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
                        text: "🏆"
                        font.pixelSize: 18
                    }
                    Text {
                        text: "盲盒排行榜"
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

                // 排行榜类型标签
                Row {
                    spacing: 10

                    Repeater {
                        model: [
                            {text: "价值排行", type: "value"},
                            {text: "数量排行", type: "count"},
                            {text: "盈亏排行", type: "profit"}
                        ]

                        delegate: Rectangle {
                            width: tabText.width + 20
                            height: 32
                            radius: 16
                            color: index === 0 ? "#e8a798" : "#f5f5f5"

                            Text {
                                id: tabText
                                anchors.centerIn: parent
                                text: modelData.text
                                font.pixelSize: 12
                                color: index === 0 ? "#ffffff" : "#666666"
                            }

                            MouseArea {
                                anchors.fill: parent
                                cursorShape: Qt.PointingHandCursor
                                onClicked: {
                                    // 切换标签逻辑（暂时省略实现）
                                    console.log("切换到", modelData.text)
                                }
                            }
                        }
                    }
                }

                // 排行榜列表
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    color: "#f9f9f9"
                    radius: 6
                    border.width: 1
                    border.color: "#e0e0e0"

                    ListView {
                        anchors.fill: parent
                        anchors.margins: 10
                        model: ListModel {
                            ListElement {
                                rank: 1; username: "开盒大师"; value: "¥856"; isProfit: true
                            }
                            ListElement {
                                rank: 2; username: "欧皇降临"; value: "¥742"; isProfit: true
                            }
                            ListElement {
                                rank: 3; username: "幸运星"; value: "¥658"; isProfit: true
                            }
                            ListElement {
                                rank: 4; username: "盒子爱好者"; value: "¥542"; isProfit: true
                            }
                            ListElement {
                                rank: 5; username: "抽奖达人"; value: "¥436"; isProfit: true
                            }
                        }

                        delegate: Rectangle {
                            width: parent.width
                            height: 45
                            color: index % 2 === 0 ? "#ffffff" : "#f8f9fa"
                            radius: 4

                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 12
                                spacing: 15

                                Text {
                                    text: "#" + model.rank
                                    font.pixelSize: 14
                                    font.weight: Font.Bold
                                    color: model.rank <= 3 ? "#e8a798" : "#999999"
                                    Layout.preferredWidth: 30
                                }

                                Text {
                                    text: model.username
                                    font.pixelSize: 14
                                    color: "#333333"
                                    Layout.fillWidth: true
                                }

                                Text {
                                    text: model.value
                                    font.pixelSize: 14
                                    font.weight: Font.Medium
                                    color: model.isProfit ? "#4caf50" : "#f44336"
                                }
                            }
                        }
                    }
                }
            }
        }

        // 控制按钮
        Row {
            Layout.alignment: Qt.AlignLeft
            spacing: 10

            // Button {
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
            //         console.log("盲盒设置已保存")
            //     }
            // }

            Button {
                text: "🔄 刷新数据"

                background: Rectangle {
                    radius: 6
                    color: parent.pressed ? "#e0e0e0" : "#f5f5f5"
                    border.width: 1
                    border.color: "#cccccc"
                }

                contentItem: Text {
                    text: parent.text
                    font.pixelSize: 14
                    color: "#333333"
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                }

                onClicked: {
                    console.log("数据已刷新")
                }
            }
        }
    }
}
