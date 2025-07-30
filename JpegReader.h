#ifndef JPEGREADER_H
#define JPEGREADER_H

#include <QObject>
#include <QQuickAsyncImageProvider>
#include <QQuickImageResponse>
#include <QRunnable>
#include <QMutex>
#include <QThreadPool>

#include "ImageReader.h"
#include "Utils.h"

class JpegReader : public ImageReader
{
public:
    JpegReader();

    virtual Result Decode() override;

private:
    Result ParseExif(
        const std::string&                                  filename,
        std::vector<std::pair<std::string, std::string>>&   exif,
        Orientation&                                        orientation);

    Result DecodeImage(
        const std::string&                                  filepath,
        std::shared_ptr<ImageData>&                         imageData,
        std::vector<std::pair<std::string, std::string>>&   exif);
};

class JpegImageResponse : public QQuickImageResponse, public QRunnable
{
public:
    JpegImageResponse(const QString& filepath);
    ~JpegImageResponse();

    QQuickTextureFactory* textureFactory() const override;
    QString errorString() const override;

    void run() override;

private:
    QString m_filepath;
    QSize m_requestedSize;
    QImage m_image;
    QString m_error;
    mutable QMutex m_mutex;
};

class JpegImageProvider : public QQuickAsyncImageProvider
{
public:
    QQuickImageResponse* requestImageResponse(const QString& id, const QSize& requestedSize) override;
};

#endif // JPEGREADER_H
