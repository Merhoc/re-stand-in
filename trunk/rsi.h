/*
 * Copyright (C) 2012 Heiko Metzger <www.heikometzger.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
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
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
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
        void write_log(QString message);

    private slots:
        void choose_uw();
        void choose_uw2();
        void visible();

        void startstop(bool writeSettings = true);

        void quit();

    private:
        QSystemTrayIcon *trayIcon;
        QMenu *trayIconMenu;

        QAction *visibleAction;
        QAction *startstopAction;
        QAction *quitAction;

        QSqlDatabase db;
        QSqlQuery query;

        bool running;

        void loadSettings();
};

#endif // RSI_H
