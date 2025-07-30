#pragma once

#include <QAbstractListModel>

struct ImageItem
{
	QString filepath;
	int flag;

	ImageItem(const QString& filepath) : filepath(filepath), flag(0) {}
};

class ImageItemModel : public QAbstractListModel
{
	Q_OBJECT
public:
	enum Roles { NameRole = Qt::UserRole + 1, FlagRole };

	explicit ImageItemModel(QObject* parent = nullptr);

	Q_INVOKABLE void UpdateFromFolderModel(const QVariantList& folderListModel);

	Q_INVOKABLE void SetItemFlag(const QString& filepath, uint8_t flag);

	Q_INVOKABLE int GetItemFlag(const QString& filepath) const;

	Q_INVOKABLE int GetItemFlag(int index) const;

	Q_INVOKABLE QString GetFilePath(int index) const;

	Q_INVOKABLE int GetCount() const { return m_items.size(); }

	Q_INVOKABLE QList<ImageItem> GetList() const { return m_items; }

protected:
	int rowCount(const QModelIndex& parent = QModelIndex()) const override;
	
	QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

	QHash<int, QByteArray> roleNames() const override;

private:
	QList<ImageItem> m_items;
};