#ifndef COMMIMPL_H
#define COMMIMPL_H

#include <QObject>
#include <QByteArray>
#include <QQueue>
#include <QMutex>
#include <QThread>
#include <QWaitCondition>
#include <type_traits>

class CommImpl : public QObject
{
    Q_OBJECT

public:
    explicit CommImpl(QObject *parent = nullptr);
    ~CommImpl() override;

    Q_INVOKABLE void enqueue(const QByteArray& rawData);
    Q_INVOKABLE void clear();

    template<typename T>
    static T ByteArrayToStructure(const QByteArray& buf)
    {
        static_assert(std::is_trivially_copyable<T>::value, "Type must be trivially copyable");
        T result;
        size_t size = std::min(static_cast<size_t>(buf.size()), sizeof(T));
        memcpy(&result, buf.constData(), size);
        return result;
    }

signals:
    void frameParsed(const QByteArray& frame);
    void parseError(const QString& error);

private:
    void onParseLoop();
    void parseByte(quint8 dat);
    void Reset();
    bool IsOverflow();

    static constexpr int HEX_MAX_PACKET_LENGTH = 256;

    quint8 m_hexParseStep = 0;
    quint16 m_hexBufferIndex = 0;
    quint16 m_hexPacketLength = 0;
    quint16 m_hexCrc16 = 0;

    QByteArray m_hexBuffer;

    // 入队队列（生产者：CommViewModel，消费者：解析线程）
    QQueue<quint8> m_receiveQueue;
    QMutex m_queueMutex;
    QWaitCondition m_queueCond;
    bool m_stopThread = false;

    // 解析线程
    QThread* m_parseThread = nullptr;
};

#endif // COMMIMPL_H
