import QtQuick 2.15
import QtQuick.Controls 2.15

Rectangle {
    id: root
    
    property bool checked: false
    property alias enabled: mouseArea.enabled
    
    width: 50
    height: 24
    radius: 12
    color: checked ? "#e8a798" : "#cccccc"
    
    Behavior on color {
        ColorAnimation { duration: 200 }
    }
    
    Rectangle {
        id: handle
        width: 18
        height: 18
        radius: 9
        anchors.verticalCenter: parent.verticalCenter
        x: root.checked ? parent.width - width - 3 : 3
        color: "#ffffff"
        
        Behavior on x {
            NumberAnimation { duration: 200; easing.type: Easing.InOutQuad }
        }
    }
    
    MouseArea {
        id: mouseArea
        anchors.fill: parent
        onClicked: {
            root.checked = !root.checked
        }
    }
}
