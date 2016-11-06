#ifndef ANDROIDTRANSFERDEVICE
#define ANDROIDTRANSFERDEVICE

#include <QMainWindow>
#include <QSettings>

#include "colouicontainer.h"
#include "elements.h"

#include "../common/direxplorer.h"
#include "../common/transfermanager.h"
#include "../common/defines.h"


class AndroidTransferDevice : public QMainWindow
{
    Q_OBJECT
public:
    explicit AndroidTransferDevice(QWidget *parent = 0);
    void setCentralWidget(QWidget * widget);

signals:

private slots:
    void on_coloUiManager_signal();
    void on_masterConnected();
    void on_statusUpdate();

private:
    ColoUiContainer *ui;

    DirExplorer dirExplorer;

    TransferManager transferSlave;
    bool readyForTransfer;

    void initialSetup();
    void logMsg(QString msg, QString color = "#FFFFFF");
    void changeLight(QString color);
    void resetUI();
};

#endif // ANDROIDTRANSFERDEVICE
