import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: root
    
    property string title: ""
    property string value: "0"
    property string subtitle: ""
    property color valueColor: "#e8a798"
    
    width: 120
    height: 100
    radius: 8
    color: "#ffffff"
    
    Column {
        anchors.centerIn: parent
        spacing: 8
        
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: root.value
            font.pixelSize: 28
            font.weight: Font.Bold
            color: root.valueColor
        }
        
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: root.title
            font.pixelSize: 12
            color: "#666666"
            horizontalAlignment: Text.AlignHCenter
        }
        
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            text: root.subtitle
            font.pixelSize: 10
            color: "#999999"
            visible: root.subtitle !== ""
            horizontalAlignment: Text.AlignHCenter
        }
    }
    
    // 悬停效果
    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        
        onEntered: {
            parent.y -= 3
            shadowEffect.visible = true
        }
        
        onExited: {
            parent.y += 3
            shadowEffect.visible = false
        }
    }
    
    // 阴影效果
    Rectangle {
        id: shadowEffect
        anchors.fill: parent
        anchors.topMargin: 5
        radius: parent.radius
        color: "#10000000"
        z: -1
        visible: false
    }
    
    Behavior on y {
        NumberAnimation { duration: 150 }
    }
}
