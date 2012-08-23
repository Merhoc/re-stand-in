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

#define RSI_REV "Revision 40"

#include "rsi.h"

rsi::rsi(QMainWindow *parent) : QMainWindow(parent) {
    setupUi(this);

    logsym[0] = tr("II");
    logsym[1] = tr("WW");
    logsym[2] = tr("DB");
    logsym[3] = tr("EE");

    write_log("re-stand-in startet.", LOG_INFO);
    write_log(RSI_REV, LOG_INFO);

    setWindowIconText("Re-stand-in");
    setWindowIcon(QIcon(":/images/icon.png"));

    running = false;

    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("rsi.SQLite3");
    if(!db.open()) {
        QMessageBox::critical(0, tr("Kann Einstellungen nicht lesen/speichern!"),
                              tr("Ein Problem mit der Datenbank ist aufgetreten!")+db.lastError().text(),
                              QMessageBox::Cancel, QMessageBox::NoButton);
        quit();
    }
    query = QSqlQuery(db);

    trayIconMenu = new QMenu(this);
    visibleAction = new QAction(tr("Fenster Zeigen"), this);

    trayIconMenu->addAction(visibleAction);
    startstopAction = new QAction(tr("Start/Stopp"), this);
    trayIconMenu->addAction(startstopAction);
    trayIconMenu->addSeparator();
    quitAction = new QAction(tr("Beenden"), this);
    trayIconMenu->addAction(quitAction);

    trayIcon = new QSystemTrayIcon(this);
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(QIcon(":/images/stopped.png"));
    trayIcon->setToolTip("re-stand-in");
    trayIcon->show();

    uwtimer1 = new QTimer(this);
    uwtimer2 = new QTimer(this);
    reseticon = new QTimer(this);
    crontimer = new QTimer(this);
    crontimer->start(24 * 60 * 60 * 1000);                       // Alle 24h Reinigungsarbeiten ausfuehren

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(tray_clicked(QSystemTrayIcon::ActivationReason)));
    connect(visibleAction, SIGNAL(triggered()), this, SLOT(visible()));
    connect(startstopAction, SIGNAL(triggered()), this, SLOT(startstop()));
    connect(quitAction, SIGNAL(triggered()), this, SLOT(quit()));

    connect(checkbox_active, SIGNAL(clicked()), this, SLOT(startstop()));
    connect(button_ok, SIGNAL(clicked()), this, SLOT(visible()));
    connect(button_quit, SIGNAL(clicked()), this, SLOT(quit()));

    connect(button_uw, SIGNAL(clicked()), this, SLOT(choose_uw()));
    connect(input_uw, SIGNAL(textChanged(QString)), this, SLOT(change_uw()));
    connect(button_uw_2, SIGNAL(clicked()), this, SLOT(choose_uw2()));
    connect(input_uw_2, SIGNAL(textChanged(QString)), this, SLOT(change_uw2()));
    connect(input_dform, SIGNAL(textChanged(QString)), this, SLOT(change_dform()));
    connect(input_int, SIGNAL(valueChanged(int)), this, SLOT(change_int()));

    connect(headText, SIGNAL(textChanged()), this, SLOT(mod_header()));
    connect(footText, SIGNAL(textChanged()), this, SLOT(mod_footer()));
    connect(headSave, SIGNAL(clicked()), this, SLOT(change_header()));
    connect(footSave, SIGNAL(clicked()), this, SLOT(change_footer()));
    connect(headReset, SIGNAL(clicked()), this, SLOT(reset_header()));
    connect(footReset, SIGNAL(clicked()), this, SLOT(reset_footer()));

    connect(reportComment, SIGNAL(cursorPositionChanged()), this, SLOT(create_report()));
    connect(sendReport, SIGNAL(clicked()), this, SLOT(send_report()));

    connect(uwtimer1, SIGNAL(timeout()), this, SLOT(parser1()));
    connect(uwtimer2, SIGNAL(timeout()), this, SLOT(parser2()));

    connect(reseticon, SIGNAL(timeout()), this, SLOT(do_reseticon()));
    connect(crontimer, SIGNAL(timeout()), this, SLOT(cron()));

    loadSettings();                                                         // Load Settings from Database
    cron();

    query.exec("SELECT `data` FROM `settings` WHERE `setting` = 'sh'");
    query.next();
    if(query.value(query.record().indexOf("data")) != "0") {
        write_log(tr("Setze von %1 fort.").arg(query.value(query.record().indexOf("data")).toString()), LOG_INFO);
    }

    write_log("Initialisierung abgeschlossen.", LOG_INFO);
}

rsi::~rsi()
{
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
    query.exec("UPDATE `settings` SET `data` = '" + QDateTime::currentDateTime().toString() + "' WHERE `setting` = 'sh'");
    QApplication::quit();
}

void rsi::cron() {
    // Datenbankbereinigung:
    QString sql = "DELETE FROM `files` WHERE"
            "   `filename` != '" + input_uw->text() + "' AND"
            "   `filename` != '" + input_uw_2->text() + "'";
    if(!query.exec(sql)) {
        write_log(query.lastError().text(), LOG_DB);
    }
}

// Aktionen im TrayIconMenu
void rsi::startstop(bool writeSettings) {
    cron();

    running = !running;

    if(running) {
        // Ueberwachung starten:
        input_uw->setEnabled(false);
        button_uw->setEnabled(false);
        input_uw_2->setEnabled(false);
        button_uw_2->setEnabled(false);
        input_dform->setEnabled(false);
        input_int->setEnabled(false);
        headText->setEnabled(false);
        footText->setEnabled(false);
        table_static->setEnabled(false);
        table_dynamic->setEnabled(false);
        if(input_uw->text() != "") {
            uwinfo1.setFile(getFilename(input_uw->text()));
            if(!uwinfo1.exists())
                write_log("Achtung: Zu Ueberwachende Datei 1 existiert nicht! (" + getFilename(input_uw->text()) + ")", LOG_WARNING);
            uwtimer1->start(input_int->value() * 1000);
            parser1();
        }else{
            write_log("Ueberwachung 1 nicht aktiv (keine Datei angegeben).", LOG_WARNING);
        }
        if(input_uw_2->text() != "") {
            uwinfo2.setFile(getFilename(input_uw_2->text(), 1));
            if(!uwinfo2.exists())
                write_log("Achtung: Zu Ueberwachende Datei 2 existiert nicht! (" + getFilename(input_uw_2->text(), 1) + ")", LOG_WARNING);
            uwtimer2->start(input_int->value() * 1000);
            parser2();
        }else{
            write_log("Ueberwachung 2 nicht aktiv (keine Datei angegeben).", LOG_WARNING);
        }
        write_log("Dienst gestartet.", LOG_INFO);
    }else{
        // Ueberwachung stoppen:
        uwtimer1->stop();
        uwtimer2->stop();
        input_uw->setEnabled(true);
        button_uw->setEnabled(true);
        input_uw_2->setEnabled(true);
        button_uw_2->setEnabled(true);
        input_dform->setEnabled(true);
        input_int->setEnabled(true);
        headText->setEnabled(true);
        footText->setEnabled(true);
        table_static->setEnabled(true);
        table_dynamic->setEnabled(true);
        write_log("Dienst gestoppt.", LOG_INFO);
    }
    if(writeSettings) {
        QString sql = "UPDATE `settings` SET `data` = '";
        sql += running?"1":"0";
        sql += "' WHERE `setting` = 'active'";
        query.exec(sql);
    }

    trayIcon->setIcon(running?QIcon(":/images/running.png"):QIcon(":/images/stopped.png"));

    checkbox_active->setChecked(running);
}

void rsi::tray_clicked(QSystemTrayIcon::ActivationReason reason) {
    if(reason != QSystemTrayIcon::Context) {
        show();
    }
}

void rsi::do_reseticon() {
    trayIcon->setIcon(running?QIcon(":/images/running.png"):QIcon(":/images/stopped.png"));
    reseticon->stop();
}

// Private Funktionen:
QString rsi::getFilename(QString raw, int offset) {
    if(raw.contains("%1"))
        return raw.arg(QDateTime::currentDateTime().addDays(offset).toString(input_dform->text()));
    else
        return raw;
}

#include "settings.cpp"
#include "parser.cpp"
