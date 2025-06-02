#include "DanmakuModel.h"

#include <QDateTime>
#include <QRandomGenerator>

DanmakuModel* DanmakuModel::m_instance = nullptr;

DanmakuModel* DanmakuModel::instance()
{
    if (!m_instance)
    {
        m_instance = new DanmakuModel();
    }
    return m_instance;
}

DanmakuModel* DanmakuModel::create(QQmlEngine* qmlEngine, QJSEngine* jsEngine)
{
    Q_UNUSED(qmlEngine)
    Q_UNUSED(jsEngine)
    return instance();
}

DanmakuModel::DanmakuModel(QObject* parent)
    : QAbstractListModel(parent)
    , m_simulationTimer(new QTimer(this))
    , m_nextId(1)
{
    // 初始化模拟数据
    m_simulatedUsers << "小可爱" << "技术宅" << "旅行家" << "舰长_星空" << "美食家"
                     << "游戏达人" << "舰长_明月" << "音乐爱好者" << "PK对手粉丝" << "老观众"
                     << "舰长_星河" << "新观众" << "设计达人" << "舰长_星云" << "科技控";

    m_simulatedTexts << "主播晚上好呀~" << "这个功能怎么实现的？" << "刚从西藏回来，风景太美了！"
                     << "今天准时报道！" << "主播吃过晚饭了吗？" << "这波操作太秀了！"
                     << "新舰长报道，支持主播！" << "主播能放点背景音乐吗？"
                     << "从隔壁直播间过来看看"
                     << "今天直播内容很精彩！" << "舰长续费成功！" << "刚来，这是什么直播间？"
                     << "这个UI设计好漂亮！" << "支持主播，加油！"
                     << "这个弹幕机器人是用什么开发的？";

    m_simulatedTypes << "normal" << "normal" << "normal" << "captain" << "normal"
                     << "normal" << "captain" << "normal" << "visitor" << "normal"
                     << "captain" << "normal" << "normal" << "captain" << "normal";

    connect(m_simulationTimer, &QTimer::timeout, this, &DanmakuModel::generateSimulatedMessage);
}

int DanmakuModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent)
    return m_messages.count();
}

QVariant DanmakuModel::data(const QModelIndex& index, int role) const
{
    if (index.row() < 0 || index.row() >= m_messages.count())
        return QVariant();

    const DanmakuMessage& message = m_messages[index.row()];

    switch (role)
    {
    case UserRole:
        return message.user;
    case TextRole:
        return message.text;
    case TimeRole:
        return message.time;
    case TypeRole:
        return message.type;
    case IdRole:
        return message.id;
    }

    return QVariant();
}

QHash<int, QByteArray> DanmakuModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[UserRole] = "user";
    roles[TextRole] = "text";
    roles[TimeRole] = "time";
    roles[TypeRole] = "type";
    roles[IdRole]   = "id";
    return roles;
}

void DanmakuModel::addMessage(const QString& user, const QString& text, const QString& type)
{
    beginInsertRows(QModelIndex(), 0, 0);

    DanmakuMessage message;
    message.user = user;
    message.text = text;
    message.time = QDateTime::currentDateTime().toString("hh:mm");
    message.type = type;
    message.id   = m_nextId++;

    m_messages.prepend(message);

    // 限制消息数量，保持最新的100条
    if (m_messages.count() > 100)
    {
        beginRemoveRows(QModelIndex(), 100, m_messages.count() - 1);
        m_messages = m_messages.mid(0, 100);
        endRemoveRows();
    }

    endInsertRows();

    emit messageAdded(user, text, type);
}

void DanmakuModel::clearMessages()
{
    beginResetModel();
    m_messages.clear();
    endResetModel();

    emit messagesCleared();
}

void DanmakuModel::pauseTTS()
{
    // TODO: 实现TTS暂停逻辑
    qDebug() << "TTS paused";
}

void DanmakuModel::resumeTTS()
{
    // TODO: 实现TTS恢复逻辑
    qDebug() << "TTS resumed";
}

void DanmakuModel::startSimulation()
{
    // 开始模拟弹幕，每1-3秒添加一条消息
    m_simulationTimer->start(QRandomGenerator::global()->bounded(1000, 3000));
}

void DanmakuModel::generateSimulatedMessage()
{
    int index = QRandomGenerator::global()->bounded(m_simulatedUsers.size());

    addMessage(m_simulatedUsers[index], m_simulatedTexts[index], m_simulatedTypes[index]);

    // 设置下次生成的时间间隔
    m_simulationTimer->start(QRandomGenerator::global()->bounded(500, 2500));
}
