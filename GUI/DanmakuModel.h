#ifndef DANMAKUMODEL_H
#define DANMAKUMODEL_H

#include <QAbstractListModel>
#include <QQmlEngine>
#include <QTimer>
#include <QtQmlIntegration>

struct DanmakuMessage
{
    QString user;
    QString text;
    QString time;
    QString type;   // normal, captain, visitor
    int     id;
};

class DanmakuModel : public QAbstractListModel
{
    Q_OBJECT
    QML_ELEMENT
    QML_SINGLETON

public:
    enum Roles
    {
        UserRole = Qt::UserRole + 1,
        TextRole,
        TimeRole,
        TypeRole,
        IdRole
    };

private:
    explicit DanmakuModel(QObject* parent = nullptr);

public:
    static DanmakuModel* instance();
    static DanmakuModel* create(QQmlEngine* qmlEngine, QJSEngine* jsEngine);

    // QAbstractListModel interface
    int rowCount(const QModelIndex& parent = QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;

    QHash<int, QByteArray> roleNames() const override;

public slots:
    Q_INVOKABLE void addMessage(const QString& user, const QString& text,
                                const QString& type = "normal");

    Q_INVOKABLE void clearMessages();

    Q_INVOKABLE void pauseTTS();

    Q_INVOKABLE void resumeTTS();

    Q_INVOKABLE void startSimulation();   // 模拟弹幕数据

signals:
    void messageAdded(const QString& user, const QString& text, const QString& type);

    void messagesCleared();

private slots:
    void generateSimulatedMessage();

private:
    static DanmakuModel* m_instance;

    QList<DanmakuMessage> m_messages;
    QTimer*               m_simulationTimer;
    int                   m_nextId;

    // 模拟数据
    QStringList m_simulatedUsers;
    QStringList m_simulatedTexts;
    QStringList m_simulatedTypes;
};

#endif   // DANMAKUMODEL_H
