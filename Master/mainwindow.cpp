/***************************************************************************//**
* @file     mainwindow.cpp
* @brief    Obsługa wymiany danych przez sloty UDP Master
* @author   John Brown nic35025@wp.pl
* @version  1.00
* @date     2017-07-11
*
* @note
* Copyright nic35025. All rights reserved. @n
* THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF Roman Szyliński
* The copyright notice above does not evidence any actual or intended
* publication of such source code.
******************************************************************************/
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QScrollBar>
#include <QTimer>
#include <QProcess>
#include <QMessageBox>
#include <QProcessEnvironment>
#include <QSettings>

#define dPortSend 35000
#define dPortReceive 35001

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect( &socketUdp, SIGNAL(readyRead()), SLOT(socketUdp_readyRead()) );
    if( !socketUdp.bind( QHostAddress::LocalHost, dPortReceive , QAbstractSocket::ShareAddress | QAbstractSocket::ReuseAddressHint ) )
    {
      qDebug( "Bind unsuccess connected to port" );
    }

    QTimer::singleShot(100, this, SLOT(RunMaster()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

#include <QMutex>
#include <QMutexLocker>
void MainWindow::logf( const QString &ACo ) {
    QMutex mux;
    const QMutexLocker ml(&mux);

    ui->pleLog->moveCursor(QTextCursor::End);
    ui->pleLog->appendPlainText( ACo );

    QScrollBar *pScroll = ui->pleLog->verticalScrollBar();
    pScroll->setValue( pScroll->maximum() );
}

#include <QDateTime>
#include <stdarg.h>

void MainWindow::logf( const char *frm, ... )
{
    QString buf;
    va_list argptr;
    va_start(argptr, frm);
    buf.vsprintf(frm, argptr );
    logf( QDateTime::currentDateTime().toString("#hhmm:ss.zzz") +"> " +buf );
}

//=======================================================
void MainWindow::socketUdp_readyRead()
{
    QByteArray baRead;
    QHostAddress lAddress;
    quint16      lPort   ;

    do {
        int iSize = socketUdp.pendingDatagramSize();
        if ( iSize == 0 )
        {
            break;
        }
        baRead.resize(iSize);
        socketUdp.readDatagram( baRead.data(), iSize, &lAddress, &lPort);
        logf("RECV(%s:%d) [%s]", lAddress.toString().toStdString().c_str(), lPort, baRead.data() );

        AnswerParse( baRead );
    } while( socketUdp.hasPendingDatagrams() );
}

void MainWindow::AnswerParse( QByteArray &baRead )
{
    ui->leAns->setText( baRead );
    if( baRead == "Close Slave Application") {
        QTimer::singleShot( 1000, this, SLOT(close()) );
    }
}

void MainWindow::CommandSend(QString ACmd)
{
    QByteArray baFrm = ACmd.toLocal8Bit();
    logf( "SEND [%s]", baFrm.data() );
    socketUdp.writeDatagram( baFrm, QHostAddress::LocalHost, dPortSend );
}

void MainWindow::RunMaster()
{
    QString strSlaveFileNameExe = QApplication::applicationDirPath()+"/Slave.exe";
    QFile fExe;
    fExe.setFileName( strSlaveFileNameExe );
    bool bOpen =fExe.open( QIODevice::ReadOnly );
    if( !bOpen )
    {
        qDebug( "File do not exist [%s]", strSlaveFileNameExe.toStdString().c_str() );
        QMessageBox mb;
        mb.setText( QString().sprintf("File do not exist [%s]", strSlaveFileNameExe.toStdString().c_str() ));
        mb.setIcon( QMessageBox::Critical );
        mb.exec();
        return;
    }
    fExe.close();

    QProcess *procSlave =new QProcess();
    procSlave->start( strSlaveFileNameExe );
}


void MainWindow::on_pbCmdLosuj_clicked()
{
    CommandSend("Losuj");
}

void MainWindow::on_pbCmdClose_clicked()
{
    CommandSend("Zamknij");
}

void MainWindow::on_pbCmdSend_clicked()
{
    CommandSend( ui->leCmd->text() );
}

void MainWindow::on_pbSlaveRun_clicked()
{
    RunMaster();
}
