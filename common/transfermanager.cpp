#include "transfermanager.h"

TransferManager::TransferManager(QObject *parent) : QObject(parent)
{

    QHostAddress address;
    QHostAddress local(QHostAddress::LocalHost);
    QList<QHostAddress> localIPs;
    foreach (address, QNetworkInterface::allAddresses()){
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != local){
            localIPs << QHostAddress(address.toIPv4Address());
        }
    }

    ownIP = localIPs.first().toIPv4Address();
    masterID = "";
    slaveID = "";

    tcpData.prepareForBuffering();

    connect(&tcpServer,&QTcpServer::newConnection,this,&TransferManager::on_incommingConnection);
    tcpServer.blockSignals(true);
    tcpSocket = NULL;


}

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< UDP RELATED FUNCTIONS >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

// Used by Slave
void TransferManager::listenForUDPAnnouncement(QString slave_name){

    if (tcpSocket != NULL) return;

    slaveID = slave_name;

    // Binding the UDP Socket and connecting to the ready read
    udpSocket.blockSignals(false);
    udpSocket.bind(QHostAddress::Any, UDP_PORT, QUdpSocket::ShareAddress | QUdpSocket::ReuseAddressHint);
    connect(&udpSocket,&QUdpSocket::readyRead,this,&TransferManager::on_newUDPData);

    //qDebug() << "Slave name is " << slaveID;
}

// Used by Master
QString TransferManager::sendUDPAnnouncement(QString master_name){

    if (tcpSocket != NULL){
        return "A connection has already been established.";
    }

    masterID = master_name;

    if (!tcpServer.isListening()){
        return "TCPServer has not been started";
    }

    if (master_name.isEmpty()){
        return "ServerID cannot be empty";
    }

    DataPacket datagram;
    datagram.setHeader(UDP_HEADER);
    datagram.append(master_name);
    datagram.append(tcpServer.serverPort());
    QByteArray data2Send = datagram.packetToByteArray();

    if (udpSocket.writeDatagram(data2Send,QHostAddress::Broadcast,UDP_PORT) != data2Send.size() ){
        return udpSocket.errorString();
    }

    return "";

}

// Used by Slave
void TransferManager::on_newUDPData(){

    QByteArray rawdata;
    QHostAddress senderIP;
    quint16 senderPort;

    // Need to redimension dthe datagram size so that It will read all remaining info.
    rawdata.resize(udpSocket.pendingDatagramSize());
    if (udpSocket.readDatagram(rawdata.data(),rawdata.size(),&senderIP,&senderPort) == -1){
        return;
    }

    //qDebug() << "SLAVE: UDP Ping" << QString(rawdata);

    // Parsing the data
    DataPacket datagram;
    datagram.prepareForBuffering();
    datagram.bufferBytes(rawdata);
    datagram.setNumberOfFields();
    if (datagram.numberOfFields() != UDP_ANNOUNCEMENT_FIELDS){
        return;
    }

    if (datagram.getPacketHeader() != UDP_HEADER){
        return;
    }
    masterID = datagram.getField(0).toString();
    quint16 tcpPort = datagram.getField(1).toUInt();

    // If this data is received then a new connection is established.
    tcpSocket = new QTcpSocket();
    tcpSocket->connectToHost(QHostAddress(senderIP),tcpPort);
    connect(tcpSocket,&QTcpSocket::connected,this,&TransferManager::on_connectionEstablished);
    connect(tcpSocket,&QTcpSocket::readyRead,this,&TransferManager::on_newTCPData);
    connect(tcpSocket,&QTcpSocket::disconnected,this,&TransferManager::on_tcpSocket_Disconnected);

    // Make sure no new connections arrive.
    udpSocket.blockSignals(true);
    isMaster = false;


}


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< TCP RELATED FUNCTIONS >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

// Used by Master
QString TransferManager::startTCPServer(){
    if (tcpSocket != NULL) return "A connecion has already been established";
    tcpServer.blockSignals(false);
    isMaster = true;
    if (!tcpServer.listen()){
       return tcpServer.errorString();
    }
    return "";
}

// Used by Master
void TransferManager::on_incommingConnection(){
    tcpSocket = tcpServer.nextPendingConnection();
    tcpServer.blockSignals(true); // Making sure no other connections are established.
    connect(tcpSocket,&QTcpSocket::readyRead,this,&TransferManager::on_newTCPData);
    connect(tcpSocket,&QTcpSocket::disconnected,this,&TransferManager::on_tcpSocket_Disconnected);
}

// Used by Slave
void TransferManager::on_connectionEstablished(){

    // Sending the greeting, as this is the slave.
    DataPacket datagram;
    datagram.setHeader(TCP_HEADER_GREETING);
    datagram.append(slaveID);
    tcpSocket->write(datagram.packetToByteArray());
    emit connectionEstablished();
}

// Used by both
void TransferManager::on_tcpSocket_Disconnected(){

    updateData.type = TM_DISCONNECT;
    updateData.message = "";

    if (fileBeingReceived.isOpen()){
        // Checking status of previous file
        if (fileBeingReceived.isOpen()){
            updateData.message = "File download of: " + fileBeingReceived.fileName() + " has failed due to connection loss"
                       + ". File Size: " + QString::number(currentFileSize)
                       + ". Bytes Received " + QString::number(bytesReceivedOfCurrentFile)
                       + ". Created file will be deleted";
            fileBeingReceived.close();
            fileBeingReceived.remove();
        }
    }

    if (fileBeingTransfered.isOpen()){
        // Checking status of previous file
        if (fileBeingTransfered.isOpen()){
            updateData.message = "File upload of: " + fileBeingTransfered.fileName() + " has failed due to connection loss"
                       + ". File Size: " + QString::number(currentFileSize)
                       + ". Bytes Received " + QString::number(bytesSentOfCurrentFile);
            fileBeingTransfered.close();
        }
    }

    tcpSocket = NULL;
    emit statusUpdate();
    if (!isMaster){
        // The slave should start listening just in case.
        listenForUDPAnnouncement(slaveID);
    }

}


// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< TRANSFER CONTROL FUNCTIONS >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

// Used by Master
void TransferManager::startTransfer(){
    fileTransferID = -1;
    transferFinished = false;
    if (fileList.isEmpty()){
        return;
    }
    if (tcpSocket == NULL){
        updateData.type = TM_ERROR;
        updateData.fileID = -1;
        updateData.message = "No connection has been established";
        emit statusUpdate();
        return;
    }
    DataPacket packet;
    packet.setHeader(TCP_HEADER_TRANSFER_START);
    qDebug() << "Sending the list size";
    packet.append(fileList.size());
    tcpSocket->write(packet.packetToByteArray());

    percentConstant = 100.0/(qreal)fileList.size();

}

// Used by Master
void TransferManager::sendFileInfoNextFile(){

    if (transferFinished) return;

    fileTransferID++;

    for (qint32 i = fileTransferID; i < fileList.size(); i++){
        QFileInfo info(fileList.at(i));
        if (info.exists()){
            fileBeingTransfered.setFileName(info.absoluteFilePath());
            if (fileBeingTransfered.open(QFile::ReadOnly)){

                // The file info packet and the control data for the transfer.
                currentFileSize = info.size();
                bytesSentOfCurrentFile = 0;
                fileTransferID = i;

                DataPacket packet;
                packet.setHeader(TCP_HEADER_FILEINFO);
                packet.append(fileTransferID);
                packet.append(currentFileSize);
                packet.append(info.fileName());

                updateData.type = TM_NEW_FILE;
                updateData.fileID = fileTransferID;
                emit statusUpdate();

                tcpSocket->write(packet.packetToByteArray());

                return;

            }
            else{
                fileBeingTransfered.close();
                updateData.type = TM_ERROR;
                updateData.fileID = i;
                updateData.message = "File " + info.absoluteFilePath() + " could not be open for reading";
                emit statusUpdate();
            }
        }
        else{
            updateData.type = TM_ERROR;
            updateData.fileID = i;
            updateData.message = "File " + info.absoluteFilePath() + " does not exist";
            emit statusUpdate();
        }
    }

    DataPacket packet;
    packet.setHeader(TCP_HEADER_TRANSFER_END);
    packet.append(masterID);
    tcpSocket->write(packet.packetToByteArray());

    updateData.type = TM_NOTIFY;
    transferFinished = true;
    updateData.message = "Finalized file transfer";
    emit statusUpdate();

}

// Used by both to buffer TCP data and process complete packets
void TransferManager::on_newTCPData(){

    while (tcpSocket->bytesAvailable() > 0){
        tcpData.bufferBytes(tcpSocket->read(1));
        if (tcpData.isPacketComplete()){
            processPacket();
            tcpData.prepareForBuffering();
        }
    }

}

// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< PACKET PROCESSING >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

// Used by Both
void TransferManager::processPacket(){

    QString header = tcpData.getPacketHeader();
    //qDebug() << "Processing packet of type:" << header;

    if (header == TCP_HEADER_GREETING){
        tcpData.setNumberOfFields(1);
        slaveID = tcpData.getField(0).toString();
        emit connectionEstablished();
        return;
    }

    // ************** TCP_HEADER_FILEINFO **************
    // Received by the Slave
    if (header == TCP_HEADER_FILEINFO){

        // Checking status of previous file
        if (fileBeingReceived.isOpen()){
            updateData.type = TM_ERROR;
            updateData.fileID = fileTransferID;
            updateData.message = "Previous file was not completely received: " + fileBeingReceived.fileName()
                       + ". File Size: " + QString::number(currentFileSize)
                       + ". Bytes Received " + QString::number(bytesReceivedOfCurrentFile)
                       + ". Created file will be deleted";
            emit statusUpdate();
            fileBeingReceived.close();
            fileBeingReceived.remove();
        }

        tcpData.setNumberOfFields();

        // This provides the file size and potential file name
        fileTransferID = tcpData.getField(0).toInt();
        currentFileSize = tcpData.getField(1).toInt();
        QString filename = tcpData.getField(2).toString();
        updateData.message = filename;
        filename = destinationDirectory + "/" + filename;
        QFileInfo info(filename);

        DataPacket packet;

        if (info.exists()){

            // File should be skipped.
            packet.setHeader(TCP_HEADER_FILE_EXISTS);
            packet.append(fileTransferID);
            tcpSocket->write(packet.packetToByteArray());

            updateData.type = TM_SKIP;
            updateData.fileID = fileTransferID;
            emit statusUpdate();

            return;

        }

        updateData.type = TM_NEW_FILE;
        updateData.fileID = fileTransferID;        
        emit statusUpdate();

        bytesReceivedOfCurrentFile = 0;
        fileBeingReceived.setFileName(filename);
        if (fileBeingReceived.open(QFile::WriteOnly)){

            //qDebug() << "SLAVE: File to receive " << filename << "Size" << currentFileSize << "fileTSID" << fileTransferID;

            packet.setHeader(TCP_HEADER_ACKNOWLEDGE);
            packet.append(fileTransferID);
            tcpSocket->write(packet.packetToByteArray());

        }
        else{

            updateData.type = TM_ERROR;
            updateData.fileID = fileTransferID;
            updateData.message = "Could not open " + filename + " for writing";
            emit statusUpdate();

            packet.setHeader(TCP_HEADER_FILE_ERROR);
            packet.append(fileTransferID);
            tcpSocket->write(packet.packetToByteArray());
        }


    }
    // ************** TCP_HEADER_ACKNOWLEDGE **************
    // Received by the Master
    else if ( header == TCP_HEADER_ACKNOWLEDGE ){

        // Should send next big chunk of data
        QByteArray fileChunk = fileBeingTransfered.read(TCP_FILE_CHUNK_SIZE);
        bytesSentOfCurrentFile = bytesSentOfCurrentFile + fileChunk.size();

        //qDebug() << "Sending" << fileChunk.size();

        DataPacket packet;
        packet.setHeader(TCP_HEADER_RAWDATA);
        packet.append(fileChunk);
        tcpSocket->write(packet.packetToByteArray());

        if (currentFileSize > 0){
            qreal pu = (qreal)bytesSentOfCurrentFile/(qreal)currentFileSize;
            updateData.type = TM_PROGRESS;
            updateData.fileProgress = pu*100.0;
            updateData.totalProgress = percentConstant*(fileTransferID + pu);
            emit statusUpdate();
        }

        if ((fileChunk.size() == 0) || (bytesSentOfCurrentFile >= currentFileSize)){
            fileBeingTransfered.close();

            // Start the transfer of the next file
            sendFileInfoNextFile();

        }

    }
    // ************** TCP_HEADER_FILE_ERROR **************
    // Received by the Master
    else if ( header == TCP_HEADER_FILE_ERROR ){
        updateData.type = TM_ERROR;
        updateData.fileID = fileTransferID;
        updateData.message = slaveID + " could not generate the file " + fileList.at(fileTransferID);
        emit statusUpdate();
        sendFileInfoNextFile();

    }
    // ************** TCP_HEADER_FILE_EXISTS **************
    // Received by the Master
    else if ( header == TCP_HEADER_FILE_EXISTS ){
        //QFileInfo file(fileList.at(fileTransferID));
        updateData.type = TM_SKIP;
        updateData.fileID = fileTransferID;
        emit statusUpdate();
        sendFileInfoNextFile();
    }

    // ************** TCP_HEADER_RAWDATA **************
    // Received by the Slave
    else if ( header == TCP_HEADER_RAWDATA ){

        tcpData.setNumberOfFields(1);
        fileBeingReceived.write(tcpData.getField(0).toByteArray());
        bytesReceivedOfCurrentFile = bytesReceivedOfCurrentFile + tcpData.getField(0).toByteArray().size();

        // qDebug() << "Received File Chunk " << tcpData.getField(0).toByteArray().size();

        if (currentFileSize > 0){
            qreal pu = (qreal)bytesReceivedOfCurrentFile/(qreal)currentFileSize;
            updateData.type = TM_PROGRESS;
            updateData.fileProgress = pu*100.0;
            updateData.totalProgress = percentConstant*(fileTransferID + pu);
            emit statusUpdate();
        }

        if (bytesReceivedOfCurrentFile >= currentFileSize){
            fileBeingReceived.close();
        }

        // Sending the ACK
        DataPacket packet;
        packet.setHeader(TCP_HEADER_ACKNOWLEDGE);
        packet.append(fileTransferID);

        tcpSocket->write(packet.packetToByteArray());

    }

    // ************** TCP_HEADER_TRANSFER_START **************
    // Received by the Slave
    else if ( header == TCP_HEADER_TRANSFER_START ){

        tcpData.setNumberOfFields();
        numOfFilesToTransfer = tcpData.getField(0).toInt();

        // Sending the ACK
        DataPacket packet;
        packet.setHeader(TCP_HEADER_TSTART_ACK);
        packet.append(slaveID);

        percentConstant = 100.0/(qreal)numOfFilesToTransfer;

        qDebug() << "Received new trasnfer start";

        updateData.type = TM_LIST_RECEIVED;
        emit statusUpdate();

        tcpSocket->write(packet.packetToByteArray());

    }
    // ************** TCP_HEADER_TSTART_ACK **************
    // Received by the Master
    else if ( header == TCP_HEADER_TSTART_ACK ){
        sendFileInfoNextFile();
    }
    // ************** TCP_HEADER_TSTART_ACK **************
    // Received by the Slave
    else if ( header == TCP_HEADER_TRANSFER_END ){

        // Checking status of the last file
        if (fileBeingReceived.isOpen()){
            updateData.type = TM_ERROR;
            updateData.fileID = fileTransferID;
            updateData.message = "Previous file was not completely received: " + fileBeingReceived.fileName()
                       + ". File Size: " + QString::number(currentFileSize)
                       + ". Bytes Received " + QString::number(bytesReceivedOfCurrentFile);
            emit statusUpdate();
            fileBeingReceived.close();
        }

        qDebug() << "Transfer end";
    }

    else{
        updateData.type = TM_ERROR;
        updateData.fileID = -1;
        updateData.message = "Unidentifed header received: " + header;
        emit statusUpdate();
        return;
    }


}


