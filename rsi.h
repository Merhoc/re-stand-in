/*
 * re-stand-in
 *
 * Heiko Metzger
 * 07.05.2012
 * rsi.cpp
 */

#ifndef RSI_H
#define RSI_H

#include <QMainWindow>
#include <QSystemTrayIcon>
#include "ui_rsi.h"

namespace Ui {
class rsi;
}

class rsi : public QMainWindow, Ui::rsi
{
    Q_OBJECT
    
    public:
        rsi(QMainWindow *parent = 0);
        ~rsi();

    private slots:
        void choose_uw();
        void choose_uw2();
        void choose_muster();
        void visible();

        void startstop();

        void quit();

    private:
        void init();

        QSystemTrayIcon *trayIcon;
        QMenu *trayIconMenu;

        QAction *visibleAction;
        QAction *startstopAction;
        QAction *quitAction;

        bool running;
};

#endif // RSI_H
