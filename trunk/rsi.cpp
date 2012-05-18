/*
 * re-stand-in
 *
 * Heiko Metzger
 * 07.05.2012
 * rsi.cpp
 */

#include "rsi.h"
#include <QtGui>
#include <QFileDialog>

rsi::rsi(QMainWindow *parent) : QMainWindow(parent) {
    setupUi(this);

    init();

    connect(button_uw, SIGNAL(clicked()), this, SLOT(choose_uw()));
    connect(button_uw_2, SIGNAL(clicked()), this, SLOT(choose_uw2()));
    connect(button_muster, SIGNAL(clicked()), this, SLOT(choose_muster()));
    connect(button_ok, SIGNAL(clicked()), this, SLOT(visible()));
    connect(button_quit, SIGNAL(clicked()), this, SLOT(quit()));

    connect(trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)), this, SLOT(visible()));
}

rsi::~rsi()
{
}

void rsi::choose_uw() {
    input_uw->insert(QFileDialog::getOpenFileName(this, tr("Datei waehlen..."), "", tr("HTML Dateien (*.htm *.html)")));
}
void rsi::choose_uw2() {
    input_uw_2->insert(QFileDialog::getOpenFileName(this, tr("Datei waehlen..."), "", tr("HTML Dateien (*.htm *.html)")));
}
void rsi::choose_muster() {
    input_muster->insert(QFileDialog::getOpenFileName(this, tr("Datei oeffnen..."), "", tr("Konfiguration (*.conf)")));
}

void rsi::visible() {
    this->setVisible(!this->isVisible());
}

void rsi::quit() {
    QApplication::quit();
}

void rsi::init() {
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
}

void rsi::startstop() {
    running = !running;

    trayIcon->showMessage(tr("Start/Stopp"), running?tr("Dienst gestartet."):tr("Dienst gestoppt."));
    trayIcon->setIcon(running?QIcon(":/images/icon.svg"):QIcon(":/images/stopped.svg"));
}
