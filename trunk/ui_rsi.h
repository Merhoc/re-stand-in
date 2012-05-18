/********************************************************************************
** Form generated from reading UI file 'rsi.ui'
**
** Created: Fri May 18 22:51:20 2012
**      by: Qt User Interface Compiler version 4.8.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_RSI_H
#define UI_RSI_H

#include <QtCore/QVariant>
#include <QtGui/QAction>
#include <QtGui/QApplication>
#include <QtGui/QButtonGroup>
#include <QtGui/QGridLayout>
#include <QtGui/QHBoxLayout>
#include <QtGui/QHeaderView>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QMainWindow>
#include <QtGui/QMenuBar>
#include <QtGui/QPushButton>
#include <QtGui/QSpacerItem>
#include <QtGui/QStatusBar>
#include <QtGui/QToolBar>
#include <QtGui/QVBoxLayout>
#include <QtGui/QWidget>

QT_BEGIN_NAMESPACE

class Ui_rsi
{
public:
    QWidget *centralWidget;
    QWidget *widget;
    QVBoxLayout *verticalLayout;
    QLabel *header;
    QGridLayout *form;
    QLabel *label_uw;
    QLineEdit *input_uw;
    QPushButton *button_uw;
    QLabel *label_muster;
    QLineEdit *input_muster;
    QPushButton *button_muster;
    QLabel *label_uw_2;
    QLineEdit *input_uw_2;
    QPushButton *button_uw_2;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QPushButton *button_ok;
    QPushButton *button_quit;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *rsi)
    {
        if (rsi->objectName().isEmpty())
            rsi->setObjectName(QString::fromUtf8("rsi"));
        rsi->resize(404, 308);
        centralWidget = new QWidget(rsi);
        centralWidget->setObjectName(QString::fromUtf8("centralWidget"));
        widget = new QWidget(centralWidget);
        widget->setObjectName(QString::fromUtf8("widget"));
        widget->setGeometry(QRect(10, 10, 381, 241));
        verticalLayout = new QVBoxLayout(widget);
        verticalLayout->setSpacing(6);
        verticalLayout->setContentsMargins(11, 11, 11, 11);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        header = new QLabel(widget);
        header->setObjectName(QString::fromUtf8("header"));
        QFont font;
        font.setPointSize(16);
        header->setFont(font);
        header->setAlignment(Qt::AlignHCenter|Qt::AlignTop);

        verticalLayout->addWidget(header);

        form = new QGridLayout();
        form->setSpacing(6);
        form->setObjectName(QString::fromUtf8("form"));
        form->setSizeConstraint(QLayout::SetDefaultConstraint);
        label_uw = new QLabel(widget);
        label_uw->setObjectName(QString::fromUtf8("label_uw"));

        form->addWidget(label_uw, 0, 0, 1, 1);

        input_uw = new QLineEdit(widget);
        input_uw->setObjectName(QString::fromUtf8("input_uw"));

        form->addWidget(input_uw, 0, 1, 1, 1);

        button_uw = new QPushButton(widget);
        button_uw->setObjectName(QString::fromUtf8("button_uw"));

        form->addWidget(button_uw, 0, 2, 1, 1);

        label_muster = new QLabel(widget);
        label_muster->setObjectName(QString::fromUtf8("label_muster"));

        form->addWidget(label_muster, 4, 0, 1, 1);

        input_muster = new QLineEdit(widget);
        input_muster->setObjectName(QString::fromUtf8("input_muster"));

        form->addWidget(input_muster, 4, 1, 1, 1);

        button_muster = new QPushButton(widget);
        button_muster->setObjectName(QString::fromUtf8("button_muster"));

        form->addWidget(button_muster, 4, 2, 1, 1);

        label_uw_2 = new QLabel(widget);
        label_uw_2->setObjectName(QString::fromUtf8("label_uw_2"));

        form->addWidget(label_uw_2, 2, 0, 1, 1);

        input_uw_2 = new QLineEdit(widget);
        input_uw_2->setObjectName(QString::fromUtf8("input_uw_2"));

        form->addWidget(input_uw_2, 2, 1, 1, 1);

        button_uw_2 = new QPushButton(widget);
        button_uw_2->setObjectName(QString::fromUtf8("button_uw_2"));

        form->addWidget(button_uw_2, 2, 2, 1, 1);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Fixed);

        form->addItem(verticalSpacer, 3, 0, 1, 1);


        verticalLayout->addLayout(form);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        horizontalLayout->setContentsMargins(-1, 40, -1, -1);
        button_ok = new QPushButton(widget);
        button_ok->setObjectName(QString::fromUtf8("button_ok"));

        horizontalLayout->addWidget(button_ok);

        button_quit = new QPushButton(widget);
        button_quit->setObjectName(QString::fromUtf8("button_quit"));

        horizontalLayout->addWidget(button_quit);


        verticalLayout->addLayout(horizontalLayout);

        rsi->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(rsi);
        menuBar->setObjectName(QString::fromUtf8("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 404, 21));
        rsi->setMenuBar(menuBar);
        mainToolBar = new QToolBar(rsi);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        rsi->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(rsi);
        statusBar->setObjectName(QString::fromUtf8("statusBar"));
        rsi->setStatusBar(statusBar);

        retranslateUi(rsi);

        QMetaObject::connectSlotsByName(rsi);
    } // setupUi

    void retranslateUi(QMainWindow *rsi)
    {
        rsi->setWindowTitle(QApplication::translate("rsi", "rsi", 0, QApplication::UnicodeUTF8));
        header->setText(QApplication::translate("rsi", "Einstellungen", 0, QApplication::UnicodeUTF8));
        label_uw->setText(QApplication::translate("rsi", "\303\234berwachen 1:", 0, QApplication::UnicodeUTF8));
        button_uw->setText(QApplication::translate("rsi", "...", 0, QApplication::UnicodeUTF8));
        label_muster->setText(QApplication::translate("rsi", "Muster:", 0, QApplication::UnicodeUTF8));
        button_muster->setText(QApplication::translate("rsi", "...", 0, QApplication::UnicodeUTF8));
        label_uw_2->setText(QApplication::translate("rsi", "\303\234berwachen 2:", 0, QApplication::UnicodeUTF8));
        button_uw_2->setText(QApplication::translate("rsi", "...", 0, QApplication::UnicodeUTF8));
        button_ok->setText(QApplication::translate("rsi", "Ok", 0, QApplication::UnicodeUTF8));
        button_quit->setText(QApplication::translate("rsi", "Beenden", 0, QApplication::UnicodeUTF8));
    } // retranslateUi

};

namespace Ui {
    class rsi: public Ui_rsi {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_RSI_H
