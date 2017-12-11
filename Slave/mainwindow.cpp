/***************************************************************************//**
* @file     mainwindow.cpp
* @brief    Obsługa wymiany danych przez sloty UDP Slave
* @author   Roman Szyliński <rszylinski@wp.pl>
* @version  1.00
* @date     2017-07-11
*
* @note
* Copyright Roman Szyliński. All rights reserved. @n
* THIS IS UNPUBLISHED PROPRIETARY SOURCE CODE OF Roman Szyliński
* The copyright notice above does not evidence any actual or intended
* publication of such source code.
******************************************************************************/
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QScrollBar>

#define dPortSend 35001
#define dPortReceive 35000

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    connect( &socketUdp, SIGNAL(readyRead()), SLOT(socketUdp_readyRead()) );
    if( !socketUdp.bind( QHostAddress::Any, dPortReceive , QAbstractSocket::ShareAddress | QAbstractSocket::ReuseAddressHint ) )
    {
      qDebug( "Bind unsuccess connected to port" );
    }

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

        CommandRun( baRead );

    } while( socketUdp.hasPendingDatagrams() );
}


void MainWindow::AnswareSend( QString ACmd )
{
    QByteArray baFrm = ACmd.toLocal8Bit();
    logf( "SEND [%s]", baFrm.data() );
    socketUdp.writeDatagram( baFrm, QHostAddress::LocalHost, dPortSend );
}

void MainWindow::CommandRun( QByteArray baBuf )
{
   ui->leCmd->setText( baBuf );
   QString strCmd = baBuf.toLower();
   if( strCmd == "losuj" )
   {
       Randomize();
       return;
   }
   if( strCmd == "zamknij" )
   {
       AnswareSend("Close Slave Application");
       QTimer::singleShot( 200, this, SLOT(DoClose()) );
       return;
   }
   if( strCmd == "help" )
   {
       AnswareSend("Hello Roman");
       return;
   }
   AnswareSend( QString("Unknow Command [%1]").arg(QString(baBuf)) );
}

void MainWindow::DoClose()
{
    QApplication::quit();
}

void MainWindow::Randomize()
{
    int dig[3];
    dig[0] = (3*rand())/RAND_MAX;
    dig[1] = (3*rand())/RAND_MAX;
    dig[2] = (3*rand())/RAND_MAX;
    ui->lDigit_1->setText( QString::number(dig[0]) );
    ui->lDigit_2->setText( QString::number(dig[1]) );
    ui->lDigit_3->setText( QString::number(dig[2]) );
    if( dig[0] == dig[1] && dig[1] == dig[2] )
    {
       AnswareSend( QString().sprintf("[%d-%d-%d] Winner",  dig[0], dig[1], dig[2] ) );
    }
    else
    {
        AnswareSend( QString().sprintf("[%d-%d-%d] Lost",  dig[0], dig[1], dig[2] ) );
    }
}
