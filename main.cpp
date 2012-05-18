/*
 * re-stand-in
 *
 * Heiko Metzger
 * 07.05.2012
 * rsi.cpp
 */

#include <QtGui>
#include "rsi.h"

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(resources);

    QApplication app(argc, argv);

    if(!QSystemTrayIcon::isSystemTrayAvailable()) {
        QMessageBox::critical(0, QObject::tr("re-stand-in"), QObject::tr("Dieses System hat keinen Systemtray, re-stand-in kann nicht ausgefuehrt werden!"));
        return 1;
    }
    QApplication::setQuitOnLastWindowClosed(false);

    rsi w;
    w.show();
    
    return app.exec();
}
