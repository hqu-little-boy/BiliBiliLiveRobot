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
            text: "直播数据统计"
            font.pixelSize: 24
            font.weight: Font.Medium
            color: "#333333"
        }
        
        // 实时数据卡片
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
                        text: "📊"
                        font.pixelSize: 18
                    }
                    Text {
                        text: "实时数据概览"
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
                
                // 数据卡片网格
                GridLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    columns: 4
                    columnSpacing: 15
                    rowSpacing: 15
                    
                    StatsCard {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        title: "当前观看人数"
                        value: "1,258"
                        valueColor: "#e8a798"
                    }
                    
                    StatsCard {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        title: "新增关注"
                        value: "24"
                        valueColor: "#6a5acd"
                    }
                    
                    StatsCard {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        title: "收到礼物"
                        value: "18"
                        valueColor: "#ffb11b"
                    }
                    
                    StatsCard {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        title: "弹幕数量"
                        value: root.danmakuModel ? root.danmakuModel.rowCount() : "0"
                        valueColor: "#4caf50"
                    }
                }
                
                // 趋势图区域占位
                Rectangle {
                    Layout.fillWidth: true
                    Layout.preferredHeight: 120
                    radius: 6
                    color: "#f9f9f9"
                    border.width: 1
                    border.color: "#e0e0e0"
                    
                    Text {
                        anchors.centerIn: parent
                        text: "📈 弹幕趋势图（待实现）"
                        font.pixelSize: 14
                        color: "#999999"
                    }
                }
            }
        }
        
        // 历史数据卡片
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
                
                // 卡片标题
                Row {
                    spacing: 10
                    Text {
                        text: "🕐"
                        font.pixelSize: 18
                    }
                    Text {
                        text: "历史数据"
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
                
                // 历史数据网格
                GridLayout {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    columns: 4
                    columnSpacing: 15
                    rowSpacing: 15
                    
                    StatsCard {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        title: "今日弹幕"
                        value: "1,456"
                        subtitle: "较昨日 +12%"
                        valueColor: "#4caf50"
                    }
                    
                    StatsCard {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        title: "今日关注"
                        value: "89"
                        subtitle: "较昨日 +23%"
                        valueColor: "#6a5acd"
                    }
                    
                    StatsCard {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        title: "今日礼物"
                        value: "156"
                        subtitle: "较昨日 +8%"
                        valueColor: "#ffb11b"
                    }
                    
                    StatsCard {
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        title: "峰值观看"
                        value: "2,344"
                        subtitle: "17:30 达到"
                        valueColor: "#e8a798"
                    }
                }
            }
        }
        
        // 详细统计表格占位
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
                
                Row {
                    spacing: 10
                    Text {
                        text: "📋"
                        font.pixelSize: 18
                    }
                    Text {
                        text: "详细统计"
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
                
                Rectangle {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    radius: 6
                    color: "#f9f9f9"
                    border.width: 1
                    border.color: "#e0e0e0"
                    
                    Text {
                        anchors.centerIn: parent
                        text: "📊 详细统计表格（待实现）"
                        font.pixelSize: 14
                        color: "#999999"
                    }
                }
            }
        }
    }
}
