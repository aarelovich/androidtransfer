#ifndef ANDROIDTRANSFERPC
#define ANDROIDTRANSFERPC

#include <QMainWindow>
#include <QSettings>

#include "colouicontainer.h"
#include "elements.h"

#include "../common/direxplorer.h"
#include "../common/transfermanager.h"
#include "../common/defines.h"

class AndroidTransferPC : public QMainWindow
{
    Q_OBJECT
public:
    explicit AndroidTransferPC(QWidget *parent = 0);
    void setCentralWidget(QWidget * widget);

signals:

private slots:
    void on_coloUiManager_signal();    
    void on_slaveGreeting();
    void on_statusUpdate();

protected:
    void closeEvent(QCloseEvent *e);

private:
    ColoUiContainer *ui;

    DirExplorer dirExplorer;

    TransferManager transferMaster;

    QStringList transferList;

    void initialSetup();

    void parseFileList(QString file);

    void logMsg(QString msg, QString color = "#FFFFFF");

    void changeLight(QString color);

    void resetUI();

    bool availableForTransfer;
};

#endif // ANDROIDTRANSFERPC
