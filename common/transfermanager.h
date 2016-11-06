#ifndef MASTERCONNECTION_H
#define MASTERCONNECTION_H

#include <QObject>
#include <QUdpSocket>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QFile>
#include <QFileInfo>
#include <QDir>

#include "datapacket.h"

//------------------------ DEFINES ----------------------------------------
#define   UDP_HEADER                "CMP_MASTER_PING"
#define   UDP_PORT                  25001
#define   UDP_ANNOUNCEMENT_FIELDS   2

#define   TCP_FILE_CHUNK_SIZE       5000

#define   TCP_HEADER_GREETING       "CMP_GREETING"
#define   TCP_HEADER_RAWDATA        "CMP_RAWDATA"
#define   TCP_HEADER_FILEINFO       "CMP_FILESIZE"
#define   TCP_HEADER_FILE_EXISTS    "CMP_FILE_EXISTS"
#define   TCP_HEADER_FILE_ERROR     "CMP_FILE_ERROR"
#define   TCP_HEADER_ACKNOWLEDGE    "CMP_ACK"
#define   TCP_HEADER_TRANSFER_START "CMP_TRANSFER_START"
#define   TCP_HEADER_TRANSFER_END   "CMP_TRANSFER_END"
#define   TCP_HEADER_TSTART_ACK     "CMP_TRANSFER_START_ACK"


class TransferManager : public QObject
{
    Q_OBJECT
public:

    typedef enum {TM_NEW_FILE,TM_ERROR,TM_NOTIFY,TM_PROGRESS,TM_DISCONNECT,TM_SKIP,TM_LIST_RECEIVED} UpdateType;
    struct UpdateData{
        UpdateType type;
        QString message;
        qint32 fileID;
        qint32 totalProgress;
        qint32 fileProgress;
    };

    explicit TransferManager(QObject *parent = 0);

    // Functions for self discovery
    QString startTCPServer();
    void listenForUDPAnnouncement(QString slave_name);
    QString sendUDPAnnouncement(QString master_name);

    void setTransferList(QStringList tlist){fileList = tlist;}
    void startTransfer();
    void setSaveDirectory(QString savedir){destinationDirectory = savedir;}

    // Getting basic data
    QString getMasterID() const {return masterID;}
    QString getSlaveID() const {return slaveID;}
    quint16 getCurrentIP() const {return ownIP;}

    UpdateData getUpdateData() const {return updateData;}

signals:
    void connectionEstablished();
    void statusUpdate();

private slots:
    void on_newUDPData();
    void on_newTCPData();
    void on_connectionEstablished();
    void on_incommingConnection();
    void on_tcpSocket_Disconnected();

private:

    // Network classes
    QTcpSocket *tcpSocket;
    QTcpServer tcpServer;
    QUdpSocket udpSocket;
    quint32 ownIP;

    // Slave and master identification
    QString masterID;
    QString slaveID;
    bool isMaster;

    // TCP Data packet
    DataPacket tcpData;

    // Update data
    UpdateData updateData;

    // File transfer variables
    qint64 currentFileSize;
    qint64 bytesReceivedOfCurrentFile;
    qint64 bytesSentOfCurrentFile;
    QFile fileBeingReceived;
    QFile fileBeingTransfered;
    qint32 fileTransferID;
    qint32 numOfFilesToTransfer;
    QStringList fileList;
    QString destinationDirectory;
    qreal percentConstant;

    // To prevent multiple notifications on the master
    bool transferFinished;

    void sendFileInfoNextFile();
    void processPacket();

};

#endif // MASTERCONNECTION_H
