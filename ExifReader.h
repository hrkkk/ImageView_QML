#pragma once

#include <QObject>

#include <exiv2/exiv2.hpp>

class ExifReader : public QObject
{
    Q_OBJECT
public:
    explicit ExifReader(QObject *parent = nullptr);

signals:
};
