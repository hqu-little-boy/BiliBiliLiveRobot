import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: root
    
    property string userName: ""
    property string messageText: ""
    property string messageTime: ""
    property string messageType: "normal"
    
    width: parent.width
    height: contentColumn.height + 24
    radius: 6
    color: "#faf8f7"
    border.width: 1
    border.color: messageType === "captain" ? "#ffb11b" : 
                  messageType === "visitor" ? "#6a5acd" : "#e8a798"
    
    Rectangle {
        width: 3
        height: parent.height
        anchors.left: parent.left
        color: messageType === "captain" ? "#ffb11b" : 
               messageType === "visitor" ? "#6a5acd" : "#e8a798"
    }
    
    Column {
        id: contentColumn
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        anchors.margins: 12
        anchors.leftMargin: 20
        spacing: 6
        
        // 用户名行
        Row {
            spacing: 8
            
            Text {
                text: root.userName
                font.pixelSize: 14
                font.weight: Font.Medium
                color: messageType === "captain" ? "#ffb11b" : 
                       messageType === "visitor" ? "#6a5acd" : "#e8a798"
            }
            
            Rectangle {
                visible: messageType !== "normal"
                width: badgeText.width + 12
                height: 20
                radius: 4
                color: messageType === "captain" ? "#fff3e0" : "#f3f0ff"
                
                Text {
                    id: badgeText
                    anchors.centerIn: parent
                    text: messageType === "captain" ? "舰长" : "串门"
                    font.pixelSize: 10
                    color: messageType === "captain" ? "#ffb11b" : "#6a5acd"
                }
            }
        }
        
        // 消息文本
        Text {
            width: parent.width
            text: root.messageText
            font.pixelSize: 13
            color: "#666666"
            wrapMode: Text.Wrap
        }
        
        // 时间
        Text {
            anchors.right: parent.right
            text: root.messageTime
            font.pixelSize: 11
            color: "#999999"
        }
    }
    
    // 鼠标悬停效果
    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        
        onEntered: {
            parent.color = "#f5f2f1"
        }
        
        onExited: {
            parent.color = "#faf8f7"
        }
    }
    
    Behavior on color {
        ColorAnimation { duration: 150 }
    }
}
