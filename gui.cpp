#include "GUI/BilibiliApi.h"
#include "GUI/DanmakuModel.h"
#include "GUI/SettingsManager.h"
#include "GUI/TTSManager.h"

#include <QGuiApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QQuickStyle>

int main(int argc, char* argv[])
{
    QGuiApplication app(argc, argv);

    // 设置应用信息
    app.setApplicationName("BilibiliBot");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("BilibiliBot Team");
    app.setOrganizationDomain("bilibilibot.com");

    // // 设置应用图标
    // app.setWindowIcon(QIcon(":/resources/icons/app_icon.png"));

    // 设置QuickControls2样式
    QQuickStyle::setStyle("Material");

    QQmlApplicationEngine engine;
    // qmlRegisterSingletonType<BilibiliApi>("BilibiliApi", 1, 0, "BilibiliApi", BilibiliApi::instance());
    qmlRegisterSingletonType<DanmakuModel>("BilibiliBot", 1, 0, "DanmakuModel",
                                           [](QQmlEngine* engine, QJSEngine* scriptEngine) -> QObject*
                                           {
                                               Q_UNUSED(engine);
                                               Q_UNUSED(scriptEngine);

                                               // 返回单例对象的指针
                                               return DanmakuModel::instance();
                                           });
    qmlRegisterSingletonType<SettingsManager>("BilibiliBot", 1, 0, "SettingsManager",
                                              [](QQmlEngine* engine, QJSEngine* scriptEngine) -> QObject*
                                              {
                                                  Q_UNUSED(engine);
                                                  Q_UNUSED(scriptEngine);

                                                  // 返回单例对象的指针
                                                  return SettingsManager::instance();
                                              });
    qmlRegisterSingletonType<BilibiliApi>("BilibiliBot", 1, 0, "BilibiliApi",
                                          [](QQmlEngine* engine, QJSEngine* scriptEngine) -> QObject*
                                          {
                                              Q_UNUSED(engine);
                                              Q_UNUSED(scriptEngine);

                                              // 返回单例对象的指针
                                              return BilibiliApi::instance();
                                          });
    qmlRegisterSingletonType<TTSManager>("BilibiliBot", 1, 0, "TTSManager",
                                          [](QQmlEngine* engine, QJSEngine* scriptEngine) -> QObject*
                                          {
                                              Q_UNUSED(engine);
                                              Q_UNUSED(scriptEngine);

                                              // 返回单例对象的指针
                                              return TTSManager::instance();
                                          });
    // 加载QML主界面
    const QUrl url(QStringLiteral("qrc:/qt/qml/BilibiliBot/qml/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject* obj, const QUrl& objUrl)
                     {
                         if (!obj && url == objUrl)
                             QCoreApplication::exit(-1);
                     }, Qt::QueuedConnection);

    engine.load(url);

    return app.exec();
}
