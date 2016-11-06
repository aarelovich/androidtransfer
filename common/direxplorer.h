#ifndef DIREXPLORER_H
#define DIREXPLORER_H

#include <QDir>
#include "colouilist.h"

class DirExplorer
{
public:
    DirExplorer();
    void setup(ColoUiList *list, ColoUiConfiguration itemConfigForFile, ColoUiConfiguration itemConfigForDir, bool dirOnly = true);
    void exploreStart(QString where);
    void goUp();
    void goInto(int rowOfDir);
    QString getCurrentDir() const {return current.absolutePath();}

private:
    QDir current;
    ColoUiList *dirList;
    ColoUiConfiguration fileItemConfig;
    ColoUiConfiguration dirItemConfig;
    bool showOnlyDirs;

    // Drive list for windows
    QStringList driveList;

    void listCurrent(bool listDrives = false);
};

#endif // DIREXPLORER_H
