#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    ui->label->setStyleSheet("font-size: 22px; font-weight: bold; color: #1e293b;");

    // 1. Initialisation UDP
    udpSocket = new QUdpSocket(this);
    udpSocket->bind(8001);
    ipSTM32 = QHostAddress("192.168.184.170"); // IP de la STM32

    ui->label->setText("Mini-Projet");

    // 2. Configuration initiale du slider
    ui->horizontalSlider->setRange(0, 180);
    ui->horizontalSlider->setValue(90);
    ui->txtAngle->setText("90");

    // 3. Connexions des signaux
    connect(ui->horizontalSlider, &QSlider::valueChanged, this, [=](int val){
        ui->txtAngle->setText(QString::number(val));
    });
    connect(ui->ButtonSendAngle, &QPushButton::clicked, this, &MainWindow::sendAngle);
    connect(ui->buttonDist, &QPushButton::clicked, this, &MainWindow::requestDistance);
    connect(ui->ButtonScan, &QPushButton::clicked, this, &MainWindow::requestScan);
    connect(udpSocket, &QUdpSocket::readyRead, this, &MainWindow::readData);
}

MainWindow::~MainWindow() {
    delete ui;
}

// --- ENVOI DE L'ANGLE ---
void MainWindow::sendAngle() {
    int val = ui->horizontalSlider->value();
    QString cmd = QString("ANGLE %1\n").arg(val);
    udpSocket->writeDatagram(cmd.toUtf8(), ipSTM32, 8001);
}

// --- DEMANDE DE DISTANCE ---
void MainWindow::requestDistance() {
    udpSocket->writeDatagram(QByteArray("DIST\n"), ipSTM32, 8001);
}

// --- DEMANDE DE SCAN ---
void MainWindow::requestScan() {
    ui->txtScanResult->setPlainText("Balayage en cours sur la carte...\n");
    udpSocket->writeDatagram(QByteArray("SCAN\n"), ipSTM32, 8001);
}

// --- RÉCEPTION UDP ---
void MainWindow::readData() {
    while (udpSocket->hasPendingDatagrams()) {
        QByteArray buffer;
        buffer.resize(udpSocket->pendingDatagramSize());
        udpSocket->readDatagram(buffer.data(), buffer.size());
        QString resp = QString::fromUtf8(buffer).trimmed();

        // Si c'est le rapport complet du SCAN ou une erreur
        if (resp.contains("TIMEOUT") || resp.contains("END") || resp.contains("\n")) {
            ui->txtScanResult->setPlainText(resp);
        } else {
            // Sinon c'est la réponse de la distance (ex: "DIST 35.5")
            ui->txtDist->setText(resp);
        }
    }
}