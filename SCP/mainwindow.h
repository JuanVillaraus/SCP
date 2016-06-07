/*
 * #ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QtSerialPort>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
*/

#ifndef SIVISO_H
#define SIVISO_H
/*
#include <blanco.h>
#include <ppi.h>
#include <signal.h>
#include <QDir>
#include <QHostAddress>
#include <QUdpSocket>

//---------headers for the use of postgres
#include<QtSql/QSql>
#include<QtSql/QSqlDatabase>
#include<QtSql/QSqlQuery>
#include<QtSql/QSqlRecord>
#include<QtSql/QSqlField>
#include<QStandardItemModel>
#include"dbasepostgresql.h"
//---------finish declarations of the headers
*/
#include <QMainWindow>
//#include "wconfig.h"


#define DAY_STYLE 0
#define DUSK_STYLE 1
#define NIGHT_STYLE 2

namespace Ui {
class siviso;
}

class siviso : public QMainWindow
{
    Q_OBJECT

public:
    explicit siviso(QWidget *parent = 0);
    ~siviso();

    QApplication *m_pApplication;

    void changeStyleSheet(int iStyle);

    int numero;
    PPI *myppi;
    Signal *mysignal;

public slots:
    void leerSocket();

private slots:

    void on_tipo_norte_clicked();

    void on_nb_clicked();

    void on_bb_clicked();

    void on_wf_clicked();

    void on_ppi_clicked();

    void on_origen_buque_clicked();

    void on_origen_target_clicked();

    void on_bw_valueChanged(double arg1);

    void on_pushButton_clicked();

    void on_it_valueChanged(double arg1);

    void on_edo_mar_valueChanged(int arg1);

    void on_prob_falsa_valueChanged(double arg1);

    void on_prob_deteccion_valueChanged(double arg1);

    void on_escala_ppi_valueChanged(double arg1);

    void on_escala_despliegue_tactico_valueChanged(double arg1);

    void on_gan_sen_valueChanged(int arg1);

    void on_frecuencia_valueChanged(int value);

    void on_dt_valueChanged(double arg1);

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_it_valueChanged(int arg1);

private:
    Ui::siviso *ui;

    QUdpSocket *udpsocket;
    QHostAddress direccionSPP;
    QHostAddress direccionApp;
    quint16 puertoSPP;
    quint16 puertoPPI;
    quint16 puertoBTR;
    quint16 puertoLF;

    // For use the class dbasepostgresql by Misael M Del Valle
    DBasePostgreSQL* myDB;

};

#endif // SIVISO_H
