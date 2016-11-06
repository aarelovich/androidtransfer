#include "direxplorer.h"

DirExplorer::DirExplorer()
{
    QFileInfoList drives = QDir::drives();
    if (drives.size() > 1){
        for (qint32 i = 0; i < drives.size(); i++){
            driveList << drives.at(i).absoluteFilePath();
        }
    }
}

void DirExplorer::setup(ColoUiList *list, ColoUiConfiguration itemConfigForFile, ColoUiConfiguration itemConfigForDir, bool dirOnly){
    dirList = list;
    fileItemConfig = itemConfigForFile;
    dirItemConfig = itemConfigForDir;
    showOnlyDirs = dirOnly;
}

void DirExplorer::exploreStart(QString where){
    current = QDir(where);
    listCurrent();
}

void DirExplorer::goUp(){
    if (!current.cdUp()){
        // Attempt to list drives windows:
        listCurrent(true);
    }
    else listCurrent();
}

void DirExplorer::goInto(int rowOfDir){

    QString dir;
    if ((rowOfDir >= 0) && (rowOfDir < dirList->getRowCount())){
        ColoUiConfiguration c = dirList->getItemConfiguration(rowOfDir);
        dir = c.getString(CPR_TEXT);
    }
    else return;

    current.cd(dir);
    listCurrent();

}


void DirExplorer::listCurrent(bool listDrives){

    QStringList list;
    QStringList fileList;
    QString title;

    if (!listDrives){
        list = current.entryList(QStringList(),QDir::Dirs | QDir::NoDotAndDotDot);
        title = current.absolutePath();
        if (!showOnlyDirs){
            fileList = current.entryList(QStringList(),QDir::Files);
        }
    }
    else{
        list = driveList;
        title = "Drives";
    }

    dirList->clearData();
    for (qint32 i = 0; i < list.size(); i++){
        ColoUiConfiguration item = dirItemConfig;
        //ColoUiConfiguration item = fileItemConfig;
        item.set(CPR_TEXT,list.at(i));
        dirList->insertRow();
        dirList->setItemConfiguration(i,0,item);
    }

    for (qint32 i = 0; i < fileList.size(); i++){
        ColoUiConfiguration item = fileItemConfig;
        item.set(CPR_TEXT,fileList.at(i));
        dirList->insertRow();
        dirList->setItemConfiguration(list.size() +i,0,item);
    }

    // Changing the header
    ColoUiConfiguration c = dirList->getHeaderConfig(0);
    c.set(CPR_TEXT,title);
    dirList->setHeaderConfig(c,0);

}
