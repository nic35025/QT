#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QUdpSocket>

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
    void DoClose();
private:
    Ui::MainWindow *ui;

    QUdpSocket socketUdp;

    void logf( const QString &ACo );
    void logf( const char *frm, ... );

    void AnswerSend( QString ACmd );

    void CommandRun( QByteArray baBuf );
    void AnswareSend(QString ACmd);
    void Randomize();
};

#endif // MAINWINDOW_H
