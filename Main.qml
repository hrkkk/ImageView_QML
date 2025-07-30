import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import Qt.labs.folderlistmodel 2.15
import QtQuick.Dialogs

import com.example 1.0

ApplicationWindow {
    id: root
    title: "图片浏览器"
    width: 800
    height: 600
    visible: true
    visibility: Window.Maximized

    property string currentImagePath: ""
    property int currentPresentMode: 0


    // 主界面布局
    ColumnLayout {
        anchors.fill: parent
        spacing: 5

        // 工具栏
        RowLayout {
            Layout.fillWidth: true
            spacing: 5

            Button {
                text: "打开目录"
                onClicked: folderDialog.open()
            }

            Button {
                text: "上一张"
                enabled: folderModel.count > 0
                onClicked: navigate(-1)
            }

            Button {
                text: "下一张"
                enabled: folderModel.count > 0
                onClicked: navigate(1)
            }

            Button {
                text: "旋转"
                onClicked: imageView.rotation += 90
            }

            Button {
                text: "切换视图"
                onClicked: {
                    if (currentPresentMode === 0) {
                        currentPresentMode = 1
                        currentImagePath = deleteListModel.get(0).fileUrl
                    } else if (currentPresentMode === 1) {
                        currentPresentMode = 0
                        currentImagePath = folderModel.get(0, "fileUrl")
                    }
                }
            }

            Button {
                text: "一键删除"
                onClicked: {
                    dialog.open()
                }
            }

            Button {
                text: "查看元数据"
                onClicked: {
                    slideBar.visible = !slideBar.visible
                }
            }
        }

        // 当前目录信息
        Label {
            Layout.fillWidth: true
            text: folderModel.folder ? "当前目录: " + folderModel.folder.toString().replace("file:///", "") : "未选择目录"
            elide: Text.ElideLeft
            padding: 5
            background: Rectangle { color: "#f0f0f0" }
        }

        Label {
            Layout.fillWidth: true
            text: "缩放比例：" + imageView.scale.toFixed(2) * 100 + "%"
            elide: Text.ElideLeft
            padding: 5
            background: Rectangle { color: "#f0f0f0" }
        }

        RowLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true

            // 图片显示区域
            Item {
                Layout.fillWidth: true
                Layout.fillHeight: true


                Image {
                    id: imageView
                    anchors.centerIn: parent
                    width: Math.min(implicitWidth, parent.width)
                    height: Math.min(implicitHeight, parent.height)
                    fillMode: Image.PreserveAspectFit
                    source: {
                        if (currentImagePath) {
                            return "image://fastjpeg/" + currentImagePath + "?async";
                        }
                        return "";
                    }
                    // autoTransform: true  // 自动根据EXIF信息变换
                    asynchronous: true
                    cache: true

                    MouseArea {
                        anchors.fill: parent
                        acceptedButtons: Qt.LeftButton | Qt.RightButton

                        onClicked: {
                            if (mouse.button === Qt.RightButton && currentImagePath !== "") {
                                ratingMenu.popup()
                            }
                        }

                        onDoubleClicked: {
                            imageView.scale = 1.0
                        }

                        onWheel: {
                            if (wheel.angleDelta.y > 10) {
                                imageView.scale += 0.1
                            } else if (wheel.angleDelta.y < -10) {
                                imageView.scale = Math.max(0.1, imageView.scale - 0.1)
                            }
                        }
                    }

                    // 在图片右上角显示当前星级
                    Rectangle {
                        anchors.right: parent.right
                        anchors.top: parent.top
                        anchors.margins: 10
                        width: 30
                        height: 30
                        radius: 15
                        color: "#AA000000"
                        visible: queryImageFlag(currentImagePath) > 0

                        Text {
                            anchors.centerIn: parent
                            text: queryImageFlag(currentImagePath) || ""
                            color: "white"
                            font.bold: true
                            font.pixelSize: 16
                        }
                    }

                    Behavior on scale {
                        NumberAnimation { duration: 200 }
                    }

                    Behavior on rotation {
                        NumberAnimation { duration: 200 }
                    }
                }

                // 加载指示器
                BusyIndicator {
                    anchors.centerIn: parent
                    running: imageView.status === Image.Loading
                }
            }

            Rectangle {
                id: slideBar
                width: 300
                Layout.fillHeight: true
                visible: false

                Text {
                    anchors.top: parent.top
                    wrapMode: Text.Wrap
                    font.bold: true
                    lineHeight: 5
                    text: "ISO: " + 100 + "\n" +
                          "光圈: " + 100 + "\n" +
                          "快门速度: " + 100 + "\n" +
                          "焦距: " + 100 + "\n" +
                          "创意外观: " + 100 + "\n" +
                          "曝光补偿: " + 100 + "\n" +
                          "拍摄日期: " + 100
                }
            }
        }

        // 缩略图栏
        Rectangle {
            Layout.fillWidth: true
            Layout.preferredHeight: 120
            anchors.bottom: parent.bottom
            color: "#e0e0e0"
            border.color: "#a0a0a0"

            ScrollView {
                id: scrollView
                anchors.fill: parent
                anchors.margins: 2
                // 禁用垂直滚动，强制横向滚动
                ScrollBar.vertical.policy: ScrollBar.AlwaysOff
                ScrollBar.horizontal.policy: ScrollBar.AsNeeded

                ListView {
                    id: thumbnailView
                    orientation: ListView.Horizontal
                    spacing: 5
                    model: imageItemModel

                    delegate: Rectangle {
                        width: 100
                        height: 100
                        color: "transparent"

                        Image {
                            id: thumbnail
                            anchors.fill: parent
                            anchors.margins: 2
                            source: {
                                return "image://fastjpeg/" + model.filepath + "?async"
                            }
                            fillMode: Image.PreserveAspectFit
                            autoTransform: true  // 自动根据EXIF信息变换
                            asynchronous: true
                            cache: false
                            sourceSize.width: 100
                            sourceSize.height: 100
                        }

                        // 选中状态指示
                        Rectangle {
                            anchors.fill: parent
                            color: "transparent"
                            border.color: currentImagePath === model.filepath ? "blue" : "transparent"
                            border.width: 2
                        }

                        Rectangle {
                            anchors.right: parent.right
                            anchors.top: parent.top
                            width: 20
                            height: 20
                            radius: 10
                            color: "#AA000000"
                            visible: model.flag  > 0

                            Text {
                                anchors.centerIn: parent
                                text: model.flag.toString() || ""
                                color: "white"
                                font.bold: true
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: {
                                if (mouse.button === Qt.LeftButton) {
                                    currentImagePath = model.filepath
                                    // 确保缩略图在视图中可见
                                    thumbnailView.positionViewAtIndex(index, ListView.Contain)
                                } else if (mouse.button === Qt.RightButton) {
                                    currentImagePath = model.filepath
                                    ratingMenu.popup()
                                }
                            }
                        }

                        // 加载指示器
                        BusyIndicator {
                            anchors.centerIn: parent
                            running: thumbnail.status === Image.Loading
                        }

                        // OpacityAnimator {
                        //     target: thumbnail
                        //     from: 0
                        //     to: 1
                        //     duration: 200
                        //     running: thumbnail.status === Image.Ready
                        // }
                    }
                }

                MouseArea {
                    anchors.fill: parent
                    propagateComposedEvents: true   // 允许事件继续传递
                    onWheel: {
                        if (wheel.angleDelta.y !== 0) {
                            // 计算目标位置
                            let newX = scrollView.ScrollBar.horizontal.position - wheel.angleDelta.y * 0.0005
                            newX = Math.max(0, Math.min(newX, 1 - scrollView.ScrollBar.horizontal.size))

                            // 平滑滚动到新位置
                            scrollView.ScrollBar.horizontal.position = newX

                            // 阻止事件继续传递（避免默认垂直滚动）
                            wheel.accepted = true
                        }
                    }
                }
            }
        }
    }

    // 目录对话框
    FolderDialog {
        id: folderDialog
        title: "选择图片目录"
        currentFolder: shortcuts.pictures
        onAccepted: {
            folderModel.folder = folderDialog.selectedFolder
        }
    }

    // 文件夹内容模型
    FolderListModel {
        id: folderModel
        showDirs: false
        nameFilters: ["*.jpg", "*.jpeg", "*.png", "*.bmp", "*.gif", "*.JPG", "*.JPEG", "*.PNG"]
        sortField: FolderListModel.Name
        onStatusChanged: {
            if (folderModel.status === FolderListModel.Ready) {
                // 拷贝给自定义的ImageItemModel
                var data = [];
                for (var i = 0; i < folderModel.count; ++i) {
                    data.push({
                        "filePath": folderModel.get(i, "filePath")
                    });
                }
                imageItemModel.UpdateFromFolderModel(data);
                
                if (imageItemModel.GetCount() > 0) {
                    currentImagePath = imageItemModel.GetFilePath(0)
                    console.log(currentImagePath)
                }
            }
        }
    }


    ImageItemModel {
        id: imageItemModel
    }

    // 星级标记菜单
    Menu {
        id: ratingMenu
        title: "标记图片"

        MenuItem {
            text: "标记为 喜欢"
            onTriggered: {
                imageItemModel.SetItemFlag(currentImagePath, 1)
            }
        }

        MenuItem {
            text: "标记为 删除"
            onTriggered: {
                imageItemModel.SetItemFlag(currentImagePath, 2)
            }
        }

        MenuSeparator {}

        MenuItem {
            text: "清除标记"
            onTriggered: {
                imageItemModel.SetItemFlag(currentImagePath, 0)
            }
        }
    }

    Dialog {
        id: dialog
        title: "提示"
        modal: true
        standardButtons: Dialog.Ok | Dialog.Cancel

        anchors.centerIn: parent
        width: Math.max(container.implicitWidth, 200)
        height: Math.max(container.implicitHeight, 150)

        Column {
            id: container
            anchors.centerIn: parent
            spacing: 10

            Label {
                text: "是否删除选中的 " + deleteListModel.count + " 张图像？"
            }

            CheckBox {
                id: checkBox
                text: "删除同名RAW文件"
                checked: true
            }
        }

        onAccepted: {
            var deleteRaw = false
            if (checkBox.checkState === Qt.Checked) {
                deleteRaw = true
            }

            console.log(deleteListModel.count)

            if (deleteListModel.count > 0) {
                var deleteFiles = []
                for (let i = 0; i < deleteListModel.count; i++) {
                    let filePath = deleteListModel.get(i).fileUrl.toString().replace(/^file:\/\/\//, "")
                    deleteFiles.push(filePath)
                }

                var failedFiles = fileDeleter.deleteFiles(deleteFiles, deleteRaw)

                // 剔除已经不存在的文件
                for (let i = deleteListModel.count - 1; i >= 0; i--) {
                    let isDeleted = true
                    let tmp = deleteListModel.get(i).fileUrl.toString().replace(/^file:\/\/\//, "")
                    for (var j = 0; j < failedFiles.length; j++) {
                        if (failedFiles[j] === tmp) {
                            isDeleted = false;
                        }
                    }
                    if (isDeleted) {
                        deleteListModel.remove(i);
                    }
                }
            }
            console.log(deleteListModel.count)
        }
    }

    FileDeleter {
        id: fileDeleter
    }

    // 导航图片
    function navigate(direction) {
        if (imageItemModel.count === 0) return

        var currentIndex = -1
        for (var i = 0; i < imageItemModel.count; i++) {
            if (imageItemModel.GetFilePath(i) === currentImagePath) {
                currentIndex = i
                break
            }
        }

        var newIndex = (currentIndex + direction + imageItemModel.count) % imageItemModel.count
        currentImagePath = imageItemModel.GetFilePath(newIndex)
        // 确保缩略图在视图中可见
        thumbnailView.positionViewAtIndex(newIndex, ListView.Contain)
    }

    // 切换幻灯片模式
    function toggleSlideshow() {
        if (slideshowTimer.running) { 
            slideshowTimer.stop()
        } else {
            if (folderModel.count > 0) {
                slideshowTimer.start()
            }
        }
    }

    function queryImageFlag(imageUrl) {
        return imageItemModel.GetItemFlag(imageUrl)
    }
}
