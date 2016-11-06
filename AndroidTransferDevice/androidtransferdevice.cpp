#include "androidtransferdevice.h"

AndroidTransferDevice::AndroidTransferDevice(QWidget *parent) : QMainWindow(parent)
{    
    connect(&transferSlave,&TransferManager::connectionEstablished,this,&AndroidTransferDevice::on_masterConnected);
    connect(&transferSlave,&TransferManager::statusUpdate,this,&AndroidTransferDevice::on_statusUpdate);
}

void AndroidTransferDevice::setCentralWidget(QWidget *widget){

   QMainWindow::setCentralWidget(widget);
   ui = (ColoUiContainer*)widget;   
   connect(ui,&ColoUiContainer::elementSignal,this,&AndroidTransferDevice::on_coloUiManager_signal);

   // Actually drawing the UI.
   ui->drawUi();

   initialSetup();

}


void AndroidTransferDevice::on_coloUiManager_signal(){
    ColoUiSignalEventInfo info = ui->getSignalInfo();
    if ( info.elementID == E_VIEWPING_BTLISTEN){
        if (info.type == ST_MOUSE_CLICK){

            if (readyForTransfer) return;

            ColoUiLineEdit *name = (ColoUiLineEdit *)ui->getElement(E_VIEWPING_LTDEVICENAME);
            QString devID = name->getText();
            if (devID.isEmpty()){
                logMsg("Device name cannot be empty",COLOR_ERROR);
                return;
            }

            ColoUiLineEdit *location = (ColoUiLineEdit *)ui->getElement(E_VIEWSEARCHINPUTFILE_LTINPUTFILE);

            if (location->getText().isEmpty()){
                logMsg("Location for download cannot be empty",COLOR_ERROR);
                return;
            }

            QFileInfo info(location->getText());
            if (!info.isDir() || !info.isWritable()){
                logMsg("Selected location for download is not writeable",COLOR_ERROR);
                return;
            }

            // Clearing any last transfer list
            resetUI();

            transferSlave.setSaveDirectory(location->getText());
            transferSlave.listenForUDPAnnouncement(devID);

            changeLight(COLOR_WAITING);

        }
    }
    else if (info.elementID == E_VIEWLOG_MLTLOG ){
        if (info.type == ST_MOUSE_DOUBLE_CLICK){
            ColoUiMultiLineText *logger = (ColoUiMultiLineText *)ui->getElement(E_VIEWLOG_MLTLOG);
            logger->clearText();
        }
    }
    else if (info.elementID == E_VIEWSEARCHINPUTFILE_BTSEARCH ){
        if (info.type == ST_MOUSE_CLICK){
            if (!ui->isViewDrawn(E_VIEWSEARCHFILE)){
                ui->startTranstion(E_VIEWSEARCHFILE,E_VIEWFILELIST);
                dirExplorer.exploreStart(".");
            }
        }
    }
    else if (info.elementID == E_VIEWPING_LTDEVICENAME ){
        if (info.type == ST_TEXT_CHANGED){
            ColoUiLineEdit *name = (ColoUiLineEdit *)ui->getElement(E_VIEWPING_LTDEVICENAME);
            QSettings settings(FILE_SETTINGS,QSettings::IniFormat);
            settings.setValue(SETTINGS_DEVICE_NAME,name->getText());
        }
    }
    else if (info.elementID == E_VIEWSEARCHFILE_BTCANCEL ){
        if (info.type == ST_MOUSE_CLICK){
            ui->startTranstion(E_VIEWSEARCHFILE,E_VIEWFILELIST);
        }
    }
    else if (info.elementID == E_VIEWSEARCHFILE_BTOK ){
        if (info.type == ST_MOUSE_CLICK){
            ColoUiLineEdit *le = (ColoUiLineEdit *) ui->getElement(E_VIEWSEARCHINPUTFILE_LTINPUTFILE);
            QString loc = dirExplorer.getCurrentDir();
            le->setText(dirExplorer.getCurrentDir());
            ui->startTranstion(E_VIEWSEARCHFILE,E_VIEWFILELIST);

            if (!loc.isEmpty()){
                QFileInfo info(loc);
                if (!info.isWritable()){
                    logMsg("The selected location " + loc + " is not writeable");
                    le->setText("");
                    return;
                }
                QSettings settings(FILE_SETTINGS,QSettings::IniFormat);
                settings.setValue(SETTINGS_LOCATION,loc);
            }

        }
    }
    else if (info.elementID == E_VIEWSEARCHFILE_BTUP ){
        if (info.type == ST_MOUSE_CLICK){
            dirExplorer.goUp();
        }
    }
    else if (info.elementID == E_VIEWSEARCHFILE_LISTDIR ){
        if (info.type == ST_MOUSE_DOUBLE_CLICK){
            dirExplorer.goInto(info.data.toPoint().x());
        }
    }

}

void AndroidTransferDevice::initialSetup(){

    // Clearing test data
    resetUI();

    ColoUiMultiLineText *mt = (ColoUiMultiLineText *)ui->getElement(E_VIEWLOG_MLTLOG);
    mt->clearText();

    ColoUiLineEdit *le = (ColoUiLineEdit *)ui->getElement(E_VIEWSEARCHINPUTFILE_LTINPUTFILE);
    le->setText("");

    // Loading data
    QSettings settings(FILE_SETTINGS,QSettings::IniFormat);

    // Loading settings
    ColoUiLineEdit *name = (ColoUiLineEdit *)ui->getElement(E_VIEWPING_LTDEVICENAME);
    QString devname = settings.value(SETTINGS_DEVICE_NAME,"Android device").toString();
    name->setText(devname);

    ColoUiLineEdit *location = (ColoUiLineEdit *)ui->getElement(E_VIEWSEARCHINPUTFILE_LTINPUTFILE);
    QString loc = settings.value(SETTINGS_LOCATION,"").toString();
    location->setText(loc);

    // Testing if location is Writeable
    if (!loc.isEmpty()){
        QFileInfo info(loc);
        if (!info.isWritable()){
            logMsg("The last location " + loc + " is not writeable");
        }
    }

    // Clearing the data and setting the widths for the columns
    ColoUiList *list = (ColoUiList *)ui->getElement(E_VIEWFILELIST_LISTFILETRANSFER);
    list->clearData();

    // Calculating the widths for the colums
    quint16 tw = list->getConfiguration().getUInt16(CPR_WIDTH);
    quint16 colSmall = 0.09*tw;
    quint16 colLarge = tw - colSmall;
    ColoUiConfiguration c = list->getHeaderConfig(0);
    c.set(CPR_WIDTH,colLarge);
    list->setHeaderConfig(c,0);
    c.set(CPR_WIDTH,colSmall);
    c.set(CPR_TEXT,"St.");
    list->setHeaderConfig(c,1);
    list->setLockColumnWidths(true);

    // Setting up the dir explorer
    ColoUiList *fileList = (ColoUiList *)ui->getElement(E_VIEWSEARCHFILE_LISTDIR);
    ColoUiConfiguration fileConfig = ui->getConfiguration(CONFIG_ITEMCONFIG);
    dirExplorer.setup(fileList,fileConfig,fileConfig,true);


}

void AndroidTransferDevice::resetUI(){
    changeLight(COLOR_OFF);
    ColoUiProgressBar *pb = (ColoUiProgressBar *)ui->getElement(E_VIEWPROGRESS_PBARCURRENT);
    pb->setPercent(0);
    pb = (ColoUiProgressBar *)ui->getElement(E_VIEWPROGRESS_PBARTOTAL);
    pb->setPercent(0);
    ColoUiList *list = (ColoUiList *)ui->getElement(E_VIEWFILELIST_LISTFILETRANSFER);
    list->clearData();
}

void AndroidTransferDevice::logMsg(QString msg, QString color){
    QFont font("Code New Roman");
    font.setPixelSize(70);
    ColoUiMultiLineText *logger = (ColoUiMultiLineText *)ui->getElement(E_VIEWLOG_MLTLOG);
    logger->appendFormattedText(msg,font,QColor(color));
}

void AndroidTransferDevice::changeLight(QString color){
    ColoUiButton *b = (ColoUiButton *)ui->getElement(E_VIEWPING_BTPINGSTATUS);
    ColoUiConfiguration c = b->getConfiguration();
    c.setColor(CPR_BACKGROUND_COLOR,color);
    c.setColor(CPR_HOVER_BACKGROUND,color);
    c.setColor(CPR_ALTERNATIVE_BACKGROUND_COLOR,color);
    b->setConfiguration(c);
}

//*************************** SLOTS FOR TRANSFER MANAGER *******************************
void AndroidTransferDevice::on_statusUpdate(){

    TransferManager::UpdateData ud = transferSlave.getUpdateData();

    ColoUiList *list = (ColoUiList *)ui->getElement(E_VIEWFILELIST_LISTFILETRANSFER);
    ColoUiProgressBar *pbar;
    ColoUiConfiguration item;
    qint32 row;

    switch (ud.type){
    case TransferManager::TM_DISCONNECT:
        if (!ud.message.isEmpty()){
            logMsg(ud.message,COLOR_ERROR);
        }
        resetUI();
        readyForTransfer = false;
        break;
    case TransferManager::TM_ERROR:
        logMsg(ud.message,COLOR_ERROR);
        if ((ud.fileID >= 0) && (ud.fileID < list->getRowCount())){
            ColoUiConfiguration c = list->getItemConfiguration(ud.fileID,1);
            c.set(CPR_ICON_PATH,":/assets/icons/fail.png");
            list->setItemConfiguration(ud.fileID,1,c);
        }
        break;
    case TransferManager::TM_NEW_FILE:
        if ((ud.fileID > 0) && (ud.fileID <= list->getRowCount())){
            qint32 row = ud.fileID-1;
            ColoUiConfiguration c = list->getItemConfiguration(row,1);
            if (c.getString(CPR_ICON_PATH).isEmpty()){
               c.set(CPR_ICON_PATH,":/assets/icons/success.png");
               list->setItemConfiguration(row,1,c);
            }
        }

        // Adding the item to a new row.
        item = ui->getConfiguration(CONFIG_ITEMCONFIG);
        list->insertRow();
        row = list->getRowCount()-1;
        list->setItemConfiguration(row,1,item);
        item.set(CPR_TEXT,ud.message);
        list->setItemConfiguration(row,0,item);

        break;
    case TransferManager::TM_LIST_RECEIVED:
        // Clear list
        list->clearData();
        break;
    case TransferManager::TM_NOTIFY:
        logMsg(ud.message);
        // Should mark the last one with a success.
        row = list->getRowCount()-1;
        item = list->getItemConfiguration(row,1);
        if (item.getString(CPR_ICON_PATH).isEmpty()){
           item.set(CPR_ICON_PATH,":/assets/icons/success.png");
           list->setItemConfiguration(row,1,item);
        }
        break;
    case TransferManager::TM_PROGRESS:

        pbar = (ColoUiProgressBar *)ui->getElement(E_VIEWPROGRESS_PBARCURRENT);
        pbar->setPercent(ud.fileProgress);
        pbar = (ColoUiProgressBar *)ui->getElement(E_VIEWPROGRESS_PBARTOTAL);
        pbar->setPercent(ud.totalProgress);

        if (ud.totalProgress == 100){
            row = list->getRowCount()-1;
            item = list->getItemConfiguration(row,1);
            if (item.getString(CPR_ICON_PATH).isEmpty()){
               item.set(CPR_ICON_PATH,":/assets/icons/success.png");
               list->setItemConfiguration(row,1,item);
            }
        }

        break;
    case TransferManager::TM_SKIP:
        if ((ud.fileID >= 0) && (ud.fileID <= list->getRowCount())){
            row = list->getRowCount()-1;
            item = list->getItemConfiguration(row,1);
            if (item.getString(CPR_ICON_PATH).isEmpty()){
               item.set(CPR_ICON_PATH,":/assets/icons/success.png");
               list->setItemConfiguration(row,1,item);
            }
        }

        // Adding the item to a new row.
        item = ui->getConfiguration(CONFIG_ITEMCONFIG);
        list->insertRow();
        row = list->getRowCount()-1;
        item.set(CPR_TEXT,ud.message);
        list->setItemConfiguration(row,0,item);
        item.set(CPR_TEXT,"");
        item.set(CPR_ICON_PATH,":/assets/icons/info.png");
        list->setItemConfiguration(row,1,item);


        break;
    }
}

void AndroidTransferDevice::on_masterConnected(){
    changeLight(COLOR_LIGHT);
    logMsg("Connected to Master " + transferSlave.getMasterID());
    readyForTransfer = true;
}

