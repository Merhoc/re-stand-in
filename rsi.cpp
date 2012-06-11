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

#include "rsi.h"
#include <QtGui>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlResult>
#include <QtSql/QSqlError>

rsi::rsi(QMainWindow *parent) : QMainWindow(parent) {
    setupUi(this);

    setWindowIconText("Re-stand-in");
    setWindowIcon(QIcon(":/images/icon.svg"));

    running = false;

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":settings:");
    if(!db.open()) {
        QMessageBox::critical(0, tr("Kann Einstellungen nicht lesen/speichern!"),
                              tr("Ein Problem mit der Datenbank ist aufgetreten!"),
                              QMessageBox::Cancel, QMessageBox::NoButton);
        this->quit();
    }
    query = QSqlQuery(db);

    trayIconMenu = new QMenu(this);
    visibleAction = new QAction(tr("Fenster Zeigen"), this);
    connect(visibleAction, SIGNAL(triggered()), this, SLOT(visible()));
    trayIconMenu->addAction(visibleAction);
    startstopAction = new QAction(tr("Start/Stopp"), this);
    connect(startstopAction, SIGNAL(triggered()), this, SLOT(startstop()));
    trayIconMenu->addAction(startstopAction);

    trayIconMenu->addSeparator();
    quitAction = new QAction(tr("Beenden"), this);
    connect(quitAction, SIGNAL(triggered()), this, SLOT(quit()));
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(QIcon(":/images/stopped.svg"));
    trayIcon->show();

    connect(checkbox_active, SIGNAL(clicked()), this, SLOT(startstop()));
    connect(button_uw, SIGNAL(clicked()), this, SLOT(choose_uw()));
    connect(button_uw_2, SIGNAL(clicked()), this, SLOT(choose_uw2()));
    connect(button_ok, SIGNAL(clicked()), this, SLOT(visible()));
    connect(button_quit, SIGNAL(clicked()), this, SLOT(quit()));

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(visible()));

    loadSettings();                                                         // Load Settings from Database

    write_log("Initialisierung abgeschlossen.");
}

rsi::~rsi()
{
}

void rsi::loadSettings() {
    if(query.exec("SELECT `data` FROM `settings` WHERE `setting` = 'uw1'")) {
        // Einstellungen Laden:
        write_log("Lade Einstellungen");
        query.next();
        input_uw->setText(query.value(query.record().indexOf("data")).toString());
        query.exec("SELECT `setting`, `data` FROM `settings` WHERE `setting` = 'uw2'");
        query.next();
        input_uw_2->setText(query.value(query.record().indexOf("data")).toString());

        query.exec("SELECT `setting`, `data` FROM `settings` WHERE `setting` = 'active'");
        query.next();
        if(query.value(query.record().indexOf("data")).toBool()) {
            startstop();
        }
    }else{
        // Standardeinstellungen Schreiben
        write_log("Lade Standardeinstellungen");
        if(!query.exec("CREATE TABLE `settings` ("
                   "    `setting`   VARCHAR(10) NOT NULL,"
                   "    `data`      TEXT NOT NULL"
                   ")")) {
            write_log(query.lastError().text());
        }
        query.exec("INSERT INTO `settings` (`setting`, `data`) VALUES('uw1', '')");
        query.exec("INSERT INTO `settings` (`setting`, `data`) VALUES('uw2', '')");
        query.exec("INSERT INTO `settings` (`setting`, `data`) VALUES('active', '0')");
    }
}

// Aktionen im Einstellungsfenster
void rsi::choose_uw() {
    input_uw->setText(QFileDialog::getOpenFileName(this, tr("Datei waehlen..."), "", tr("HTML Dateien (*.htm *.html)")));
    query.exec("UPDATE `settings` SET `data` = '" + input_uw->text() + "' WHERE `setting` = 'uw1'");
}
void rsi::choose_uw2() {
    input_uw_2->setText(QFileDialog::getOpenFileName(this, tr("Datei waehlen..."), "", tr("HTML Dateien (*.htm *.html)")));
    query.exec("UPDATE `settings` SET `data` = '" + input_uw_2->text() + "' WHERE `setting` = 'uw2'");
}

// Mehrfach genutzte Funktionen
void rsi::visible() {
    this->setVisible(!this->isVisible());
    this->activateWindow();
    this->raise();
}

void rsi::quit() {
    if(running) {
        startstop(false);
    }
    QApplication::quit();
}

// Aktionen im TrayIconMenu
void rsi::startstop(bool writeSettings) {
    running = !running;

    if(writeSettings) {
        QString sql = "UPDATE `settings` SET `data` = '";
        sql += running?"1":"0";
        sql += "' WHERE `setting` = 'active'";
        query.exec(sql);
    }

    write_log(running?tr("Dienst gestartet."):tr("Dienst gestoppt."));
    trayIcon->setIcon(running?QIcon(":/images/icon.svg"):QIcon(":/images/stopped.svg"));

    checkbox_active->setChecked(running);
}

// Private Funktionen:
void rsi::write_log(QString message) {
    log->appendPlainText(QDateTime::currentDateTime().toString() + ": " + message);
    trayIcon->showMessage(tr("re-stand-in"), message, QSystemTrayIcon::Information, 5000);
}
