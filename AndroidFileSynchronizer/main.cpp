#include <QApplication>
#include "androidtransferpc.h"
#include "colouicreator.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    AndroidTransferPC *mainWindow = new AndroidTransferPC();
    ColoUiContainer *ui = new ColoUiContainer(mainWindow);

    // Creating the Ui
    ColoUiCreator creator;
    creator.createFromResource(ui);

    mainWindow->setCentralWidget(ui);
    mainWindow->showMaximized();

    return a.exec();
}
