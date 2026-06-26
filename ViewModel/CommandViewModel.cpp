#include "CommandViewModel.h"
#include <QVariantMap>
#include <QDebug>
#include <QClipboard>
#include <QGuiApplication>
#include <QtEndian>

CommandViewModel::CommandViewModel(QObject *parent)
    : QObject(parent)
{
    initCommandGroups();
}

CommandViewModel::~CommandViewModel()
{
}

QVariantList CommandViewModel::commandGroups() const
{
    return m_commandGroups;
}

QVariantMap CommandViewModel::selectedCommand() const
{
    return m_selectedCommand;
}

QString CommandViewModel::generatedHex() const
{
    return m_generatedHex;
}

int CommandViewModel::selectedOp() const
{
    return m_selectedOp;
}

int CommandViewModel::selectedData1() const
{
    return m_selectedData1;
}

void CommandViewModel::generateHex()
{
    if (m_selectedCommand.isEmpty()) {
        m_generatedHex = "";
        emit generatedHexChanged();
        return;
    }

    quint8 cmd = m_selectedCommand["cmd"].toUInt();
    quint8 id = m_selectedCommand["subId"].toUInt();
    quint8 op = static_cast<quint8>(m_selectedOp);
    quint16 data1 = static_cast<quint16>(m_selectedData1);

    send_user_head_t head;
    head.sof = 0xAA;
    head.sof1 = 0x55;
    head.cmd = cmd;
    head.id = id;
    head.op = op;
    head.v_len = qToBigEndian(data1);

    QString hex;
    hex += QString("%1 ").arg(head.sof, 2, 16, QChar('0')).toUpper();
    hex += QString("%1 ").arg(head.sof1, 2, 16, QChar('0')).toUpper();
    hex += QString("%1 ").arg(head.cmd, 2, 16, QChar('0')).toUpper();
    hex += QString("%1 ").arg(head.id, 2, 16, QChar('0')).toUpper();
    hex += QString("%1 ").arg(head.op, 2, 16, QChar('0')).toUpper();
    hex += QString("%1 ").arg((head.v_len >> 8) & 0xFF, 2, 16, QChar('0')).toUpper();
    hex += QString("%1").arg(head.v_len & 0xFF, 2, 16, QChar('0')).toUpper();

    m_generatedHex = hex;
    emit generatedHexChanged();
}

void CommandViewModel::copyHex()
{
    if (!m_generatedHex.isEmpty()) {
        QClipboard *clipboard = QGuiApplication::clipboard();
        clipboard->setText(m_generatedHex);
        emit hexCopied();
    }
}

void CommandViewModel::sendCommand(quint8 cmd, quint8 id)
{
    send_user_head_t head;
    head.sof = 0xAA;
    head.sof1 = 0x55;
    head.cmd = cmd;
    head.id = id;
    head.op = static_cast<quint8>(m_selectedOp);
    head.v_len = qToBigEndian(static_cast<quint16>(m_selectedData1));

    QByteArray data(reinterpret_cast<const char*>(&head), sizeof(send_user_head_t));
    emit sendDataRequested(data);
    emit commandSent(cmd, id);

    qDebug() << "[CommandViewModel] Sent command: cmd=" << QString::number(cmd, 16)
             << ", id=" << QString::number(id, 16);
}

void CommandViewModel::setSelectedCommand(const QVariantMap& cmd)
{
    m_selectedCommand = cmd;
    emit selectedCommandChanged();
    generateHex();
}

void CommandViewModel::setSelectedOp(int op)
{
    m_selectedOp = op;
    emit selectedOpChanged();
    generateHex();
}

void CommandViewModel::setSelectedData1(int data1)
{
    m_selectedData1 = data1;
    emit selectedData1Changed();
    generateHex();
}

void CommandViewModel::initCommandGroups()
{
    QVariantList group0;
    group0.append(QVariantMap{ {"id", 1}, {"label", "传感器数据"}, {"cmd", 0}, {"subId", 1}, {"description", "请求传感器数据"} });
    group0.append(QVariantMap{ {"id", 2}, {"label", "状态数据"}, {"cmd", 0}, {"subId", 2}, {"description", "请求状态数据"} });
    group0.append(QVariantMap{ {"id", 3}, {"label", "视觉数据"}, {"cmd", 0}, {"subId", 3}, {"description", "请求视觉数据"} });
    group0.append(QVariantMap{ {"id", 4}, {"label", "频率数据"}, {"cmd", 0}, {"subId", 4}, {"description", "请求频率数据"} });
    group0.append(QVariantMap{ {"id", 5}, {"label", "序列号"}, {"cmd", 0}, {"subId", 5}, {"description", "请求设备序列号"} });
    group0.append(QVariantMap{ {"id", 6}, {"label", "算法模式"}, {"cmd", 0}, {"subId", 6}, {"description", "请求算法模式"} });
    group0.append(QVariantMap{ {"id", 7}, {"label", "参数A"}, {"cmd", 0}, {"subId", 7}, {"description", "请求参数A"} });
    group0.append(QVariantMap{ {"id", 8}, {"label", "电源状态"}, {"cmd", 0}, {"subId", 8}, {"description", "请求电源状态"} });
    group0.append(QVariantMap{ {"id", 9}, {"label", "校准数据"}, {"cmd", 0}, {"subId", 9}, {"description", "请求校准数据"} });
    group0.append(QVariantMap{ {"id", 10}, {"label", "传感器原始"}, {"cmd", 0}, {"subId", 10}, {"description", "请求传感器原始数据"} });

    QVariantList group1;
    group1.append(QVariantMap{ {"id", 1}, {"label", "设置频率"}, {"cmd", 1}, {"subId", 1}, {"description", "设置数据发送频率"} });
    group1.append(QVariantMap{ {"id", 2}, {"label", "设置算法"}, {"cmd", 1}, {"subId", 2}, {"description", "设置算法模式"} });
    group1.append(QVariantMap{ {"id", 3}, {"label", "设置参数"}, {"cmd", 1}, {"subId", 3}, {"description", "设置设备参数"} });

    QVariantList group2;
    group2.append(QVariantMap{ {"id", 1}, {"label", "查询频率"}, {"cmd", 2}, {"subId", 1}, {"description", "查询当前频率"} });
    group2.append(QVariantMap{ {"id", 2}, {"label", "查询算法"}, {"cmd", 2}, {"subId", 2}, {"description", "查询当前算法"} });
    group2.append(QVariantMap{ {"id", 3}, {"label", "查询参数"}, {"cmd", 2}, {"subId", 3}, {"description", "查询设备参数"} });

    QVariantList group3;
    group3.append(QVariantMap{ {"id", 2}, {"label", "开始校准"}, {"cmd", 3}, {"subId", 2}, {"description", "开始传感器校准"} });
    group3.append(QVariantMap{ {"id", 3}, {"label", "获取校准"}, {"cmd", 3}, {"subId", 3}, {"description", "获取校准结果"} });

    m_commandGroups.append(QVariantMap{ {"name", "数据请求"}, {"cmd", 0}, {"commands", group0} });
    m_commandGroups.append(QVariantMap{ {"name", "参数设置"}, {"cmd", 1}, {"commands", group1} });
    m_commandGroups.append(QVariantMap{ {"name", "参数查询"}, {"cmd", 2}, {"commands", group2} });
    m_commandGroups.append(QVariantMap{ {"name", "校准控制"}, {"cmd", 3}, {"commands", group3} });
}
