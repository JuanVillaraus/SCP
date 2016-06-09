/*#include "siviso.h"
#include "ui_siviso.h"

siviso::siviso(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::siviso)
{
    ui->setupUi(this);


}

siviso::~siviso()
{
    delete ui;
}

*/



#include "siviso.h"
#include "ui_siviso.h"

#include <QFile>
#include <QDebug>
#include <QtCore>
#include <QtGui>

#define localdir QHostAddress("192.168.1.178")        //de donde nos comunicamos
#define puertolocal 5002

siviso::siviso(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::siviso)
{
    ui->setupUi(this);
    myppi = new PPI();
    mysignal = new Signal();

    udpsocket = new QUdpSocket(this);
    udpsocket->bind(localdir,puertolocal);
    connect(udpsocket,SIGNAL(readyRead()),this,SLOT(leerSocket()));
    //connect(serialPort, SIGNAL(readyRead()),this,SLOT(leerSerial()));

    direccionSPP = "192.168.1.177";                   //direccion del SPP
    puertoSPP = 8888;                                 //puerto del SPP
    direccionApp = "192.168.1.178";                   //direccion que usaran las aplicaciones
    //udpsocket->writeDatagram(ui->view->text().toLatin1(),direccionPar,puertoPar); //visualiza la direcion IP y puerto del que envia

    /*serialPort->setPortName("/dev/ttyS2");
    if(serialPort->open(QIODevice::ReadWrite))
        qDebug("Puerto serial abierto\n");
    else
        qDebug("Error de coexion con el puerto serial\n");
    serialPort->setBaudRate(QSerialPort::Baud9600);
    serialPort->setDataBits(QSerialPort::Data8);
    serialPort->setStopBits(QSerialPort::OneStop);
    serialPort->setParity(QSerialPort::NoParity);
    serialPort->setFlowControl(QSerialPort::NoFlowControl);*/


    ui->textTestSend->appendPlainText("Esto se enviará al subsistema \n");

    ui->frecuencia->setValue(mysignal->get_frec());
    ui->bw->setValue(mysignal->get_bw());
    ui->it->setValue(mysignal->get_it());
    ui->dt->setValue(mysignal->get_dt());
    ui->gan_sen->setValue(mysignal->get_ganancia_sensor());
    ui->dt->setValue(mysignal->get_dt());
    ui->prob_falsa->setValue(mysignal->get_prob_falsa());
    ui->prob_deteccion->setValue(mysignal->get_prob_deteccion());
    ui->edo_mar->setValue(mysignal->get_edo_mar());

//This use for TEST the class DBasePostgreSQL by Misael M Del Valle -- Status: Functional
//    myDB = new DBasePostgreSQL("172.16.1.3","PruebaQT",5432,"Administrador","nautilus");
//    myDB->executeSql("Select * from usuarios");

//Config Widget TEST -- Status: In construction and Functional
//   wConfig *w = new wConfig();
//   w->show();

}

siviso::~siviso()
{
    serialPort->close();
    delete serialPort;
    delete ui;
}

void siviso::on_toolButton_clicked()
{
    if(mycontrol){
        mycontrol=false;
        ui->textTestSend->setVisible(false);
        ui->textTestGrap->setVisible(false);
        ui->view->setVisible(false);
        ui->pushButton_info->setVisible(false);
        ui->pushButton_send->setVisible(false);
    }else{
        mycontrol=true;
        ui->textTestSend->setVisible(true);
        ui->textTestGrap->setVisible(true);
        ui->view->setVisible(true);
        ui->pushButton_info->setVisible(true);
        ui->pushButton_send->setVisible(true);
    }
}

void siviso::changeStyleSheet(int iStyle)
{
    QDir dir;

    if(m_pApplication != NULL)
        {

        //Change directory of the file
        QString strFilePath(dir.absolutePath()+"/");

            if(iStyle == DAY_STYLE)
                strFilePath.append("siviso_day.css");

            if(iStyle == DUSK_STYLE)
                strFilePath.append("siviso_dusk.css");

            if(iStyle == NIGHT_STYLE)
                strFilePath.append("siviso_night.css");

            QFile file(strFilePath);

            if(!file.open( QIODevice::ReadOnly | QIODevice::Text ))
            {
                qDebug() << "SIVISO: Failed to open- Copy the CSS File under the folder build...";
            }
            else
            {
                m_pApplication->setStyleSheet(QString(file.readAll()));
                file.close();
            }
    }
}

void siviso::leerSocket()
{
    while(udpsocket->hasPendingDatagrams())
    {
        QByteArray datagram;
        datagram.resize(udpsocket->pendingDatagramSize());
        QHostAddress sender;
        quint16 senderPort;
        udpsocket->readDatagram(datagram.data(),datagram.size(), &sender, &senderPort);
        //ui->textTestGrap->appendPlainText(QString(datagram) + " (ip->" + sender.toString() + " ; port-> " + QString("%1").arg(senderPort) + " ) ");
        QString info = datagram.data();
        //QString s = " ";
        /*if(info.data()[0]=='$')                       //verifica cabecera y parte elo paquete en secciones sepáradas ṕor punto y coma
        for(int x=0;x<=info.size();x++)
           {
            if(info.data()[x]!=';')
                s += info.data()[x];
            else{
                ui->textTestGrap->appendPlainText(s);
                s = " ";
            }
        }
        else
            ui->textTestGrap->appendPlainText("ERROR: el mensaje no tiene cabecera");*/
        ui->textTestGrap->appendPlainText(" port-> " + QString("%1").arg(senderPort));
        ui->textTestGrap->appendPlainText(info);
        //s = " ";

        if(info == "runPPI")
            puertoPPI = senderPort;
        if(info == "runBTR")
            puertoBTR = senderPort;
        if(info == "runLF")
            puertoLF = senderPort;
        //puertoPar = senderPort;
        if(puertoSPP == senderPort){
             udpsocket->writeDatagram(info.toLatin1(),direccionApp,puertoBTR);
             udpsocket->writeDatagram(info.toLatin1(),direccionApp,puertoLF);
        }
        if(info == "BTR" || info == "LOFAR")
            udpsocket->writeDatagram(info.toLatin1(),direccionSPP,puertoSPP);
        if(puertoBTR == senderPort){
            ui->textTestGrap->appendPlainText(" intento enviar ");
            udpsocket->writeDatagram(info.toLatin1(),direccionSPP,puertoSPP);
            ui->textTestGrap->appendPlainText(" paquete enviado ");
        }
    }
}

void siviso::leerSerial()
{
    char buffer[101];
    int nDatos;

    nDatos = serialPort->read(buffer,100);
    buffer[nDatos] = '\0';
    ui->textTestGrap->appendPlainText(buffer);
}

void siviso::on_tipo_norte_clicked()
{
    //PPI *myppi = new PPI();
    ui->textTestGrap->appendPlainText("clic");
    if(myppi->get_tipo_norte())
    {
        ui->tipo_norte->setText("Norte\nRelativo");
        myppi->set_tipo_norte(false);
    }
    else
    {
        ui->tipo_norte->setText("Norte\nVerdadero");
        myppi->set_tipo_norte(true);
    }

}

void siviso::on_nb_clicked()
{
    ui->textTestGrap->appendPlainText("despliega LOFAR");
    QString s = "LOFAR";
    ui->view->appendPlainText("send: " + s);
    udpsocket->writeDatagram(s.toLatin1(),direccionSPP,puertoSPP);
    s = "LF_ON";
    udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoLF);
    s = "BTR_OFF";
    udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoBTR);
}

void siviso::on_bb_clicked()
{
    ui->textTestGrap->appendPlainText("habilita los botones Waterfall y PPI");
    QString s = "END COMMUNICATION";
    ui->view->appendPlainText("send: " + s);
    udpsocket->writeDatagram(s.toLatin1(),direccionSPP,puertoSPP);
}

void siviso::on_wf_clicked()
{
    ui->textTestGrap->appendPlainText("despliega Waterfall");
    QString s = "BTR";
    ui->view->appendPlainText("send to SSPP: " + s);
    udpsocket->writeDatagram(s.toLatin1(),direccionSPP,puertoSPP);
    s = "BTR_ON";
    udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoBTR);
    s = "LF_OFF";
    udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoLF);
}

void siviso::on_ppi_clicked()
{
    ui->textTestGrap->appendPlainText("despliega PPI");
    QString s = "VERSION";
    ui->view->appendPlainText("send: " + s);
    udpsocket->writeDatagram(s.toLatin1(),direccionSPP,puertoSPP);
    s = "BTR_OFF";
    udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoBTR);
    s = "LF_OFF";
    udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoLF);
}

void siviso::on_origen_buque_clicked()
{
    ui->textTestGrap->appendPlainText("se cambio el origen a buque");
}

void siviso::on_origen_target_clicked()
{
    ui->textTestGrap->appendPlainText("se cambio el origen a blanco");
}

void siviso::on_pushButton_info_clicked()
{
    ui->view->appendPlainText("info: ");
    QString s = mysignal->get_info_signal();
    ui->view->appendPlainText(s);
    ui->view->appendPlainText("portSPP " + QString("%1").arg(puertoSPP));
    ui->view->appendPlainText("portBTR " + QString("%1").arg(puertoBTR));
    ui->view->appendPlainText("portLF " + QString("%1").arg(puertoLF));
}

void siviso::on_frecuencia_valueChanged(int value)
{
    mysignal->set_frec(value);

    ui->textTestGrap->appendPlainText("frec: ");
    QString s = QString::number(value);
    ui->textTestGrap->appendPlainText(s);
}

void siviso::on_bw_valueChanged(double arg1)
{
    mysignal->set_bw(arg1);

    ui->textTestGrap->appendPlainText("bw: ");
    QString s = QString::number(arg1);
    ui->textTestGrap->appendPlainText(s);
}

/*void siviso::on_it_valueChanged(double arg1)
{
    mysignal->set_it(arg1);

    ui->textTestSend->appendPlainText("It: ");
    QString s = QString::number(arg1);
    ui->textTestSend->appendPlainText(s);
}*/

/*void siviso::on_dt_valueChanged(double arg1)
{
    mysignal->set_dt(arg1);

    ui->textTestGrap->appendPlainText("dt: ");
    QString s = QString::number(arg1);
    ui->textTestGrap->appendPlainText(s);
}*/

void siviso::on_edo_mar_valueChanged(int arg1)
{
    mysignal->set_edo_mar(arg1);

    ui->textTestSend->appendPlainText("edo_mar: ");
    QString s = QString::number(arg1);
    ui->textTestSend->appendPlainText(s);
}

void siviso::on_prob_falsa_valueChanged(double arg1)
{
    mysignal->set_prob_falsa(arg1);

    ui->textTestSend->appendPlainText("prob_falsa: ");
    QString s = QString::number(arg1);
    ui->textTestSend->appendPlainText(s);
}

void siviso::on_prob_deteccion_valueChanged(double arg1)
{
    mysignal->set_prob_deteccion(arg1);

    ui->textTestSend->appendPlainText("prob_deteccion: ");
    QString s = QString::number(arg1);
    ui->textTestSend->appendPlainText(s);
}

void siviso::on_escala_ppi_valueChanged(double arg1)
{
    ui->textTestGrap->appendPlainText("escala PPI: ");
    QString s = QString::number(arg1);
    ui->textTestGrap->appendPlainText(s);
}

void siviso::on_escala_despliegue_tactico_valueChanged(double arg1)
{
    ui->textTestGrap->appendPlainText("desp_tact: ");
    QString s = QString::number(arg1);
    ui->textTestGrap->appendPlainText(s);
}

void siviso::on_gan_sen_valueChanged(int arg1)
{
    mysignal->set_ganancia_sensor(arg1);

    ui->textTestSend->appendPlainText("ganancia_sensor: ");
    QString s = QString::number(arg1);
    ui->textTestSend->appendPlainText(s);
}

//boton para enviar la informacion
void siviso::on_pushButton_send_clicked()
{
    //QString s = mysignal->send_to_sensor();               //guarda en la variable "s" lo que tiene en sensor
    QString s = "START COMMUNICATION";
    ui->view->appendPlainText("send: " + s);
    udpsocket->writeDatagram(s.toLatin1(),direccionSPP,puertoSPP);
}



/*void siviso::on_pushButton_clicked()
{


}*/

void siviso::on_it_valueChanged(int arg1)
{
    mysignal->set_it(arg1);

    ui->textTestSend->appendPlainText("It: ");
    QString s = QString::number(arg1);
    ui->textTestSend->appendPlainText(s);
}
