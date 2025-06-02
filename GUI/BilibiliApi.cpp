#include "BilibiliApi.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QRandomGenerator>
#include <QUrlQuery>

BilibiliApi* BilibiliApi::m_instance = nullptr;

BilibiliApi* BilibiliApi::instance()
{
    if (!m_instance)
    {
        m_instance = new BilibiliApi();
    }
    return m_instance;
}

BilibiliApi* BilibiliApi::create(QQmlEngine* qmlEngine, QJSEngine* jsEngine)
{
    Q_UNUSED(qmlEngine)
    Q_UNUSED(jsEngine)
    return instance();
}

BilibiliApi::BilibiliApi(QObject* parent)
    : QObject(parent)
    , m_networkManager(new QNetworkAccessManager(this))
    , m_qrCheckTimer(new QTimer(this))
    , m_eventSimulationTimer(new QTimer(this))
    , m_isLoggedIn(false)
    , m_isConnected(false)
    , m_qrCodeUrl("")
    , m_loginStatus("等待扫码中...")
    , m_currentRoomId("")
{
    // connect(m_qrCheckTimer, &QTimer::timeout, this, &BilibiliApi::checkQRCodeStatus);
    // connect(m_eventSimulationTimer, &QTimer::timeout, this, &BilibiliApi::startEventSimulation);
}

void BilibiliApi::setCurrentRoomId(const QString& roomId)
{
    if (m_currentRoomId != roomId)
    {
        m_currentRoomId = roomId;
        // emit currentRoomIdChanged();
    }
}

void BilibiliApi::generateQRCode()
{
    // TODO: 实现真实的二维码生成逻辑
    // 这里模拟生成二维码URL
    m_qrCodeUrl   = "https://passport.bilibili.com/qrcode/h5/login?oauthKey=example_key";
    m_loginStatus = "等待扫码中...";

    // emit qrCodeUrlChanged();
    // emit qrCodeGenerated(m_qrCodeUrl);

    // 开始检查登录状态
    m_qrCheckTimer->start(2000);   // 每2秒检查一次
}

void BilibiliApi::checkLoginStatus()
{
    // TODO: 实现真实的登录状态检查
    // 这里只是更新状态文本
    static int checkCount = 0;
    checkCount++;

    if (checkCount % 3 == 0)
    {
        m_loginStatus = "扫码成功，等待确认...";
    }
    else
    {
        m_loginStatus = "等待扫码中...";
    }

    emit loginStatusChanged();
}

void BilibiliApi::checkQRCodeStatus()
{
    // TODO: 实现真实的二维码状态检查
    checkLoginStatus();
}

void BilibiliApi::logout()
{
    m_isLoggedIn   = false;
    m_isConnected  = false;
    m_accessToken  = "";
    m_refreshToken = "";
    m_qrCheckTimer->stop();
    m_eventSimulationTimer->stop();

    emit loginStatusChanged();
    emit connectionStatusChanged();
}

void BilibiliApi::simulateLogin()
{
    m_isLoggedIn  = true;
    m_loginStatus = "登录成功";
    m_accessToken = "simulated_access_token";
    m_qrCheckTimer->stop();

    emit loginStatusChanged();
    emit loginSuccess();
}

void BilibiliApi::connectToRoom(const QString& roomId)
{
    if (!m_isLoggedIn)
    {
        emit connectionError("请先登录");
        return;
    }

    setCurrentRoomId(roomId);

    // TODO: 实现真实的直播间连接逻辑
    // 模拟连接成功
    m_isConnected = true;
    emit connectionStatusChanged();
    emit roomConnected();

    // 开始模拟事件
    startEventSimulation();
}

void BilibiliApi::disconnectFromRoom()
{
    m_isConnected = false;
    m_eventSimulationTimer->stop();

    emit connectionStatusChanged();
    emit roomDisconnected();
}

void BilibiliApi::getRoomInfo()
{
    // TODO: 实现真实的房间信息获取
    QVariantMap roomInfo;
    roomInfo["title"]  = "测试直播间";
    roomInfo["online"] = QRandomGenerator::global()->bounded(100, 10000);
    roomInfo["fans"]   = QRandomGenerator::global()->bounded(1000, 100000);

    emit roomInfoReceived(roomInfo);
}

void BilibiliApi::getUserInfo()
{
    // TODO: 实现真实的用户信息获取
    QVariantMap userInfo;
    userInfo["name"]      = "测试用户";
    userInfo["level"]     = QRandomGenerator::global()->bounded(1, 60);
    userInfo["followers"] = QRandomGenerator::global()->bounded(100, 50000);

    emit userInfoReceived(userInfo);
}

void BilibiliApi::onNetworkReply()
{
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply)
        return;

    // TODO: 处理网络响应
    QByteArray    data = reply->readAll();
    QJsonDocument doc  = QJsonDocument::fromJson(data);

    reply->deleteLater();
}

void BilibiliApi::startEventSimulation()
{
    if (!m_isConnected)
        return;

    // 模拟各种直播间事件
    int eventType = QRandomGenerator::global()->bounded(0, 4);

    QStringList users    = {"小可爱", "技术宅", "旅行家", "舰长_星空", "美食家", "游戏达人"};
    QStringList messages = {"666", "主播好厉害", "学到了", "支持支持", "太强了", "厉害厉害"};
    QStringList gifts    = {"小心心", "辣条", "牛哇", "冰阔落", "小花花"};
    QStringList types    = {"normal", "normal", "normal", "captain", "normal"};

    QString user = users[QRandomGenerator::global()->bounded(users.size())];

    switch (eventType)
    {
    case 0:   // 弹幕
    {
        QString message = messages[QRandomGenerator::global()->bounded(messages.size())];
        QString type    = types[QRandomGenerator::global()->bounded(types.size())];
        emit    danmakuReceived(user, message, type);
    }
    break;
    case 1:   // 礼物
    {
        QString gift  = gifts[QRandomGenerator::global()->bounded(gifts.size())];
        int     count = QRandomGenerator::global()->bounded(1, 10);
        emit    giftReceived(user, gift, count);
    }
    break;
    case 2:   // 关注
        emit followReceived(user);
        break;
    case 3:   // 分享
        emit shareReceived(user);
        break;
    }

    // 设置下次事件时间
    int nextInterval = QRandomGenerator::global()->bounded(1000, 5000);
    m_eventSimulationTimer->start(nextInterval);
}
