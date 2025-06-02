import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import BilibiliBot 1.0
import "."

Rectangle {
    id: root
    color: "#f8f5f2"
    
    RowLayout {
        anchors.fill: parent
        spacing: 0
        
        // 侧边栏
        Sidebar {
            id: sidebar
            Layout.preferredWidth: 70
            Layout.fillHeight: true
            
            onPageChanged: function(page) {
                contentLoader.source = page + "Page.qml"
            }
        }
        
        // 主内容区域
        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "transparent"
            
            Loader {
                id: contentLoader
                anchors.fill: parent
                anchors.margins: 20
                source: "DanmakuPage.qml"
            }
        }
    }
}
