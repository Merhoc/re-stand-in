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

rsi::rsi(QMainWindow *parent) : QMainWindow(parent) {
    setupUi(this);

    setWindowIconText("Re-stand-in");
    setWindowIcon(QIcon(":/images/icon.svg"));

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
    trayIcon->setIcon(QIcon(":/images/stopped.svg"));
    trayIcon->show();

    uwtimer1 = new QTimer(this);
    uwtimer2 = new QTimer(this);
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

    connect(uwtimer1, SIGNAL(timeout()), this, SLOT(parser1()));
    connect(uwtimer2, SIGNAL(timeout()), this, SLOT(parser2()));

    connect(crontimer, SIGNAL(timeout()), this, SLOT(cron()));

    loadSettings();                                                         // Load Settings from Database

    query.exec("SELECT `data` FROM `settings` WHERE `setting` = 'sh'");
    query.next();
    if(query.value(query.record().indexOf("data")) != "0") {
        write_log(tr("Setze von %1 fort.").arg(query.value(query.record().indexOf("data")).toString()));
    }

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
        query.exec("SELECT `setting`, `data` FROM `settings` WHERE `setting` = 'dform'");
        query.next();
        input_dform->setText(query.value(query.record().indexOf("data")).toString());
        query.exec("SELECT `setting`, `data` FROM `settings` WHERE `setting` = 'int'");
        query.next();
        input_int->setValue(query.value(query.record().indexOf("data")).toInt());

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
        query.exec("INSERT INTO `settings` (`setting`, `data`) VALUES('sh', '0')");
        query.exec("INSERT INTO `settings` (`setting`, `data`) VALUES('dform', 'MM-dd-yyyy')");
        query.exec("INSERT INTO `settings` (`setting`, `data`) VALUES('int', '5')");
        query.exec("INSERT INTO `settings` (`setting`, `data`) VALUES('header', '"
                   "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\"\r\n"
                   "       \"http://www.w3.org/TR/html4/loose.dtd\">\r\n"
                   "<html>\r\n"
                   "<head>\r\n"
                   "<title>Vertretungsplan</title>\r\n"
                   "<style type=\"text/css\" media=\"screen\">\r\n"
                   "    table {\r\n"
                   "        border-collapse: collapse;\r\n"
                   "    }\r\n"
                   "    th {\r\n"
                   "        border: 1px solid #000000;\r\n"
                   "        empty-cells: show;\r\n"
                   "    }\r\n"
                   "    td {\r\n"
                   "        border: 1px solid #000000;\r\n"
                   "        empty-cells: show;\r\n"
                   "    }\r\n"
                   "    .row0 {\r\n"
                   "        background-color: #BBBBBB;\r\n"
                   "    }\r\n"
                   "    .row1 {\r\n"
                   "        background-color: #FFFFFF;\r\n"
                   "    }\r\n"
                   "    .col0 {\r\n"
                   "        width: 3em;\r\n"
                   "    }\r\n"
                   "    .col1 {\r\n"
                   "        width: 3em;\r\n"
                   "    }\r\n"
                   "    .col2 {\r\n"
                   "        width: 2em;\r\n"
                   "    }\r\n"
                   "    .col3 {\r\n"
                   "        width: 3em;\r\n"
                   "    }\r\n"
                   "    .col4 {\r\n"
                   "        width: 7em;\r\n"
                   "    }\r\n"
                   "    .col5 {\r\n"
                   "        width: 10em;\r\n"
                   "    }\r\n"
                   "    .col6 {\r\n"
                   "        width: 10em;\r\n"
                   "    }\r\n"
                   "</style>\r\n"
                   "</head>\r\n"
                   "<body>\r\n')");
        query.exec("INSERT INTO `settings` (`setting`, `data`) VALUES('footer', '\r\n"
                   "    <hr>\r\n"
                   "</body>\r\n"
                   "</html>')");

        if(!query.exec("CREATE TABLE `files` ("
                   "    `filename`   VARCHAR(128) NOT NULL,"
                   "    `lastchange` VARCHAR(128) NOT NULL"
                   ")")) {
            write_log(query.lastError().text());
        }
        if(!query.exec("CREATE TABLE `static` ("
                   "    `search`   TEXT NOT NULL,"
                   "    `set` TEXT NOT NULL"
                   ")")) {
            write_log(query.lastError().text());
        }
        query.exec("INSERT INTO `static` (`search`, `set`) VALUES('<p><big><b>', '<h1>')");
        query.exec("INSERT INTO `static` (`search`, `set`) VALUES('</b></big><br>', '</h1>')");
        query.exec("INSERT INTO `static` (`search`, `set`) VALUES('ä', '&auml;')");
        query.exec("INSERT INTO `static` (`search`, `set`) VALUES('ü', '&uuml;')");
        query.exec("INSERT INTO `static` (`search`, `set`) VALUES('ö', '&ouml;')");
        query.exec("INSERT INTO `static` (`search`, `set`) VALUES('Ä', '&Auml;')");
        query.exec("INSERT INTO `static` (`search`, `set`) VALUES('Ü', '&Uuml;')");
        query.exec("INSERT INTO `static` (`search`, `set`) VALUES('Ö', '&Ouml;')");
        query.exec("INSERT INTO `static` (`search`, `set`) VALUES('ß', '&szlig;')");
        if(!query.exec("CREATE TABLE `dynamic` ("
                   "    `search`   TEXT NOT NULL,"
                   "    `set` TEXT NOT NULL,"
                   "    `maxval` TINYINT NOT NULL"
                   ")")) {
            write_log(query.lastError().text());
        }
        query.exec("INSERT INTO `dynamic` (`search`, `set`, `maxval`) VALUES('<tr>', '<tr class=\"row%1\">', '1')");
        query.exec("INSERT INTO `dynamic` (`search`, `set`, `maxval`) VALUES('<th>', '<th class=\"col%1\">', '6')");
        visible();
    }
    // Werte von Eingabefeldern laden:
    reset_header();
    reset_footer();
    // Tabellen:
    query.exec("SELECT `search`, `set`, `rowid` FROM `static`");
    statrows = query.size() + 1;
    statmodel = new QStandardItemModel(statrows, 2, this);
    statmodel->setHorizontalHeaderItem(0, new QStandardItem(tr("Suchen")));
    statmodel->setHorizontalHeaderItem(1, new QStandardItem(tr("Ersetzen")));
    row = 0;
    while(query.next()) {
        statmodel->setItem(row, 0, new QStandardItem(query.value(query.record().indexOf("search")).toString()));
        statmodel->setItem(row, 1, new QStandardItem(query.value(query.record().indexOf("set")).toString()));
        statmodel->setItem(row, 2, new QStandardItem(query.value(query.record().indexOf("rowid")).toString()));
        statmodel->item(row, 2)->setEnabled(false);
        row ++;
    }
    statmodel->setItem(row, 0, new QStandardItem("<neu>"));
    statmodel->setItem(row, 1, new QStandardItem("<neu>"));
    table_static->setModel(statmodel);
    table_static->setColumnWidth(0, 250);
    table_static->setColumnWidth(1, 340);
    table_static->setColumnWidth(2, 0);

    query.exec("SELECT `search`, `set`, `maxval`, `rowid` FROM `dynamic`");
    dynrows = query.size() + 1;
    dynmodel = new QStandardItemModel(dynrows, 3, this);
    dynmodel->setHorizontalHeaderItem(0, new QStandardItem(tr("Suchen")));
    dynmodel->setHorizontalHeaderItem(1, new QStandardItem(tr("Ersetzen")));
    dynmodel->setHorizontalHeaderItem(2, new QStandardItem(tr("Zählen bis")));
    row = 0;
    while(query.next()) {
        dynmodel->setItem(row, 0, new QStandardItem(query.value(query.record().indexOf("search")).toString()));
        dynmodel->setItem(row, 1, new QStandardItem(query.value(query.record().indexOf("set")).toString()));
        dynmodel->setItem(row, 2, new QStandardItem(query.value(query.record().indexOf("maxval")).toString()));
        dynmodel->setItem(row, 3, new QStandardItem(query.value(query.record().indexOf("rowid")).toString()));
        dynmodel->item(row, 3)->setEnabled(false);
        row ++;
    }
    dynmodel->setItem(row, 0, new QStandardItem("<neu>"));
    dynmodel->setItem(row, 1, new QStandardItem("<neu>"));
    dynmodel->setItem(row, 2, new QStandardItem("1"));
    table_dynamic->setModel(dynmodel);
    table_dynamic->setColumnWidth(0, 200);
    table_dynamic->setColumnWidth(1, 300);
    table_dynamic->setColumnWidth(2, 70);
    table_dynamic->setColumnWidth(3, 0);
    connect(statmodel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(change_static(QStandardItem*)));
    connect(dynmodel, SIGNAL(itemChanged(QStandardItem*)), this, SLOT(change_dynamic(QStandardItem*)));
}

// Aktionen im Einstellungsfenster
void rsi::choose_uw() {
    input_uw->setText(QFileDialog::getOpenFileName(this, tr("Datei waehlen..."), "", tr("HTML Dateien (*.htm *.html)")));
}
void rsi::change_uw() {
    query.exec("UPDATE `settings` SET `data` = '" + input_uw->text() + "' WHERE `setting` = 'uw1'");
    query.exec("UPDATE `settings` SET `data` = '0' WHERE `setting` = 'proc_uw1'");
}
void rsi::choose_uw2() {
    input_uw_2->setText(QFileDialog::getOpenFileName(this, tr("Datei waehlen..."), "", tr("HTML Dateien (*.htm *.html)")));
}
void rsi::change_uw2() {
    query.exec("UPDATE `settings` SET `data` = '" + input_uw_2->text() + "' WHERE `setting` = 'uw2'");
    query.exec("UPDATE `settings` SET `data` = '0' WHERE `setting` = 'proc_uw2'");
}
void rsi::change_dform() {
    if(!query.exec("UPDATE `settings` SET `data` = '" + input_dform->text() + "' WHERE `setting` = 'dform'")) {
        write_log(query.lastError().text());
    }
}
void rsi::change_int() {
    if(!query.exec("UPDATE `settings` SET `data` = '" + input_int->text() + "' WHERE `setting` = 'int'")) {
        write_log(query.lastError().text());
    }
}
void rsi::change_header() {
    if(!query.exec("UPDATE `settings` SET `data` = '" + headText->toPlainText() + "' WHERE `setting` = 'header'")) {
        write_log(query.lastError().text());
    }
    headSave->setEnabled(false);
    headReset->setEnabled(false);
}
void rsi::change_footer() {
    if(!query.exec("UPDATE `settings` SET `data` = '" + footText->toPlainText() + "' WHERE `setting` = 'footer'")) {
        write_log(query.lastError().text());
    }
    footSave->setEnabled(false);
    headReset->setEnabled(false);
}
void rsi::mod_header() {
    headSave->setEnabled(true);
    headReset->setEnabled(true);
}
void rsi::mod_footer() {
    footSave->setEnabled(true);
    footReset->setEnabled(true);
}
void rsi::reset_header() {
    if(!query.exec("SELECT `data` FROM `settings` WHERE `setting` = 'header'")) {
        write_log(query.lastError().text());
    }
    query.next();
    headText->clear();
    headText->appendPlainText(query.value(query.record().indexOf("data")).toByteArray());
    headSave->setEnabled(false);
    headReset->setEnabled(false);
}
void rsi::reset_footer() {
    if(!query.exec("SELECT `data` FROM `settings` WHERE `setting` = 'footer'")) {
        write_log(query.lastError().text());
    }
    query.next();
    footText->clear();
    footText->appendPlainText(query.value(query.record().indexOf("data")).toByteArray());
    footSave->setEnabled(false);
    footReset->setEnabled(false);
}
void rsi::change_static(QStandardItem* item) {
    // Item in Statisch-Tabelle geändert
    if(item->text() != "<neu>") {
        if(item->row() == statmodel->rowCount() - 1) {
            // Neue Zeile
            query.exec("SELECT `rowid` FROM `static` WHERE `search` = '"  +statmodel->item(item->row(), 0)->text() + "'");
            if(query.next()) {
                write_log("Statische Änderung nicht gespeichert: Suchwort existiert bereits!", true);
            }else{
                if(!query.exec("INSERT INTO `static` (`search`, `set`) VALUES('" + statmodel->item(item->row(), 0)->text() + "', '" + statmodel->item(item->row(), 1)->text() + "')")) {
                    write_log(query.lastError().text());
                }
                query.exec("SELECT `rowid` FROM `static` WHERE `search` = '"  + statmodel->item(item->row(), 0)->text() + "'");
                query.next();
                statmodel->setItem(item->row() + 1, 0, new QStandardItem("<neu>"));
                statmodel->setItem(item->row() + 1, 1, new QStandardItem("<neu>"));
                statmodel->setItem(item->row(), 2, new QStandardItem(query.value(query.record().indexOf("rowid")).toString()));
                statmodel->item(item->row(), 2)->setEnabled(false);
            }
        }else{
            // Zeile geändert
            if(statmodel->item(item->row(), 0)->text() == "!") {
                query.exec("DELETE FROM `static` WHERE `rowid` = '"  + statmodel->item(item->row(), 2)->text() + "'");
                statmodel->removeRow(item->row());
            }else{
                query.exec("UPDATE `static` SET "
                           "    `search` = '" + statmodel->item(item->row(), 0)->text() + "', "
                           "    `set` = '"  +statmodel->item(item->row(), 1)->text() + "'"
                           "WHERE `rowid` = '"  + statmodel->item(item->row(), 2)->text() + "'");
            }
        }
    }
}
void rsi::change_dynamic(QStandardItem* item) {
    // Item in Dynamisch-Tabellle geändert
    if(dynmodel->item(item->row(), 0)->text() != "<neu>") {
        if(item->row() == dynmodel->rowCount() - 1) {
            // Neue Zeile
            query.exec("SELECT `rowid` FROM `dynamic` WHERE `search` = '"  + dynmodel->item(item->row(), 0)->text() + "'");
            if(query.next()) {
                write_log("Dynamische Änderung nicht gespeichert: Suchwort existiert bereits!", true);
            }else{
                if(!query.exec("INSERT INTO `dynamic` (`search`, `set`, `maxval`) VALUES("
                           "    '" + dynmodel->item(item->row(), 0)->text() + "',"
                           "    '" + dynmodel->item(item->row(), 1)->text() + "',"
                           "    '" + dynmodel->item(item->row(), 2)->text() + "')")) {
                    write_log(query.lastError().text());
                }
                query.exec("SELECT `rowid` FROM `dynamic` WHERE `search` = '"  + dynmodel->item(item->row(), 0)->text() + "'");
                query.next();
                dynmodel->setItem(item->row() + 1, 0, new QStandardItem("<neu>"));
                dynmodel->setItem(item->row() + 1, 1, new QStandardItem("<neu>"));
                dynmodel->setItem(item->row() + 1, 2, new QStandardItem("1"));
                dynmodel->setItem(item->row(), 3, new QStandardItem(query.value(query.record().indexOf("rowid")).toString()));
                dynmodel->item(item->row(), 3)->setEnabled(false);
            }
        }else{
            // Zeile geändert
            if(dynmodel->item(item->row(), 0)->text() == "!") {
                query.exec("DELETE FROM `dynamic` WHERE `rowid` = '"  + dynmodel->item(item->row(), 3)->text() + "'");
                dynmodel->removeRow(item->row());
            }else{
                query.exec("UPDATE `dynamic` SET "
                           "    `search` = '" + dynmodel->item(item->row(), 0)->text() + "',"
                           "    `set` = '"  + dynmodel->item(item->row(), 1)->text() + "',"
                           "    `maxval` = '" + dynmodel->item(item->row(), 2)->text() + "'"
                           "WHERE `rowid` = '"  + dynmodel->item(item->row(), 3)->text() + "'");
            }
        }
    }
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
    if(running) {
        // Derzeit verarbeitete Dateien unangetastet lassen:
        QString sql = "DELETE FROM `files` WHERE"
                "   `filename` != '" + input_uw->text() + "' AND"
                "   `filename` != '" + input_uw_2->text() + "'";
        if(!query.exec(sql)) {
            write_log(query.lastError().text());
        }
    }else{
        if(!query.exec("DELETE FROM `files` WHERE 1")) {
            write_log(query.lastError().text());
        }
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
                write_log("Achtung: Zu überwachende Datei 1 existiert nicht! (" + getFilename(input_uw->text()) + ")");
            uwtimer1->start(input_int->value() * 1000);
            parser1();
        }else{
            write_log("Überwachung 1 nicht aktiv (keine Datei angegeben).");
        }
        if(input_uw_2->text() != "") {
            uwinfo2.setFile(getFilename(input_uw_2->text(), 1));
            if(!uwinfo2.exists())
                write_log("Achtung: Zu überwachende Datei 2 existiert nicht! (" + getFilename(input_uw_2->text(), 1) + ")");
            uwtimer2->start(input_int->value() * 1000);
            parser2();
        }else{
            write_log("Überwachung 2 nicht aktiv (keine Datei angegeben).");
        }
        write_log("Dienst gestartet.", true);
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
        write_log("Dienst gestoppt.", true);
    }
    if(writeSettings) {
        QString sql = "UPDATE `settings` SET `data` = '";
        sql += running?"1":"0";
        sql += "' WHERE `setting` = 'active'";
        query.exec(sql);
    }

    trayIcon->setIcon(running?QIcon(":/images/icon.svg"):QIcon(":/images/stopped.svg"));

    checkbox_active->setChecked(running);
}

void rsi::tray_clicked(QSystemTrayIcon::ActivationReason reason) {
    if(reason != QSystemTrayIcon::Context) {
        show();
    }
}

// Private Funktionen:
void rsi::write_log(QString message, bool tray) {
    log->appendPlainText(QDateTime::currentDateTime().toString() + ": " + message);
    if(tray)
        trayIcon->showMessage(tr("re-stand-in"), message, QSystemTrayIcon::Information, 5000);
}
QString rsi::getFilename(QString raw, int offset) {
    if(raw.contains("%1"))
        return raw.arg(QDateTime::currentDateTime().addDays(offset).toString(input_dform->text()));
    else
        return raw;
}

// Datei Ueberpruefen:
void rsi::parser1() {
    parser(getFilename(input_uw->text()));
}
void rsi::parser2() {
    parser(getFilename(input_uw_2->text(), 1));
}

void rsi::parser(QString filename) {
    QFileInfo fileinfo;
    QString line;
    QByteArray output;
    bool parse = false;
    fileinfo.setFile(filename);
    //Überprüfung auf aktualität, evtl. Aktualisierung
    if(fileinfo.isReadable()) {
        if(!query.exec("SELECT `lastchange` FROM `files` WHERE `filename` = '"+ filename +"'")) {
            write_log(query.lastError().text());
        }
        if(!query.next()) {
            // Noch nie verarbeitet.
            if(!query.exec("INSERT INTO `files` (`filename`, `lastchange`) VALUES ('"+filename+"', '"+fileinfo.lastModified().toString()+"')")) {
                write_log(query.lastError().text());
            }
            parse = true;
        }else{
            if(query.value(query.record().indexOf("lastchange")) != fileinfo.lastModified().toString()) {
                parse = true;
            }
        }
        if(parse) {
            // Array fuer dynamischen Inhalt vorbereiten:
            query.exec("SELECT `rowid` FROM `dynamic`");
            int rows = 0;
            while(query.next()) {
                rows = query.value(query.record().indexOf("rowid")).toInt();
            }
            unsigned int phase[rows];
            for(int i = 0; i < rows; i++) {
                phase[i] = 0;
            }
            // Verarbeiten:
            if(!query.exec("SELECT `data` FROM `settings` WHERE `setting` = 'header'")) {
                write_log(query.lastError().text());
            }
            query.next();
            output = query.value(query.record().indexOf("data")).toByteArray();

            pfile.setFileName(filename);
            pfile.open(QIODevice::ReadOnly | QIODevice::Text);
            QTextStream in(&pfile);
            do {
                line = in.readLine();
                query.exec("SELECT `rowid`, `search`, `set`, `maxval` FROM `dynamic`");
                while(query.next()) {
                    if(line.contains(query.value(query.record().indexOf("search")).toString())) {
                        if(phase[query.value(query.record().indexOf("rowid")).toInt()] == query.value(query.record().indexOf("maxval"))) {
                            phase[query.value(query.record().indexOf("rowid")).toInt()] = 0;
                        }else{
                            phase[query.value(query.record().indexOf("rowid")).toInt()]++;
                        }
                    }
                    line.replace(query.value(query.record().indexOf("search")).toString(), query.value(query.record().indexOf("set")).toString().arg(phase[query.value(query.record().indexOf("rowid")).toInt()]));
                }
                query.exec("SELECT `search`, `set` FROM `static`");
                while(query.next()) {
                    line.replace(query.value(query.record().indexOf("search")).toString(), query.value(query.record().indexOf("set")).toString());
                }
                output += line + tr("\r\n");
            } while (!line.isNull());
            if(!query.exec("SELECT `data` FROM `settings` WHERE `setting` = 'footer'")) {
                write_log(query.lastError().text());
            }
            query.next();
            output += query.value(query.record().indexOf("data")).toByteArray();
            pfile.close();
            pfile.remove(filename);
            pfile.setFileName(filename);
            pfile.open(QIODevice::WriteOnly | QIODevice::Text);
            if(pfile.write(output) == -1) {
                write_log("Fehler beim Schreiben der Ausgabedatei: "+pfile.errorString());
            }
            pfile.close();
            fileinfo.refresh();
            if(!query.exec("UPDATE `files` SET `lastchange` = '"+fileinfo.lastModified().toString()+"' WHERE `filename` = '"+filename+"'")) {
                write_log(query.lastError().text());
            }
            write_log(filename + " verarbeitet.");
        }
    }
}
