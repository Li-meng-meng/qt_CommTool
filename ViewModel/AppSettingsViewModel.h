#ifndef APPSETTINGSVIEWMODEL_H
#define APPSETTINGSVIEWMODEL_H

#include <QObject>
#include <QQmlEngine>

class AppSettingsViewModel : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(bool isDarkMode READ getIsDarkMode WRITE setIsDarkMode NOTIFY isDarkModeChanged)

public:
    explicit AppSettingsViewModel(QObject *parent = nullptr);
    ~AppSettingsViewModel() override;

    bool getIsDarkMode() const;
    void setIsDarkMode(bool dark);

signals:
    void isDarkModeChanged(bool dark);

private:
    bool m_isDarkMode;
};

#endif // APPSETTINGSVIEWMODEL_H