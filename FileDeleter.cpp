#include "FileDeleter.h"

#include <QDebug>
#include <QFile>
#include <QFileInfo>

#include <windows.h>
#include <shellapi.h>

bool moveFileToTrash(const QString &filePath)
{
    QString tmp = filePath;

    WCHAR from[MAX_PATH];
    memset(from, 0, sizeof(from));
    tmp.replace('/', '\\').toWCharArray(from);

    SHFILEOPSTRUCT fileOp;
    memset(&fileOp, 0, sizeof(fileOp));
    fileOp.wFunc = FO_DELETE;
    fileOp.pFrom = from;
    fileOp.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_SILENT;

    int result = SHFileOperation(&fileOp);
    return result == 0;
}

QString replaceFileExtension(const QString &filePath, const QString &newExtension)
{
    QFileInfo fileInfo(filePath);

    // 确保新扩展名不以点开头
    QString cleanExtension = newExtension.startsWith(".")
                                 ? newExtension.mid(1)
                                 : newExtension;

    return fileInfo.path() + "/" + fileInfo.completeBaseName() + "." + cleanExtension;
}

FileDeleter::FileDeleter(QObject *parent)
    : QObject{parent}
{}

Q_INVOKABLE QStringList FileDeleter::deleteFiles(const QStringList& filePaths, bool deleteRaw)
{
    QStringList failedFiles;
    for (const QString& path : filePaths) {
        if (!moveFileToTrash(path)) {
            failedFiles.append(path);
        }
        if (deleteRaw) {
            // 获取RAW文件地址
            QString rawFilePath = replaceFileExtension(path, "ARW");
            if (!moveFileToTrash(rawFilePath)) {
                failedFiles.append(rawFilePath);
            }
        }
    }
    return failedFiles;
}
