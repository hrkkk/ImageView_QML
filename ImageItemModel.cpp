#include "ImageItemModel.h"

#include <QUrl>

ImageItemModel::ImageItemModel(QObject* parent) :
	QAbstractListModel(parent)
{
}

int ImageItemModel::rowCount(const QModelIndex& parent) const
{
	return m_items.size();
}

QVariant ImageItemModel::data(const QModelIndex& index, int role) const
{
	if (!index.isValid() || index.row() >= m_items.size()) {
		return QVariant();
	}

	ImageItem item = m_items[index.row()];
	switch (role) {
		case NameRole: return item.filepath;
		case FlagRole: return item.flag;
		default: return QVariant();
	}
}

QHash<int, QByteArray> ImageItemModel::roleNames() const
{
	return {
		{ NameRole, "filepath" },
		{ FlagRole, "flag" }
	};
}

void ImageItemModel::UpdateFromFolderModel(const QVariantList& folderListModel)
{
	beginResetModel();
	m_items.clear();

	for (const QVariant& item : folderListModel) {
		QVariantMap map = item.toMap();
		QString filepath = QUrl::fromLocalFile(map["filePath"].toString()).toString();
		m_items.append(ImageItem(filepath));
	}

	endResetModel();
}

void ImageItemModel::SetItemFlag(const QString& filepath, uint8_t flag)
{
	auto iter = std::find_if(m_items.begin(), m_items.end(), [=](const ImageItem& item) {
		return item.filepath == filepath;
	});

	if (iter != m_items.end()) {
		iter->flag = flag;
	}

	// 通知视图数据已更改
	QModelIndex modelIndex = createIndex((iter - m_items.begin()), 0);
	emit dataChanged(modelIndex, modelIndex, {FlagRole});
}

int ImageItemModel::GetItemFlag(const QString& filepath) const
{
	auto iter = std::find_if(m_items.begin(), m_items.end(), [&](const ImageItem& item) {
		return item.filepath == filepath;
	});

	if (iter != m_items.end()) {
		return iter->flag;
	}

	return 0;
}

int ImageItemModel::GetItemFlag(int index) const
{
	if (index < 0 || index >= m_items.size()) {
		return 0;
	}

	return m_items[index].flag;
}

QString ImageItemModel::GetFilePath(int index) const
{
	if (index < 0 || index >= m_items.size()) {
		return "";
	}

	return m_items[index].filepath;
}