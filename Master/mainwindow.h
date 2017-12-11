#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>
#include <QDateTime>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void socketUdp_readyRead();

    void on_pbCmdLosuj_clicked();

    void on_pbCmdClose_clicked();

    void on_pbCmdSend_clicked();

    void RunMaster();

    void on_pbSlaveRun_clicked();

private:
    Ui::MainWindow *ui;

    QUdpSocket socketUdp;

    void logf( const QString &ACo );
    void logf( const char *frm, ... );

    void CommandSend( QString ACmd );
    void AnswerParse( QByteArray &baRead );

};

#endif // MAINWINDOW_H
