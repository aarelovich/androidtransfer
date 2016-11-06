#include "direxplorer.h"

DirExplorer::DirExplorer()
{

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
    current.cdUp();
    listCurrent();
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


void DirExplorer::listCurrent(){

    QStringList list = current.entryList(QStringList(),QDir::Dirs | QDir::NoDotAndDotDot);
    QStringList fileList;
    if (!showOnlyDirs){
        fileList = current.entryList(QStringList(),QDir::Files);
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
    c.set(CPR_TEXT,current.absolutePath());
    dirList->setHeaderConfig(c,0);

}
