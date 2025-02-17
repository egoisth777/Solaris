#include "mainwindow.h"
#include <ui_mainwindow.h>
#include "cameracontrolshelp.h"
#include <QResizeEvent>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow), cHelp()
{
    ui->setupUi(this);
    ui->mygl->setFocus();
    this->playerInfoWindow.show();
    playerInfoWindow.move(QGuiApplication::primaryScreen()->availableGeometry().center() - this->rect().center() + QPoint(this->width() * 0.75, 0));

    connect(ui->mygl, SIGNAL(sig_sendPlayerPos(QString)), &playerInfoWindow, SLOT(slot_setPosText(QString)));
    connect(ui->mygl, SIGNAL(sig_sendPlayerVel(QString)), &playerInfoWindow, SLOT(slot_setVelText(QString)));
    connect(ui->mygl, SIGNAL(sig_sendPlayerAcc(QString)), &playerInfoWindow, SLOT(slot_setAccText(QString)));
    connect(ui->mygl, SIGNAL(sig_sendPlayerLook(QString)), &playerInfoWindow, SLOT(slot_setLookText(QString)));
    connect(ui->mygl, SIGNAL(sig_sendPlayerChunk(QString)), &playerInfoWindow, SLOT(slot_setChunkText(QString)));
    connect(ui->mygl, SIGNAL(sig_sendPlayerTerrainZone(QString)), &playerInfoWindow, SLOT(slot_setZoneText(QString)));
    connect(ui->mygl, SIGNAL(sig_sendFPS(QString)), &playerInfoWindow, SLOT(slot_setFPSText(QString)));
    connect(ui->mygl, SIGNAL(sig_sendAveFPS(QString)), &playerInfoWindow, SLOT(slot_setAveFPSText(QString)));
    connect(ui->mygl, SIGNAL(sig_enableLoadingBanner(bool)), this, SLOT(slotEnableLoadingBanner(bool)));
    connect(ui->mygl, SIGNAL(sig_sendLoadingStatus(QString)), this, SLOT(slotSetLoadingStatus(QString)));


    ui->loadingBanner->setScaledContents(true);
    slotEnableLoadingBanner(true);
    this->unsetCursor();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionQuit_triggered()
{
    QApplication::exit();
}

void MainWindow::on_actionCamera_Controls_triggered()
{
    cHelp.show();
}

void MainWindow::slotEnableLoadingBanner(bool isEnable)
{
    ui->loadingBanner->setHidden(!isEnable);
}

void MainWindow::slotSetLoadingStatus(QString val)
{
    this->setWindowTitle("OptFine" + val);
}
