import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Rectangle {
    id: root
    width: 70
    color: "#ffffff"
    
    signal pageChanged(string page)
    
    property string currentPage: "Danmaku"
    
    // 使用纯QML阴影效果替代DropShadow
    Rectangle {
        anchors.fill: parent
        anchors.leftMargin: 2
        color: "#10000000"
        radius: parent.radius
        opacity: 0.1
        z: -1
    }
    
    Column {
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
        anchors.topMargin: 20
        spacing: 0
        
        Repeater {
            model: [
                { id: "Danmaku", icon: "💬", text: "实时弹幕" },
                { id: "LoginInfo", icon: "👤", text: "登录信息" },
                // { id: "Stats", icon: "📊", text: "数据统计" },
                { id: "Welcome", icon: "👋", text: "弹幕欢迎" },
                { id: "Gift", icon: "🎁", text: "礼物答谢" },
                { id: "Interaction", icon: "💬", text: "互动回复" },
                { id: "Blindbox", icon: "📦", text: "盲盒统计" },
                { id: "PK", icon: "👊", text: "PK 相关" },
                { id: "Settings", icon: "⚙️", text: "设置" }
            ]
            
            Rectangle {
                width: 70
                height: 70
                color: "transparent"
                
                Rectangle {
                    width: 3
                    height: parent.height
                    anchors.left: parent.left
                    color: "#e8a798"
                    visible: root.currentPage === modelData.id
                }
                
                Column {
                    anchors.centerIn: parent
                    spacing: 4
                    
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: modelData.icon
                        font.pixelSize: 20
                    }
                    
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        text: modelData.text
                        font.pixelSize: 9
                        color: "#333333"
                        horizontalAlignment: Text.AlignHCenter
                    }
                }
                
                MouseArea {
                    anchors.fill: parent
                    hoverEnabled: true
                    
                    // onEntered: {
                    //     if (root.currentPage !== modelData.id) {
                    //         parent.color = "#f5f5f5"
                    //     }
                    // }
                    //
                    // onExited: {
                    //     if (root.currentPage !== modelData.id) {
                    //         parent.color = "transparent"
                    //     }
                    // }
                    
                    onClicked: {
                        root.currentPage = modelData.id
                        root.pageChanged(modelData.id)
                        // parent.color = "#f0e6e4"
                    }
                }
                
                Behavior on color {
                    ColorAnimation { duration: 200 }
                }
            }
        }
    }
}
