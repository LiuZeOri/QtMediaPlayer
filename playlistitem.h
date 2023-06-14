#ifndef PLAYLISTITEM_H
#define PLAYLISTITEM_H

#include <QObject>

class PlayListItem: public QObject
{
    Q_OBJECT
public:
    explicit PlayListItem(const QString &path, QObject *parent = nullptr);


    Q_INVOKABLE QString filePath() const;
    void setFilePath(const QString &filePath);

    Q_INVOKABLE QString fileName() const;
    void setFileName(const QString &fileName);

    Q_INVOKABLE QString folderPath() const;
    void setFolderPath(const QString &folderPath);

    Q_INVOKABLE QString duration() const;
    void setDuration(const QString &duration);

private:
    QString m_fileName;
    QString m_filePath;
    QString m_folderPath;
    QString m_duration;
};

#endif // PLAYLISTITEM_H
