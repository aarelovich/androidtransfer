#include "datapacket.h"

DataPacket::DataPacket()
{
    // Default field separator
    fieldSeparator='|';

    // Making sure the header is empty
    header = "";
    packetReceived = false;
    packetSize = 0;
}

void DataPacket::prepareForBuffering(){
    packetData.clear();
    header = "";
    fields.clear();
    packetSize = -1;
    packetReceived = false;
    buffer.clear();
}

void DataPacket::bufferBytes(QByteArray bytes){


    for (qint32 i = 0; i < bytes.size(); i++){

        if (packetSize > 0){
            // This means we are buffering the rest of the data until the packet size is zero, which mean all data was received
            packetData.append(bytes.at(i));
            //qDebug() << "Buffering payload " << QString(packetData);
            packetSize--;
            if (packetSize == 0){
                packetReceived = true;
            }
        }
        else{

            if (bytes.at(i) != fieldSeparator){
                packetData.append(bytes.at(i));
            }
            else{
                // WE got a separator, this is either the header, the file size or the rest of the data
                if (header.isEmpty()){
                    header = QString(packetData);
                    //qDebug() << "Buffer Bytes Got header" << header;
                    packetData.clear();
                }
                else if (packetSize == -1){
                    packetSize = packetData.toInt();
                    //qDebug() << "Buffer Bytes Got size" << packetSize;
                    packetData.clear();
                }
            }
        }
    }


}

void DataPacket::append(QVariant field){
    fields << field;
}

void DataPacket::setHeader(QString string){
    header = string;
}

QByteArray DataPacket::packetToByteArray() const{
    qint32 size = 0;
    QByteArray fieldBytes;
    QByteArray payload;
    payload.append(header.toLatin1());
    payload.append(fieldSeparator);

    for (qint32 i = 0; i < fields.size(); i++){
        QByteArray temp = fields.at(i).toByteArray();
        size = size + temp.size();
        fieldBytes.append(temp);

        // Last field does not require a separator
        if (i < fields.size()-1){
            size = size + 1;
            fieldBytes.append(fieldSeparator);
        }
    }

    payload.append(QByteArray::number(size));
    payload.append(fieldSeparator);
    payload.append(fieldBytes);
    return payload;
}

QVariant DataPacket::getField(quint32 i) const{
    if (i < (quint32)fields.size()){
        return fields.at(i);
    }
    else return QVariant();
}

void DataPacket::setNumberOfFields(qint32 nfields){

    // The header and size have allready been parsed.

    fields.clear();
    QByteArray field;

    if (nfields == 1){
        // All data is one field
        fields << QVariant(packetData);
        packetData.clear();
        return;
    }

    for (qint32 i = 0; i < packetData.size(); i++){
        if (packetData.at(i) == fieldSeparator){

            fields << QVariant(field);
            field.clear();

            if (nfields > 0){
                if (fields.size() == nfields-1){
                    // The last field is the rest of the data
                    qint32 rightDataSize = packetData.size() - i -1;
                    field = packetData.right(rightDataSize);
                    break;
                }
            }

        }
        else{
            field.append(packetData.at(i));
        }
    }

    // Adding the last field
    fields << QVariant(field);

    //qDebug() << "Remaining data had " << fields.size();

    // Clearing the data
    packetData.clear();


}
