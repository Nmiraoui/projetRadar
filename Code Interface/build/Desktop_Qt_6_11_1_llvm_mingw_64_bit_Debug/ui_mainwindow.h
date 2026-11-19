/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 6.11.1
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSlider>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QSlider *horizontalSlider;
    QLineEdit *txtAngle;
    QPushButton *ButtonSendAngle;
    QPushButton *buttonDist;
    QLineEdit *txtDist;
    QTextEdit *txtScanResult;
    QPushButton *ButtonScan;
    QLabel *label;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        horizontalSlider = new QSlider(centralwidget);
        horizontalSlider->setObjectName("horizontalSlider");
        horizontalSlider->setGeometry(QRect(550, 390, 161, 31));
        horizontalSlider->setOrientation(Qt::Orientation::Horizontal);
        txtAngle = new QLineEdit(centralwidget);
        txtAngle->setObjectName("txtAngle");
        txtAngle->setGeometry(QRect(550, 340, 161, 31));
        ButtonSendAngle = new QPushButton(centralwidget);
        ButtonSendAngle->setObjectName("ButtonSendAngle");
        ButtonSendAngle->setGeometry(QRect(540, 440, 181, 91));
        buttonDist = new QPushButton(centralwidget);
        buttonDist->setObjectName("buttonDist");
        buttonDist->setGeometry(QRect(310, 440, 171, 91));
        txtDist = new QLineEdit(centralwidget);
        txtDist->setObjectName("txtDist");
        txtDist->setGeometry(QRect(330, 390, 141, 31));
        txtScanResult = new QTextEdit(centralwidget);
        txtScanResult->setObjectName("txtScanResult");
        txtScanResult->setGeometry(QRect(80, 160, 171, 261));
        ButtonScan = new QPushButton(centralwidget);
        ButtonScan->setObjectName("ButtonScan");
        ButtonScan->setGeometry(QRect(80, 440, 161, 91));
        label = new QLabel(centralwidget);
        label->setObjectName("label");
        label->setGeometry(QRect(410, 150, 91, 121));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 22));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        ButtonSendAngle->setText(QCoreApplication::translate("MainWindow", "ANGLE", nullptr));
        buttonDist->setText(QCoreApplication::translate("MainWindow", "DISTANCE", nullptr));
        ButtonScan->setText(QCoreApplication::translate("MainWindow", "SCAN", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", " Mini-Projet", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
