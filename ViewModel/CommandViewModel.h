#ifndef COMMANDVIEWMODEL_H
#define COMMANDVIEWMODEL_H

#include <QObject>
#include <QVariantList>
#include <QQmlEngine>
#include "../Help/DataParser.h"

class CommandViewModel : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(QVariantList commandGroups READ commandGroups CONSTANT)
    Q_PROPERTY(QVariantMap selectedCommand READ selectedCommand WRITE setSelectedCommand NOTIFY selectedCommandChanged)
    Q_PROPERTY(QString generatedHex READ generatedHex NOTIFY generatedHexChanged)
    Q_PROPERTY(int selectedOp READ selectedOp WRITE setSelectedOp NOTIFY selectedOpChanged)
    Q_PROPERTY(int selectedData1 READ selectedData1 WRITE setSelectedData1 NOTIFY selectedData1Changed)

public:
    explicit CommandViewModel(QObject *parent = nullptr);
    ~CommandViewModel() override;

    QVariantList commandGroups() const;
    QVariantMap selectedCommand() const;
    QString generatedHex() const;
    int selectedOp() const;
    int selectedData1() const;

    Q_INVOKABLE void generateHex();
    Q_INVOKABLE void copyHex();

public slots:
    void sendCommand(quint8 cmd, quint8 id);
    void setSelectedCommand(const QVariantMap& cmd);
    void setSelectedOp(int op);
    void setSelectedData1(int data1);

signals:
    void commandSent(quint8 cmd, quint8 id);
    void sendDataRequested(const QByteArray& data);
    void selectedCommandChanged();
    void generatedHexChanged();
    void selectedOpChanged();
    void selectedData1Changed();
    void hexCopied();

private:
    void initCommandGroups();
    QByteArray buildPayload(quint8 cmd, quint8 id);

    QVariantList m_commandGroups;
    QVariantMap m_selectedCommand;
    QString m_generatedHex;
    int m_selectedOp = 0;
    int m_selectedData1 = 0;
};

#endif // COMMANDVIEWMODEL_H
