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
 * log.cpp
 */

#ifndef LOG_CPP
#define LOG_CPP

#include "rsi.h"

void rsi::write_log(QString message, unsigned short level) {
    log->appendPlainText(QDateTime::currentDateTime().toString() + ": [" + logsym[level] + "] " + message);
}

void rsi::create_report() {
    if(reportComment->toPlainText() == tr("Kommentar erstellen...")) {
        reportComment->setPlainText("");
    }
}

void rsi::send_report() {
    sendReport->setDisabled(true);
    if(checkbox_active->isChecked())
        startstop();
    QNetworkRequest request(QUrl("http://www.heikometzger.de/re-stand-in-report.php"));
    request.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded");

    QByteArray data;
    QUrl params;

    params.addQueryItem("mail", input_email->text());
    params.addQueryItem("log", log->toPlainText());
    params.addQueryItem("comment", reportComment->toPlainText());
    data = params.encodedQuery();

    netman.post(request, data);

    write_log("Bericht gesendet.", LOG_INFO);
    sendReport->setText("Hochgeladen.");
}

#endif // LOG_CPP
