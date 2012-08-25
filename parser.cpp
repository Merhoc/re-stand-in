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
 * parser.cpp
 */

#ifndef PARSER_CPP
#define PARSER_CPP

#include "rsi.h"
// ----------------------------------------------- Datei Ueberpruefen: --------------------------------------------------------
void rsi::parser1() {
    parser(getFilename(input_uw->text()));
}
void rsi::parser2() {
    parser(getFilename(input_uw_2->text(), 1));
}
void rsi::prepare_dyn_array() {
    // Array fuer dynamische Aenderungen vorbereiten:
    query.exec("SELECT `rowid` FROM `dynamic` ORDER BY `rowid` DESC LIMIT 0, 1;");
    query.next();
    int rows = query.value(query.record().indexOf("rowid")).toInt();
    for(int i = 0; i <= rows; i++) {
        phase[i] = INT_MAX;
    }
}

void rsi::parser(QString filename) {
    QFileInfo fileinfo;
    QString line;
    bool parse = false;
    fileinfo.setFile(filename);
    fileinfo.refresh();
    //Ueberpruefung auf aktualitaet, evtl. Aktualisierung
    if(fileinfo.isReadable()) {
        if(!query.exec("SELECT `lastchange` FROM `files` WHERE `filename` = '"+ filename +"'"))
            write_log(query.lastError().text(), LOG_DB);
        if(!query.next()) {
            // Noch nie verarbeitet.
            if(!query.exec("INSERT INTO `files` (`filename`, `lastchange`) VALUES ('"+filename+"', '"+fileinfo.lastModified().toString()+"')"))
                write_log(query.lastError().text(), LOG_DB);
            parse = true;
        }else{
            // Aenderung seit letzter Verarbeitung?
            if(query.value(query.record().indexOf("lastchange")) != fileinfo.lastModified().toString())
                parse = true;
        }

        // Wenn erforderlich, verarbeiten:
        if(parse) {
            rsi::prepare_dyn_array();
            // Verarbeiten:
            if(!query.exec("SELECT `data` FROM `settings` WHERE `setting` = 'header'")) {
                write_log(query.lastError().text(), LOG_DB);
            }
            query.next();
            QByteArray output = query.value(query.record().indexOf("data")).toByteArray();

            // Datei oeffnen:
            pfile.setFileName(filename);
            pfile.open(QIODevice::ReadOnly | QIODevice::Text);
            QTextStream in(&pfile);
            // Zeile fuer Zeile lesen und verarbeiten
            do {
                // Zeile lesen
                line = in.readLine();
                // Dynamische Aenderungen:
                query.exec("SELECT `rowid`, `search`, `set`, `maxval` FROM `dynamic`");
                while(query.next()) {
                    if(line.contains(query.value(query.record().indexOf("search")).toString())) {
                        if(phase[query.value(query.record().indexOf("rowid")).toInt()] >= query.value(query.record().indexOf("maxval")).toUInt()) {
                            phase[query.value(query.record().indexOf("rowid")).toInt()] = 0;
                        }else{
                            phase[query.value(query.record().indexOf("rowid")).toInt()]++;
                        }
                    }
                    line.replace(query.value(query.record().indexOf("search")).toString(), query.value(query.record().indexOf("set")).toString().arg(phase[query.value(query.record().indexOf("rowid")).toInt()]));
                }

                // Statische Aenderungen
                query.exec("SELECT `search`, `set` FROM `static`");
                while(query.next()) {
                    line.replace(query.value(query.record().indexOf("search")).toString(), query.value(query.record().indexOf("set")).toString());
                }

                // Fertige Zeile zwischenspeichern:
                output.append(line);
                output.append("\r\n");
            } while (!line.isNull());
            if(!query.exec("SELECT `data` FROM `settings` WHERE `setting` = 'footer'")) {
                write_log(query.lastError().text(), LOG_DB);
            }
            query.next();
            output += query.value(query.record().indexOf("data")).toByteArray();
            pfile.close();
            if(pfile.remove(filename)) {
                pfile.setFileName(filename);
                pfile.open(QIODevice::WriteOnly | QIODevice::Text);
                if(pfile.write(output) == -1) {
                    write_log(tr("Fehler beim Schreiben der Ausgabedatei: %1").arg(pfile.errorString()), LOG_ERROR);
                }
                pfile.close();
                fileinfo.refresh();
                if(!query.exec("UPDATE `files` SET `lastchange` = '" + fileinfo.lastModified().toString() + "' WHERE `filename` = '" + filename + "'")) {
                    write_log(query.lastError().text(), LOG_DB);
                }
                trayIcon->setIcon(QIcon(":/images/succeeded.png"));
                reseticon->start(5 * 1000); // Icon nach 5 Sekunden zuruecksetzen
                write_log(tr("%1 verarbeitet.").arg(filename), LOG_INFO);
            }else{
                write_log(tr("Konnte %1 nicht verarbeiten: Schreibzugriff nicht moeglich!").arg(filename), LOG_WARNING);
            }
        }
    }
}

#endif // PARSER_CPP
