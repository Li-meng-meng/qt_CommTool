#include "CommandViewModel.h"
#include <QVariantMap>
#include <QDebug>
#include <QClipboard>
#include <QGuiApplication>
#include <QtEndian>
#include "../Help/crc.h"

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

    QByteArray payload = buildPayload(cmd, id);

    send_user_head_t head;
    head.sof = 0xAA;
    head.sof1 = 0x55;
    head.cmd = cmd;
    head.id = id;
    head.op = op;
    ushort dataLen = static_cast<ushort>(7 + payload.size() + 2);
    head.v_len = dataLen;

    QByteArray headerBytes(reinterpret_cast<const char*>(&head), sizeof(send_user_head_t));

    QByteArray dataWithoutCrc = headerBytes + payload;

    quint16 calculatedCrc16 = CRC::CRC16(
        reinterpret_cast<const quint8*>(dataWithoutCrc.constData()),
        static_cast<quint16>(dataWithoutCrc.size())
    );

    QByteArray crcBytes(2, 0);
    crcBytes[0] = static_cast<char>(calculatedCrc16 & 0xFF);
    crcBytes[1] = static_cast<char>((calculatedCrc16 >> 8) & 0xFF);

    QByteArray finalPacket = dataWithoutCrc + crcBytes;

    QString hex;
    for (int i = 0; i < finalPacket.size(); ++i) {
        hex += QString("%1 ").arg(static_cast<quint8>(finalPacket[i]), 2, 16, QChar('0')).toUpper();
    }
    hex = hex.trimmed();

    m_generatedHex = hex;
    emit generatedHexChanged();
}

QByteArray CommandViewModel::buildPayload(quint8 cmd, quint8 id)
{
    switch (cmd) {
    case 0:
        switch (id) {
        case 1:
        case 2:
        case 3:
        case 9:
        case 10:
            return QByteArray();
        case 4:
        case 6:
        case 7:
        case 8: {
            QByteArray payload(1, 0);
            payload[0] = static_cast<char>(m_selectedData1 & 0xFF);
            return payload;
        }
        case 5: {
            QByteArray payload(16, 0);
            QString strValue = QString::number(m_selectedData1);
            for (int i = 0; i < strValue.size() && i < 16; ++i) {
                payload[i] = static_cast<char>(strValue.at(i).unicode());
            }
            return payload;
        }
        default:
            return QByteArray();
        }
    case 1:
        switch (id) {
        case 2:
            return QByteArray();
        case 1:
        case 3: {
            QByteArray payload(1, 0);
            payload[0] = static_cast<char>(m_selectedData1 & 0xFF);
            return payload;
        }
        default:
            return QByteArray();
        }
    case 2:
        switch (id) {
        case 2:
            return QByteArray();
        case 1:
        case 3: {
            QByteArray payload(1, 0);
            payload[0] = static_cast<char>(m_selectedData1 & 0xFF);
            return payload;
        }
        default:
            return QByteArray();
        }
    case 3:
        switch (id) {
        default:
            return QByteArray();
        }
    default:
        return QByteArray();
    }
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
    quint8 op = static_cast<quint8>(m_selectedOp);

    QByteArray payload = buildPayload(cmd, id);

    send_user_head_t head;
    head.sof = 0xAA;
    head.sof1 = 0x55;
    head.cmd = cmd;
    head.id = id;
    head.op = op;
    ushort dataLen = static_cast<ushort>(7 + payload.size() + 2);
    head.v_len = dataLen;

    QByteArray headerBytes(reinterpret_cast<const char*>(&head), sizeof(send_user_head_t));

    QByteArray dataWithoutCrc = headerBytes + payload;

    quint16 calculatedCrc16 = CRC::CRC16(
        reinterpret_cast<const quint8*>(dataWithoutCrc.constData()),
        static_cast<quint16>(dataWithoutCrc.size())
    );

    QByteArray crcBytes(2, 0);
    crcBytes[0] = static_cast<char>(calculatedCrc16 & 0xFF);
    crcBytes[1] = static_cast<char>((calculatedCrc16 >> 8) & 0xFF);

    QByteArray finalPacket = dataWithoutCrc + crcBytes;

    emit sendDataRequested(finalPacket);
    emit commandSent(cmd, id);

    qDebug() << "[CommandViewModel] Sent command: cmd=" << QString::number(cmd, 16)
             << ", id=" << QString::number(id, 16)
             << ", HEX=" << finalPacket.toHex(' ').toUpper();
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
