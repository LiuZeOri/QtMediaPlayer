#include "playlistmodel.h"
#include "playlistitem.h"


#include <QUrl>
#include <QCollator>
#include <QDirIterator>
#include <QFileInfo>
#include <QProcess>
#include <QMimeData>
#include <QMimeDatabase>
#include <QDebug>
#include <QVariant>

PlayListmodel::PlayListmodel(QObject *parent): QAbstractListModel(parent)
{
}

int PlayListmodel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return 0;

    return m_playlist.size();
}

QHash<int, QByteArray> PlayListmodel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[TitleRole] = "title";
    roles[PathRole] = "path";
    roles[FolderPathRole] = "folderPath";
    roles[DurationRole] = "duration";
    return roles;
}

QVariant PlayListmodel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || m_playlist.empty())
        return QVariant("");

    auto playListItem = m_playlist.at(index.row());
    switch (role) {
    case TitleRole:
        return QVariant(playListItem->fileName());
    case PathRole:
        return QVariant(playListItem->filePath());
    case DurationRole:
        return QVariant(playListItem->duration());
    case FolderPathRole:
        return QVariant(playListItem->folderPath());
    }

    return QVariant("");
}

void PlayListmodel::clear()
{
    qDeleteAll(m_playlist);
    beginResetModel();
    m_playlist.clear();
    endResetModel();
}

void PlayListmodel::getSiblingItems(QUrl url)
{
    clear();
    QFileInfo openedFileInfo(url.toLocalFile());
//    qDebug()<<url.toLocalFile();
    if(openedFileInfo.exists() && openedFileInfo.isFile()){
        //创建兄弟lists,迭代器,mimeType预备函数
        QStringList siblingFiles;
        QDirIterator it(openedFileInfo.absolutePath(), QDir::Files, QDirIterator::NoIteratorFlags);
        while (it.hasNext()){

            //查询下一个
            QString siblingFile = it.next();
            qDebug()<<siblingFile;

            //查询下一个是否为音频文件
            QFileInfo siblingFileInfo(siblingFile);
            qDebug()<<siblingFile;
            auto siblingUrl = QUrl::fromLocalFile(siblingFile);
            QMimeType mimeType = mimeTypeCheck(siblingUrl);
            if(!siblingFileInfo.exists()){
                continue;
            }

            if(mimeType.name().startsWith("video/")) {
                siblingFiles.append(siblingFileInfo.absoluteFilePath());
            }
        }
        //排序
        QCollator collator;
        collator.setNumericMode(true);
        std::sort(siblingFiles.begin(), siblingFiles.end(), collator);

        //依次加入listitem中
        beginInsertRows(QModelIndex(), 0, siblingFiles.count() - 1);
        for (int i = 0; i < siblingFiles.count(); ++i) {
            qDebug()<<i;
            auto item = new PlayListItem(siblingFiles.at(i), this);
            qDebug()<<siblingFiles.at(i);
            m_playlist.append(item);
        }
        endInsertRows();
    }
}

QString PlayListmodel::getPath(int index)
{
    // ensure the requested path is valid
    if (m_playlist.isEmpty()) {
        return QString();
    }
    if (m_playlist.size() <= index) {
        return m_playlist[0]->filePath();
    }
    return m_playlist[index]->filePath();
}

void PlayListmodel::removeItem(int index)
{
    beginRemoveRows(QModelIndex(), index, index);
    m_playlist.removeAt(index);
    endRemoveRows();
    Q_EMIT itemRemoved(index, getPath(index));
}

void PlayListmodel::appendItem(QUrl url)
{
    //解析文件
    PlayListItem *item{nullptr};
    qsizetype row{m_playlist.count()};
    QFileInfo openedFileInfo(url.toLocalFile());

    //判断文件类型
    if (openedFileInfo.exists() && openedFileInfo.isFile()){
        QMimeType mimeType = mimeTypeCheck(url);
        if(mimeType.name().startsWith("video/")){
            item = new PlayListItem(openedFileInfo.absoluteFilePath(), this);
        }
    }
//    qDebug()<<openedFileInfo;

    //判断是否其中有内容
    if (item == nullptr) {
        return;
    }

    beginInsertRows(QModelIndex(), m_playlist.count(), m_playlist.count());
    m_playlist.append(item);
    Q_EMIT itemAdded(row, item->filePath());
    endInsertRows();

//    qDebug()<<m_playlist;

}

QMimeType PlayListmodel::mimeTypeCheck(const QUrl url){

    QMimeDatabase db;
    QMimeType mimeType = db.mimeTypeForUrl(url);

    return mimeType;

}


void PlayListmodel::openFile(const QString &path){
    QUrl qurl(path);
    QMimeType mimeType = mimeTypeCheck(qurl);
    if(mimeType.name().startsWith("video/")){
        if(m_isSibling){
            clear();
            getSiblingItems(qurl);
        }else{
            appendItem(qurl);
        }
    }
}



bool  PlayListmodel::getIsSibling() const{
    return m_isSibling;
}

void PlayListmodel::setIsSibling(bool is)
{
    m_isSibling = is;
    Q_EMIT isSiblingChanged();
}


#include "moc_playlistmodel.cpp"
