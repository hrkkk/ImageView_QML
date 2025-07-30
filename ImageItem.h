#pragma once

#include <QObject>
#include <QString>

class ImageItem : public QObject
{
	Q_GADGET
	Q_PROPERTY(QString filepath READ GetFilepath)
	Q_PROPERTY(int flag READ GetFlag WRITE SetFlag)
public:
	ImageItem() = default;

	ImageItem(const QString& filepath);

	ImageItem(const ImageItem& o) { m_filepath = o.m_filepath; m_flag = o.m_flag; }

	ImageItem& operator=(const ImageItem& o) { m_filepath = o.m_filepath; m_flag = o.m_flag; return *this; }

	QString GetFilepath() const { return m_filepath; }

	int GetFlag() const { return m_flag; }

	void SetFlag(int flag) { m_flag = flag; }

private:
	QString m_filepath;
	int m_flag;
};

Q_DECLARE_METATYPE(ImageItem)