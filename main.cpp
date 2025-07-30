#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QIcon>

#include "FileDeleter.h"
#include "JpegReader.h"
#include "ImageItemModel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    // 设置应用图标
    app.setWindowIcon(QIcon("C:/Users/hrkkk/Desktop/tmp/ImageViewerQML/icon.png"));

    // 创建解码线程池（最大并发4线程）
    QThreadPool::globalInstance()->setMaxThreadCount(4);

    qmlRegisterType<FileDeleter>("com.example", 1, 0, "FileDeleter");
    qmlRegisterType<ImageItemModel>("com.example", 1, 0, "ImageItemModel");

    QQmlApplicationEngine engine;

    engine.addImageProvider("fastjpeg", new JpegImageProvider());

    //const QUrl url(QStringLiteral("qrc:/ImageViewerQML/Main.qml"));
    const QUrl url(QUrl::fromLocalFile("C:/Users/hrkkk/Desktop/tmp/ImageViewerQML/Main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
