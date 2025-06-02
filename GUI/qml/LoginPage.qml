import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    color: "#f8f5f2"
    
    property var bilibiliApi
    
    signal loginSuccess()
    
    ColumnLayout {
        anchors.centerIn: parent
        spacing: 30
        
        // Logo和标题
        ColumnLayout {
            Layout.alignment: Qt.AlignHCenter
            spacing: 15
            
            Rectangle {
                Layout.alignment: Qt.AlignHCenter
                width: 80
                height: 80
                radius: 40
                color: "#e8a798"
                
                Text {
                    anchors.centerIn: parent
                    text: "🤖"
                    font.pixelSize: 36
                }
            }
            
            Text {
                Layout.alignment: Qt.AlignHCenter
                text: "B站弹幕机器人"
                font.pixelSize: 28
                font.weight: Font.Medium
                color: "#333333"
            }
        }
        
        // 登录卡片
        Rectangle {
            Layout.alignment: Qt.AlignHCenter
            width: 400
            height: 450
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
                anchors.centerIn: parent
                spacing: 20
                
                Text {
                    Layout.alignment: Qt.AlignHCenter
                    text: "请使用B站客户端扫描二维码登录"
                    font.pixelSize: 14
                    color: "#666666"
                }
                
                // 二维码区域
                Rectangle {
                    Layout.alignment: Qt.AlignHCenter
                    width: 220
                    height: 220
                    radius: 8
                    color: "#f9f9f9"
                    border.width: 1
                    border.color: "#e0e0e0"
                    
                    Rectangle {
                        id: qrCodePlaceholder
                        anchors.centerIn: parent
                        width: 200
                        height: 200
                        radius: 4
                        color: "#f0f0f0"
                        
                        // 棋盘格背景模拟二维码
                        property int squareSize: 10
                        
                        Repeater {
                            model: 20 * 20
                            Rectangle {
                                x: (index % 20) * qrCodePlaceholder.squareSize
                                y: Math.floor(index / 20) * qrCodePlaceholder.squareSize
                                width: qrCodePlaceholder.squareSize
                                height: qrCodePlaceholder.squareSize
                                color: (Math.random() > 0.5) ? "#333333" : "#f0f0f0"
                            }
                        }
                        
                        Rectangle {
                            anchors.centerIn: parent
                            width: 150
                            height: 30
                            color: "#f0f0f0"
                            radius: 4
                            
                            Text {
                                anchors.centerIn: parent
                                text: "二维码加载区域"
                                font.pixelSize: 12
                                color: "#999999"
                            }
                        }
                    }
                }
                
                // 登录状态
                Rectangle {
                    Layout.alignment: Qt.AlignHCenter
                    Layout.fillWidth: true
                    Layout.margins: 20
                    height: 40
                    radius: 6
                    color: root.bilibiliApi ? (root.bilibiliApi.isLoggedIn ? "#e8f5e8" : "#f0e6e4") : "#f0e6e4"
                    
                    Row {
                        anchors.centerIn: parent
                        spacing: 8
                        
                        Text {
                            id: statusIcon
                            text: root.bilibiliApi ? (root.bilibiliApi.isLoggedIn ? "✅" : "🔄") : "🔄"
                            font.pixelSize: 14
                            
                            RotationAnimation {
                                id: rotationAnimation
                                target: statusIcon
                                property: "rotation"
                                from: 0
                                to: 360
                                duration: 2000
                                loops: Animation.Infinite
                                running: root.bilibiliApi ? !root.bilibiliApi.isLoggedIn : true
                            }
                        }
                        
                        Text {
                            text: root.bilibiliApi ? (root.bilibiliApi.loginStatus || "等待扫码中...") : "等待扫码中..."
                            font.pixelSize: 14
                            color: "#666666"
                        }
                    }
                }
                
                // 按钮
                Button {
                    Layout.alignment: Qt.AlignHCenter
                    text: root.bilibiliApi ? (root.bilibiliApi.isLoggedIn ? "登录成功" : "模拟登录成功") : "模拟登录成功"
                    font.pixelSize: 14
                    enabled: root.bilibiliApi ? !root.bilibiliApi.isLoggedIn : true
                    
                    background: Rectangle {
                        radius: 6
                        color: {
                            if (!parent.enabled) return "#cccccc"
                            return parent.pressed ? "#d89788" : "#e8a798"
                        }
                        
                        Behavior on color {
                            ColorAnimation { duration: 150 }
                        }
                    }
                    
                    contentItem: Text {
                        text: parent.text
                        font: parent.font
                        color: parent.enabled ? "#ffffff" : "#999999"
                        horizontalAlignment: Text.AlignHCenter
                        verticalAlignment: Text.AlignVCenter
                    }
                    
                    onClicked: {
                        if (root.bilibiliApi && !root.bilibiliApi.isLoggedIn) {
                            root.bilibiliApi.simulateLogin()
                        }
                    }
                }
            }
        }
    }
    
    Component.onCompleted: {
        if (root.bilibiliApi) {
            root.bilibiliApi.generateQRCode()
        }
    }
}
