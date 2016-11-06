#include "androidtransferpc.h"

AndroidTransferPC::AndroidTransferPC(QWidget *parent) : QMainWindow(parent)
{
    availableForTransfer = false;
    transferMaster.startTCPServer();
    connect(&transferMaster,&TransferManager::connectionEstablished,this,&AndroidTransferPC::on_slaveGreeting);
    connect(&transferMaster,&TransferManager::statusUpdate,this,&AndroidTransferPC::on_statusUpdate);
}

void AndroidTransferPC::setCentralWidget(QWidget *widget){
   QMainWindow::setCentralWidget(widget);
   ui = (ColoUiContainer*)widget;   
   connect(ui,&ColoUiContainer::elementSignal,this,&AndroidTransferPC::on_coloUiManager_signal);

   // Actually drawing the UI.
   ui->drawUi();

   initialSetup();
}


void AndroidTransferPC::on_coloUiManager_signal(){
    ColoUiSignalEventInfo info = ui->getSignalInfo();
    if ( info.elementID == E_VIEWPROGRESS){
    }
    else if (info.elementID == E_VIEWPROGRESS_LABELCFILE ){
    }
    else if (info.elementID == E_VIEWPROGRESS_LABELTOTAL ){
    }
    else if (info.elementID == E_VIEWPROGRESS_PBARCURRENT ){
    }
    else if (info.elementID == E_VIEWPROGRESS_PBARTOTAL ){
    }
    else if (info.elementID == E_VIEWFILELIST ){
    }
    else if (info.elementID == E_VIEWFILELIST_LISTFILETRANSFER ){
    }
    else if (info.elementID == E_VIEWPING ){
    }
    else if (info.elementID == E_VIEWPING_BTPING ){
        if (info.type == ST_MOUSE_CLICK){
            ColoUiLineEdit *le = (ColoUiLineEdit *)ui->getElement(E_VIEWPING_LTDEVICENAME);
            transferMaster.sendUDPAnnouncement(le->getText());
        }
    }
    else if (info.elementID == E_VIEWPING_BTSTART){
        if (info.type == ST_MOUSE_CLICK){
            if (availableForTransfer){
                transferMaster.setTransferList(transferList);
                transferMaster.startTransfer();
            }
            else{
                logMsg("Not connected to any slave",COLOR_ERROR);
            }
        }
    }
    else if (info.elementID == E_VIEWPING_BTPINGSTATUS ){
    }
    else if (info.elementID == E_VIEWPING_LABELDEVNAME ){
    }
    else if (info.elementID == E_VIEWPING_LTDEVICENAME ){
    }
    else if (info.elementID == E_VIEWSEARCHFILE ){
    }
    else if (info.elementID == E_VIEWSEARCHFILE_BTCANCEL ){
        if (info.type == ST_MOUSE_CLICK){
            ui->startTranstion(E_VIEWFILELIST,E_VIEWSEARCHFILE);
        }
    }
    else if (info.elementID == E_VIEWSEARCHFILE_BTUP ){
        if (info.type == ST_MOUSE_CLICK){
            dirExplorer.goUp();
        }
    }
    else if (info.elementID == E_VIEWSEARCHFILE_LISTDIR ){
        if (info.type == ST_MOUSE_DOUBLE_CLICK){
            qint32 rowClicked = info.data.toPoint().x();
            ColoUiList *dlist = (ColoUiList *)ui->getElement(E_VIEWSEARCHFILE_LISTDIR);
            ColoUiConfiguration c = dlist->getItemConfiguration(rowClicked,0);
            if (c.getBool(IS_FOLDER)){
                dirExplorer.goInto(rowClicked);
            }
            else{
                QString selFile = dirExplorer.getCurrentDir() + "/" + c.getString(CPR_TEXT);
                ColoUiLineEdit *le = (ColoUiLineEdit *)ui->getElement(E_VIEWSEARCHINPUTFILE_LTINPUTFILE);
                le->setText(selFile);
                ui->startTranstion(E_VIEWFILELIST,E_VIEWSEARCHFILE);
                parseFileList(selFile);
            }
        }
    }
    else if (info.elementID == E_VIEWSEARCHINPUTFILE ){
    }
    else if (info.elementID == E_VIEWSEARCHINPUTFILE_BTSEARCH ){
        if (info.type == ST_MOUSE_CLICK){
            if (!ui->isViewDrawn(E_VIEWSEARCHFILE)){
                ui->startTranstion(E_VIEWFILELIST,E_VIEWSEARCHFILE);
                dirExplorer.exploreStart(".");
            }
        }
    }
    else if (info.elementID == E_VIEWSEARCHINPUTFILE_LABELFILEINPUT ){
    }
    else if (info.elementID == E_VIEWSEARCHINPUTFILE_LTINPUTFILE ){
    }
    else if (info.elementID == E_VIEWLOG ){
    }
    else if (info.elementID == E_VIEWLOG_LABELLOG ){
    }
    else if (info.elementID == E_VIEWLOG_MLTLOG ){
        if (info.type == ST_MOUSE_DOUBLE_CLICK){
            ColoUiMultiLineText *logger = (ColoUiMultiLineText *)ui->getElement(E_VIEWLOG_MLTLOG);
            logger->clearText();
        }
    }

}

void AndroidTransferPC::initialSetup(){

    QSettings settings(FILE_SETTINGS,QSettings::IniFormat);

    // Loading settings
    ColoUiLineEdit *name = (ColoUiLineEdit *)ui->getElement(E_VIEWPING_LTDEVICENAME);
    QString devname = settings.value(SETTINGS_DEVICE_NAME,"Device name").toString();
    name->setText(devname);


    // Clearing the data and setting the widths for the columns
    ColoUiList *list = (ColoUiList *)ui->getElement(E_VIEWFILELIST_LISTFILETRANSFER);
    list->clearData();

    // Calculating the widths for the colums
    quint16 tw = list->getConfiguration().getUInt16(CPR_WIDTH);
    quint16 colSmall = 0.06*tw;
    quint16 colLarge = tw - colSmall;
    ColoUiConfiguration c = list->getHeaderConfig(0);
    c.set(CPR_WIDTH,colLarge);
    list->setHeaderConfig(c,0);
    c.set(CPR_WIDTH,colSmall);
    c.set(CPR_TEXT,"Status");
    list->setHeaderConfig(c,1);
    list->setLockColumnWidths(true);

    // Making the OK button invisible in the dir list
    ColoUiButton *ok = (ColoUiButton *)ui->getElement(E_VIEWSEARCHFILE_BTOK);
    ok->setVisible(false);

    // Setting up the dir explorer
    ColoUiList *fileList = (ColoUiList *)ui->getElement(E_VIEWSEARCHFILE_LISTDIR);
    ColoUiConfiguration fileConfig = ui->getConfiguration(CONFIG_ITEMCONFIG);
    fileConfig.set(IS_FOLDER,false);
    ColoUiConfiguration dirConfig = fileConfig;
    dirConfig.set(CPR_ICON_PATH,":/assets/icons/folder.png");
    dirConfig.set(IS_FOLDER,true);
    dirExplorer.setup(fileList,fileConfig,dirConfig,false);

    // Clearing test data
    ColoUiProgressBar *pb = (ColoUiProgressBar *)ui->getElement(E_VIEWPROGRESS_PBARCURRENT);
    pb->setPercent(0);
    pb = (ColoUiProgressBar *)ui->getElement(E_VIEWPROGRESS_PBARTOTAL);
    pb->setPercent(0);

    fileList = (ColoUiList *)ui->getElement(E_VIEWFILELIST_LISTFILETRANSFER);
    fileList->clearData();
    fileList->setLockColumnWidths(true);

    ColoUiMultiLineText *mt = (ColoUiMultiLineText *)ui->getElement(E_VIEWLOG_MLTLOG);
    mt->clearText();

    ColoUiLineEdit *le = (ColoUiLineEdit *)ui->getElement(E_VIEWSEARCHINPUTFILE_LTINPUTFILE);
    le->setText("");

}

void AndroidTransferPC::logMsg(QString msg, QString color){
    QFont font("Code New Roman");
    font.setPixelSize(30);
    ColoUiMultiLineText *logger = (ColoUiMultiLineText *)ui->getElement(E_VIEWLOG_MLTLOG);
    logger->appendFormattedText(msg,font,QColor(color));
}

void AndroidTransferPC::parseFileList(QString file){

    QFile f(file);
    if (!f.open(QFile::ReadOnly)){
        logMsg("Could not open " + file + " for reading",COLOR_ERROR);
        return;
    }

    ColoUiList *flist = (ColoUiList *)ui->getElement(E_VIEWFILELIST_LISTFILETRANSFER);
    flist->clearData();
    transferList.clear();

    ColoUiConfiguration listItem = ui->getConfiguration(CONFIG_ITEMCONFIG);

    QTextStream reader(&f);
    while (!reader.atEnd()){

        QString line = reader.readLine();
        if (line.startsWith('#')) continue;

        if (QFile(line).exists()){

            QFileInfo info(line);
            flist->insertRow();
            qint32 row = flist->getRowCount()-1;

            listItem.set(CPR_TEXT,info.fileName());
            flist->setItemConfiguration(row,0,listItem);
            listItem.set(CPR_TEXT,"");
            flist->setItemConfiguration(row,1,listItem);

            transferList << line;

        }
        else{
            logMsg("Could not find file " + line);
        }
    }


}

void AndroidTransferPC::closeEvent(QCloseEvent *e){
    Q_UNUSED(e);
    QSettings settings(FILE_SETTINGS,QSettings::IniFormat);

    ColoUiLineEdit *name = (ColoUiLineEdit *)ui->getElement(E_VIEWPING_LTDEVICENAME);
    settings.setValue(SETTINGS_DEVICE_NAME,name->getText());
}

void AndroidTransferPC::changeLight(QString color){
    ColoUiButton *b = (ColoUiButton *)ui->getElement(E_VIEWPING_BTPINGSTATUS);
    ColoUiConfiguration c = b->getConfiguration();
    c.setColor(CPR_BACKGROUND_COLOR,color);
    c.setColor(CPR_HOVER_BACKGROUND,color);
    c.setColor(CPR_ALTERNATIVE_BACKGROUND_COLOR,color);
    b->setConfiguration(c);

}


//************************* TRANSFER FUNCTIONS AND SLOTS ******************************
void AndroidTransferPC::on_slaveGreeting(){
    availableForTransfer = true;
    changeLight(COLOR_LIGHT);
    logMsg("Connected to slave " + transferMaster.getSlaveID());
}

void AndroidTransferPC::resetUI(){
    changeLight(COLOR_OFF);
    ColoUiProgressBar *pb = (ColoUiProgressBar *)ui->getElement(E_VIEWPROGRESS_PBARCURRENT);
    pb->setPercent(0);
    pb = (ColoUiProgressBar *)ui->getElement(E_VIEWPROGRESS_PBARTOTAL);
    pb->setPercent(0);
    ColoUiList *list = (ColoUiList *)ui->getElement(E_VIEWFILELIST_LISTFILETRANSFER);
    list->clearData();
}

void AndroidTransferPC::on_statusUpdate(){

    TransferManager::UpdateData ud = transferMaster.getUpdateData();

    ColoUiList *list = (ColoUiList *)ui->getElement(E_VIEWFILELIST_LISTFILETRANSFER);
    ColoUiProgressBar *pbar;
    ColoUiConfiguration item;
    qint32 row;

    switch (ud.type){
    case TransferManager::TM_DISCONNECT:
        resetUI();
        availableForTransfer = false;
        if (!ud.message.isEmpty()){
            logMsg(ud.message,COLOR_ERROR);
        }
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
        if ((ud.fileID > 0) && (ud.fileID < list->getRowCount())){
            qint32 row = ud.fileID-1;
            ColoUiConfiguration c = list->getItemConfiguration(row,1);
            if (c.getString(CPR_ICON_PATH).isEmpty()){
               c.set(CPR_ICON_PATH,":/assets/icons/success.png");
               list->setItemConfiguration(row,1,c);
            }
        }
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
    case TransferManager::TM_LIST_RECEIVED:
        // Master will never receive this signal.
        break;
    case TransferManager::TM_PROGRESS:

        pbar = (ColoUiProgressBar *)ui->getElement(E_VIEWPROGRESS_PBARCURRENT);
        pbar->setPercent(ud.fileProgress);
        pbar = (ColoUiProgressBar *)ui->getElement(E_VIEWPROGRESS_PBARTOTAL);
        pbar->setPercent(ud.totalProgress);

        break;
    case TransferManager::TM_SKIP:
        if ((ud.fileID >= 0) && (ud.fileID < list->getRowCount())){
            ColoUiConfiguration c = list->getItemConfiguration(ud.fileID,1);
            c.set(CPR_ICON_PATH,":/assets/icons/info.png");
            list->setItemConfiguration(ud.fileID,1,c);
        }
        break;
    }
}


