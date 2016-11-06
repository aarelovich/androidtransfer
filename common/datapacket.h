#ifndef DATAPACKET_H
#define DATAPACKET_H

#include <QByteArray>
#include <QList>
#include <QVariant>
#include <QDebug>

class DataPacket
{
public:
    DataPacket();

    //--------------  BUFFERING FUNCTIONS ------------
    // Prepares objec to start receiving a packet byte by byte.
    void prepareForBuffering();

    // The first time this function is used buffer mode is enabled.
    // This mode will clear all data first and the then search for header and
    // file size.
    void bufferBytes(QByteArray bytes);
    bool isPacketComplete() const {return packetReceived;}
    void setNumberOfFields(qint32 nfields = -1);

    //--------------  PACKET FORMING FUNCTIONS ------------
    // Usefull to form packets to send
    void setHeader(QString string);
    void append(QVariant field);
    void clear();

    //-------------- DATA ACCESS FUNCTIONS ------------
    QByteArray packetToByteArray() const;
    QVariant getField(quint32 i) const;
    QString getPacketHeader() const {return header;}
    qint32 numberOfFields() const {return fields.size(); }

    // In case the default separator needs to be used.
    void setFieldSeparator(QChar fs) {fieldSeparator = fs;}

private:
    QChar fieldSeparator;
    QString header;
    qint32 packetSize;
    QList<QVariant> fields;
    QByteArray packetData;
    QByteArray buffer;
    bool packetReceived;
};

#endif // DATAPACKET_H
