#ifndef FILEDELETER_H
#define FILEDELETER_H

#include <QObject>

class FileDeleter : public QObject
{
    Q_OBJECT
public:
    explicit FileDeleter(QObject *parent = nullptr);

    Q_INVOKABLE QStringList deleteFiles(const QStringList& filePaths, bool deleteRaw);

signals:
};

#endif // FILEDELETER_H
