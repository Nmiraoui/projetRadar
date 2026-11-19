#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QHostAddress>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void sendAngle();
    void requestDistance();
    void requestScan();
    void readData();

private:
    Ui::MainWindow *ui; // Pointeur vers l'interface visuelle .ui
    QUdpSocket *udpSocket;
    QHostAddress ipSTM32;
};

#endif // MAINWINDOW_H