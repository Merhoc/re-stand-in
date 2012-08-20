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

void rsi::parser(QString filename) {
    QFileInfo fileinfo;
    QString line;
    QByteArray output;
    bool parse = false;
    fileinfo.setFile(filename);
    fileinfo.refresh();
    //Ueberpruefung auf aktualitaet, evtl. Aktualisierung
    if(fileinfo.isReadable()) {
        if(!query.exec("SELECT `lastchange` FROM `files` WHERE `filename` = '"+ filename +"'"))
            write_log(query.lastError().text());
        if(!query.next()) {
            // Noch nie verarbeitet.
            if(!query.exec("INSERT INTO `files` (`filename`, `lastchange`) VALUES ('"+filename+"', '"+fileinfo.lastModified().toString()+"')"))
                write_log(query.lastError().text());
            parse = true;
        }else{
            // Aenderung seit letzter Verarbeitung?
            if(query.value(query.record().indexOf("lastchange")) != fileinfo.lastModified().toString())
                parse = true;
        }

        // Wenn erforderlich, verarbeiten:
        if(parse) {
            // Array fuer dynamische Aenderungen vorbereiten:
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
                        if(phase[query.value(query.record().indexOf("rowid")).toInt()] == query.value(query.record().indexOf("maxval"))) {
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
                output.append(tr("\r\n"));
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
            trayIcon->setIcon(QIcon(":/images/succeeded.png"));
            reseticon->start(5 * 1000); // Icon nach 5 Sekunden zuruecksetzen
            write_log(filename + " verarbeitet.");
        }
    }
}

#endif // PARSER_CPP
