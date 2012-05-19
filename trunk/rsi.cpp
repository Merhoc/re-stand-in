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

rsi::rsi(QMainWindow *parent) : QMainWindow(parent) {
    setupUi(this);

    setWindowIconText("Re-stand-in");
    setWindowIcon(QIcon(":/images/icon.svg"));

    running = false;

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
    connect(button_muster, SIGNAL(clicked()), this, SLOT(choose_muster()));
    connect(button_ok, SIGNAL(clicked()), this, SLOT(visible()));
    connect(button_quit, SIGNAL(clicked()), this, SLOT(quit()));

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(visible()));

    write_log("Initialisierung abgeschlossen.");
}

rsi::~rsi()
{
}

// Aktionen im Einstellungsfenster
void rsi::choose_uw() {
    input_uw->insert(QFileDialog::getOpenFileName(this, tr("Datei waehlen..."), "", tr("HTML Dateien (*.htm *.html)")));
}
void rsi::choose_uw2() {
    input_uw_2->insert(QFileDialog::getOpenFileName(this, tr("Datei waehlen..."), "", tr("HTML Dateien (*.htm *.html)")));
}
void rsi::choose_muster() {
    input_muster->insert(QFileDialog::getOpenFileName(this, tr("Datei oeffnen..."), "", tr("Konfiguration (*.conf)")));
}

// Mehrfach genutzte Funktionen
void rsi::visible() {
    this->setVisible(!this->isVisible());
    this->activateWindow();
    this->raise();
}

void rsi::quit() {
    QApplication::quit();
}

// Aktionen im TrayIconMenu
void rsi::startstop() {
    running = !running;

    trayIcon->showMessage(tr("re-stand-in"), running?tr("Dienst gestartet."):tr("Dienst gestoppt."));
    trayIcon->setIcon(running?QIcon(":/images/icon.svg"):QIcon(":/images/stopped.svg"));

    checkbox_active->setChecked(running);
}

// Private Funktionen:
void rsi::write_log(QString message) {
    log->appendPlainText(QDateTime::currentDateTime().toString() + ": " + message);
}
