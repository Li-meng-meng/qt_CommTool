#include "CommImpl.h"
#include "crc.h"
#include <QDebug>

CommImpl::CommImpl(QObject *parent)
    : QObject(parent)
    , m_hexBuffer(HEX_MAX_PACKET_LENGTH, 0)
{
    // 创建解析线程
    m_parseThread = QThread::create([this]() {
        qDebug() << "[CommImpl] Parse thread started";
        onParseLoop();
        qDebug() << "[CommImpl] Parse thread stopped";
    });
    m_parseThread->start();
}

CommImpl::~CommImpl()
{
    qDebug() << "[CommImpl] Destructor START";

    // 停止解析线程
    {
        QMutexLocker locker(&m_queueMutex);
        m_stopThread = true;
        m_queueCond.wakeAll();
    }

    if (m_parseThread) {
        m_parseThread->quit();
        m_parseThread->wait(1000);
        delete m_parseThread;
        m_parseThread = nullptr;
    }

    qDebug() << "[CommImpl] Destructor END";
}

void CommImpl::enqueue(const QByteArray& rawData)
{
    QMutexLocker locker(&m_queueMutex);
    for (int i = 0; i < rawData.size(); ++i) {
        m_receiveQueue.enqueue(static_cast<quint8>(rawData[i]));
    }
    m_queueCond.wakeAll();
}

void CommImpl::clear()
{
    QMutexLocker locker(&m_queueMutex);
    m_receiveQueue.clear();
    Reset();
}

void CommImpl::onParseLoop()
{
    qDebug() << "[CommImpl] Parse thread started, waiting for data...";
    while (true) {
        quint8 dat = 0;
        bool hasData = false;

        {
            QMutexLocker locker(&m_queueMutex);
            if (m_stopThread) {
                break;
            }

            if (m_receiveQueue.isEmpty()) {
                m_queueCond.wait(&m_queueMutex);
                if (m_stopThread) {
                    break;
                }
                if (m_receiveQueue.isEmpty()) {
                    continue;
                }
            }

            dat = m_receiveQueue.dequeue();
            hasData = true;
        }

        if (hasData) {
            parseByte(dat);
        }
    }
}

void CommImpl::parseByte(quint8 dat)
{
    switch (m_hexParseStep) {
    case 0:
        if (dat == 0xAA) {
            m_hexBufferIndex = 0;
            m_hexBuffer[m_hexBufferIndex++] = dat;
            m_hexParseStep = 1;
        }
        break;

    case 1:
        if (dat == 0x55) {
            m_hexBuffer[m_hexBufferIndex++] = dat;
            m_hexParseStep = 2;
        } else {
            Reset();
        }
        break;

    case 2:
        if (IsOverflow()) break;
        m_hexBuffer[m_hexBufferIndex++] = dat;
        m_hexParseStep = 3;
        break;

    case 3:
        if (IsOverflow()) break;
        m_hexBuffer[m_hexBufferIndex++] = dat;
        m_hexParseStep = 4;
        break;

    case 4:
        if (IsOverflow()) break;
        m_hexBuffer[m_hexBufferIndex++] = dat;
        m_hexPacketLength = dat;
        m_hexParseStep = 5;
        break;

    case 5:
        if (IsOverflow()) break;
        m_hexBuffer[m_hexBufferIndex++] = dat;
        m_hexPacketLength = static_cast<quint16>(m_hexPacketLength + ((dat & 0x0F) << 8));

        if (m_hexPacketLength > HEX_MAX_PACKET_LENGTH) {
            Reset();
            break;
        }

        m_hexParseStep = 6;
        break;

    case 6:
        if (IsOverflow()) break;
        m_hexBuffer[m_hexBufferIndex++] = dat;

        if (m_hexBufferIndex >= m_hexPacketLength) {
            if (m_hexPacketLength >= 2) {
                m_hexCrc16 = static_cast<quint16>(
                    (m_hexBuffer[m_hexPacketLength - 1] << 8) |
                    m_hexBuffer[m_hexPacketLength - 2]
                );

                if (CRC::CRC16(reinterpret_cast<const quint8*>(m_hexBuffer.data()), static_cast<quint16>(m_hexPacketLength - 2)) == m_hexCrc16) {
                    QByteArray frame(m_hexBuffer.constData(), static_cast<int>(m_hexPacketLength));
                    emit frameParsed(frame);
                } else {
                    emit parseError("CRC16 verification failed");
                }
            }
            Reset();
        }
        break;

    default:
        Reset();
        break;
    }
}

void CommImpl::Reset()
{
    m_hexBufferIndex = 0;
    m_hexParseStep = 0;
    m_hexPacketLength = 0;
}

bool CommImpl::IsOverflow()
{
    if (m_hexBufferIndex >= static_cast<quint16>(m_hexBuffer.size())) {
        Reset();
        return true;
    }
    return false;
}
