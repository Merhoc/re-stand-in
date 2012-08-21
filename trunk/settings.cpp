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
 * 20.08.2012
 * settings.cpp
 */

#ifndef SETTINGS_CPP
#define SETTINGS_CPP

#include "rsi.h"
#include "standard_settings.hpp"

// Aktionen im Einstellungsfenster
// =================================================            Allgemein             =========================================
// ------------------------------------------------- Zu ueberwachende Dateien waehlen -----------------------------------------
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
// ------------------------------------------------- Datumsformat -------------------------------------------------------------
void rsi::change_dform() {
    if(!query.exec("UPDATE `settings` SET `data` = '" + input_dform->text() + "' WHERE `setting` = 'dform'")) {
        write_log(query.lastError().text());
    }
}
// ------------------------------------------------- Ueberpruefungsintervall --------------------------------------------------
void rsi::change_int() {
    if(!query.exec("UPDATE `settings` SET `data` = '" + input_int->text() + "' WHERE `setting` = 'int'")) {
        write_log(query.lastError().text());
    }
}
// =================================================        Kopf/Fuss          ================================================
// ------------------------------------------------- Header / Footer ----------------------------------------------------------
void rsi::change_header() {
    if(!query.exec("UPDATE `settings` SET `data` = '" + headText->toPlainText() + "' WHERE `setting` = 'header'")) {
        write_log(query.lastError().text());
    }
    headSave->setEnabled(false);
    headReset->setEnabled(false);
}
void rsi::mod_header() {
    headSave->setEnabled(true);
    headReset->setEnabled(true);
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
void rsi::mod_footer() {
    footSave->setEnabled(true);
    footReset->setEnabled(true);
}
void rsi::change_footer() {
    if(!query.exec("UPDATE `settings` SET `data` = '" + footText->toPlainText() + "' WHERE `setting` = 'footer'")) {
        write_log(query.lastError().text());
    }
    footSave->setEnabled(false);
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
// =================================================        Statisch        ===================================================
void rsi::change_static(QStandardItem* item) {
    // Item in Statisch-Tabelle geaendert
    if(item->text() != "<neu>") {
        if(item->row() == statmodel->rowCount() - 1) {
            // Neue Zeile
            query.exec("SELECT `rowid` FROM `static` WHERE `search` = '"  +statmodel->item(item->row(), 0)->text() + "'");
            if(query.next()) {
                write_log("Statische Aenderung nicht gespeichert: Suchwort existiert bereits!", true);
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
            // Zeile geaendert
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
// =================================================        Dynamisch       ===================================================
void rsi::change_dynamic(QStandardItem* item) {
    // Item in Dynamisch-Tabellle geaendert
    if(dynmodel->item(item->row(), 0)->text() != "<neu>") {
        if(item->row() == dynmodel->rowCount() - 1) {
            // Neue Zeile
            query.exec("SELECT `rowid` FROM `dynamic` WHERE `search` = '"  + dynmodel->item(item->row(), 0)->text() + "'");
            if(query.next()) {
                write_log("Dynamische Aenderung nicht gespeichert: Suchwort existiert bereits!", true);
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
            // Zeile geaendert
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

// Einstellungen laden:

void rsi::loadSettings() {
    if(!query.exec("SELECT `data` FROM `settings` WHERE `setting` = 'uw1'")) {
        // Standard-Datenbank erstellen
        write_log("Lade Standardeinstellungen");
        QStringList sqls = SQL_standard_settings.split("!#!");
        for (int i = 0; i < sqls.size(); ++i) {
            if(!query.exec( sqls.at(i).toLocal8Bit().constData() )) {
                write_log("Fehler beim Laden der Standarddatenbank - Einstellungen ueberpruefen!");
            }
        }
        visible();
    }
    // Werte von Eingabefeldern laden:
    reset_header();
    reset_footer();
    // Einstellungen Laden:
    write_log("Lade Einstellungen");
    query.exec("SELECT `data` FROM `settings` WHERE `setting` = 'uw1'");
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
    dynmodel->setHorizontalHeaderItem(2, new QStandardItem(tr("Zaehlen bis")));
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

#endif // SETTINGS_CPP
