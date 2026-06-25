#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include "ViewModel/CommViewModel.h"
#include "ViewModel/DataPlotViewModel.h"
#include "ViewModel/MainViewModel.h"
#include "ViewModel/AppSettingsViewModel.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQuickStyle::setStyle("Fusion");

    qmlRegisterType<CommViewModel>("CommTool.ViewModel", 1, 0, "CommViewModel");
    qmlRegisterType<DataPlotViewModel>("CommTool.ViewModel", 1, 0, "DataPlotViewModel");
    qmlRegisterType<MainViewModel>("CommTool.ViewModel", 1, 0, "MainViewModel");
    qmlRegisterType<AppSettingsViewModel>("CommTool.ViewModel", 1, 0, "AppSettingsViewModel");

    QQmlApplicationEngine engine;
    const QUrl url(u"qrc:/CommTool/View/MainView.qml"_qs);
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}