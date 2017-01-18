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

//#define localdir QHostAddress("192.168.1.178")        //de donde nos comunicamos
#define puertolocal 5002

siviso::siviso(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::siviso)
{
    ui->setupUi(this);
    myppi = new PPI();
    mysignal = new Signal();
    proceso1 = new QProcess(this);
    proceso2 = new QProcess(this);
    proceso3 = new QProcess(this);
    numCatchSend = 0;
    catchSend = "";
    catchHeader = "";
    compGraf = "";
    bCatchHeader = false;
    GPSt = "";
    GPSn = "";
    GPSw = "";
    bGPSt = false;
    bGPSn = false;
    bGPSw = false;

    udpsocket = new QUdpSocket(this);
    //udpsocket->bind(localdir,puertolocal);
    udpsocket->bind(QHostAddress::LocalHost, puertolocal);
    serialPortDB9 = new QSerialPort();
    serialPortGPS = new QSerialPort();
    serialPortUSB = new QSerialPort();
    connect(udpsocket,SIGNAL(readyRead()),this,SLOT(leerSocket()));
    connect(serialPortDB9, SIGNAL(readyRead()),this,SLOT(leerSerialDB9()));
    connect(serialPortGPS, SIGNAL(readyRead()),this,SLOT(leerSerialGPS()));
    connect(serialPortUSB, SIGNAL(readyRead()),this,SLOT(leerSerialUSB()));

    direccionSPP = "192.168.1.177";                   //direccion del SPP
    puertoSPP = 8888;                                 //puerto del SPP
    //direccionApp = "192.168.1.178";                   //direccion que usaran las aplicaciones
    direccionApp = "127.0.0.1";                   //direccion que usaran las aplicaciones
    //udpsocket->writeDatagram(ui->view->text().toLatin1(),direccionPar,puertoPar); //visualiza la direcion IP y puerto del que envia


    ui->view->appendPlainText("Esto se enviará al subsistema \n");

    ui->frecuencia->setValue(mysignal->get_frec());
    ui->bw->setValue(mysignal->get_bw());
    ui->it->setValue(mysignal->get_it());
    ui->dt->setValue(mysignal->get_dt());
    ui->gan_sen->setValue(mysignal->get_ganancia_sensor());
    ui->dt->setValue(mysignal->get_dt());
    ui->prob_falsa->setValue(mysignal->get_prob_falsa());
    ui->prob_deteccion->setValue(mysignal->get_prob_deteccion());
    ui->edo_mar->setValue(mysignal->get_edo_mar());

    bToolButton=false;
    ui->viewGPS->setVisible(false);
    ui->textTestGrap->setVisible(false);
    ui->view->setVisible(false);
    ui->pushButton_info->setVisible(false);
    ui->pushButton_send->setVisible(false);
    ui->closeJars->setVisible(false);
    ui->openJars->setVisible(false);

    serialPortUSB->write("GAIN 3\n");
    
    QFile file1("resource/colorUp.txt");
    if(file1.open(QIODevice::WriteOnly)){
        QTextStream stream(&file1);
        stream<<"255";
    } else {
        qDebug();
    }
    file1.close();

    QFile file2("resource/colorDw.txt");
    if(file2.open(QIODevice::WriteOnly)){
        QTextStream stream(&file2);
        stream<<"0";
    } else {
        qDebug();
    }
    file2.close();

    proceso1->startDetached("java -jar Lofar.jar");
    proceso2->startDetached("java -jar BTR.jar");
    proceso3->startDetached("java -jar Btg.jar");


//This use for TEST the class DBasePostgreSQL by Misael M Del Valle -- Status: Functional
//    myDB = new DBasePostgreSQL("172.16.1.3","PruebaQT",5432,"Administrador","nautilus");
//    myDB->executeSql("Select * from usuarios");

//Config Widget TEST -- Status: In construction and Functional
//   wConfig *w = new wConfig();
//   w->show();



}

siviso::~siviso()
{
    QString s = "EXIT";
    udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoBTR);
    udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoLF);
    udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoBTG);
    serialPortUSB->write("END COMMUNICATION\n");
    serialPortDB9->close();
    serialPortGPS->close();
    serialPortUSB->close();
    delete serialPortDB9;
    delete serialPortGPS;
    delete serialPortUSB;
    delete ui;
    proceso1->close();
    proceso2->close();
    proceso3->close();
}

void siviso::on_toolButton_clicked()
{
    if(bToolButton){
        bToolButton=false;
        ui->viewGPS->setVisible(false);
        ui->textTestGrap->setVisible(false);
        ui->view->setVisible(false);
        ui->pushButton_info->setVisible(false);
        ui->pushButton_send->setVisible(false);
        ui->closeJars->setVisible(false);
        ui->openJars->setVisible(false);
    }else{
        bToolButton=true;
        ui->viewGPS->setVisible(true);
        ui->textTestGrap->setVisible(true);
        ui->view->setVisible(true);
        ui->pushButton_info->setVisible(true);
        ui->pushButton_send->setVisible(true);
        ui->closeJars->setVisible(true);
        ui->openJars->setVisible(true);
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
        /*QString s = " ";
        if(info.data()[0]=='$')                       //verifica cabecera y parte elo paquete en secciones sepáradas ṕor punto y coma
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

        QString s;
        if(info == "runBTG"){
            puertoBTG = senderPort;
            s = "OFF";
            udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoBTG);
        }
        if(info == "runBTR"){
            puertoBTR = senderPort;
            s = "LONG";
            udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoBTR);
            s = "OFF";
            udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoBTR);
        }
        if(info == "runLF"){
            puertoLF = senderPort;
            s = "LONG";
            udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoLF);
            s = "OFF";
            udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoLF);
        }
        if(info == "runREC")
            puertoREC = senderPort;
        //puertoPar = senderPort;
        if(puertoSPP == senderPort){
             udpsocket->writeDatagram(info.toLatin1(),direccionApp,puertoBTR);
             udpsocket->writeDatagram(info.toLatin1(),direccionApp,puertoLF);
        }
        if(info == "BTR")
            serialPortUSB->write("BTR\n");
        if(info == "LOFAR")
            serialPortUSB->write("LOFAR\n");
    }
}

void siviso::on_btOpenPort_clicked()
{
    //Esta parte esta comentada porque en la computadora de desarrollo no tiene acceso a los puertos seriales y proboca crashed
    serialPortDB9->setPortName("/dev/ttyS0");
    if(serialPortDB9->open(QIODevice::ReadWrite))
        ui->view->appendPlainText("Puerto serial db9 abierto\n");
        //qDebug("Puerto serial abierto\n");
    else
        ui->view->appendPlainText("Error de conexion con el puerto serial db9\n");
        //qDebug("Error de coexion con el puerto serial\n");
    serialPortDB9->setBaudRate(QSerialPort::Baud9600);
    serialPortDB9->setDataBits(QSerialPort::Data8);
    serialPortDB9->setStopBits(QSerialPort::OneStop);
    serialPortDB9->setParity(QSerialPort::NoParity);
    serialPortDB9->setFlowControl(QSerialPort::NoFlowControl);

    serialPortGPS->setPortName("/dev/ttyS2");
    if(serialPortGPS->open(QIODevice::ReadWrite))
        ui->view->appendPlainText("Puerto serial GPS abierto\n");
        //qDebug("Puerto serial abierto\n");
    else
        ui->view->appendPlainText("Error de conexion con el puerto serial GPS\n");
        //qDebug("Error de coexion con el puerto serial\n");
    serialPortGPS->setBaudRate(QSerialPort::Baud4800);
    serialPortGPS->setDataBits(QSerialPort::Data8);
    serialPortGPS->setStopBits(QSerialPort::OneStop);
    serialPortGPS->setParity(QSerialPort::NoParity);
    serialPortGPS->setFlowControl(QSerialPort::NoFlowControl);

    serialPortUSB->setPortName("/dev/ttyUSB1");
    if(serialPortUSB->open(QIODevice::ReadWrite))
        ui->view->appendPlainText("Puerto serial abierto\n");
        //qDebug("Puerto serial abierto USB\n");
    else
        ui->view->appendPlainText("Error de conexion con el puerto serial USB\n");
        //qDebug("Error de coexion con el puerto serial\n");
    serialPortUSB->setBaudRate(QSerialPort::Baud115200);
    serialPortUSB->setDataBits(QSerialPort::Data8);
    serialPortUSB->setStopBits(QSerialPort::OneStop);
    serialPortUSB->setParity(QSerialPort::NoParity);
    serialPortUSB->setFlowControl(QSerialPort::NoFlowControl);

    serialPortUSB->write("START COMMUNICATION\n");
    serialPortUSB->write("SPEED 1500\n");

    /*QString s = "BTR_EXIT";
    udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoBTR);
    s = "LF_EXIT";
    udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoLF);*/

}

void siviso::leerSerialDB9()
{
    char buffer[101];
    int nDatos;

    nDatos = serialPortDB9->read(buffer,100);
    buffer[nDatos] = '\0';
    ui->textTestGrap->appendPlainText(buffer);

    QString str;
    str=QString(buffer);
    double d;
    QString s;

    for(int x=0;x<str.size();x++){
        if(str[x]=='#'){
            catchSend = "";
            nBati = 1;
            batiP = "";
            batiS = "";
            batiT = "";
        }
        if(str[x]=='1'||str[x]=='2'||str[x]=='3'||str[x]=='4'||str[x]=='5'||str[x]=='6'||str[x]=='7'||str[x]=='8'||str[x]=='9'||str[x]=='0'||str[x]=='.'||str[x]=='-'||str[x]=='e'){
            if(!bDecimal){
                switch(nBati){
                case 1:
                    batiT += str[x];
                    break;
                case 2:
                    batiP += str[x];
                    break;
                case 3:
                    batiS += str[x];
                    break;
                }
            }
        }
        if(str[x]=='T'||str[x]=='P'||str[x]=='C'){
            nBati++;
        }
        if(str[x]=='$' && batiP != "" && batiT !="" && batiS != ""){
            d = batiP.toDouble();
            batiP = QString::number(d);
            d = batiT.toDouble();
            d *= 10;
            batiT = QString::number(d);
            catchSend = batiP + "," + batiT + "," + batiS + ";";
            ui->viewGPS->appendPlainText("esto enviaré a BTG: " + catchSend);
            udpsocket->writeDatagram(catchSend.toLatin1(),direccionApp,puertoBTG);
            s = "RP";
            udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoBTG);
        }
    }
}

void siviso::leerSerialGPS()
{
    char buffer[101];
    int nDatos;
    double latlong;

    nDatos = serialPortGPS->read(buffer,100);
    buffer[nDatos] = '\0';
    //ui->viewGPS->appendPlainText(buffer);

    QString str;
    str=QString(buffer);
    int n =str.size();
    //ui->textTestGrap->appendPlainText(QString::number(n));

    numCatchSend += n;
    for(int x=0;x<str.size();x++){
        if(str[x]=='$'){
            bCatchHeader = true;
            catchHeader = "";
            GPSt = "";
            GPSn = "";
            GPSw = "";
        }
        if(bCatchHeader){
            if(str[x]!=','){
                catchHeader += str[x];
            } else {
                bCatchHeader = false;
                if(catchHeader == "$GPGGA"){
                    bGPSt = true;
                }
            }
        } else if(bGPSt){
            if(str[x]!=','){
                GPSt += str[x];
            } else {
                GPSt = "";
                bGPSt = false;
                bGPSn = true;
            }
        } else if(bGPSn){
            if(str[x]!='N'){
                if(str[x]!=',')
                    GPSn += str[x];
            } else {
                latlong = GPSn.toDouble()/-100;
                ui->gpsLong->setNum(latlong);
                GPSn = "";
                bGPSn = false;
                bGPSw = true;
            }
        } else if(bGPSw){
            if(str[x]!='W'){
                if(str[x]!=',')
                    GPSw += str[x];
            } else {
                latlong = GPSw.toDouble()/100;
                ui->gpsLat->setNum(latlong);
                GPSw = "";
                bGPSw = false;
            }
        }
    }
}

void siviso::leerSerialUSB()
{
    char buffer[101];
    int nDatos;
    numCatchSend++;
    serialPortUSB->flush();
    nDatos = serialPortUSB->read(buffer,100);

    buffer[nDatos] = '\0';
    //ui->view->appendPlainText(buffer);

    QString str;
    str=QString(buffer);
    int n =str.size();
    //ui->textTestGrap->appendPlainText(QString::number(n));


    numCatchSend += n;
    for(int x=0;x<str.size();x++){
        if(str[x]=='1'||str[x]=='2'||str[x]=='3'||str[x]=='4'||str[x]=='5'||str[x]=='6'||str[x]=='7'||str[x]=='8'||str[x]=='9'||str[x]=='0'||str[x]==','||str[x]==';'){
            catchSend += str[x];
        }
        if(str[x]==';'){
            ui->textTestGrap->appendPlainText("esto enviare: "+catchSend);
            if(compGraf=="BTR")
                udpsocket->writeDatagram(catchSend.toLatin1(),direccionApp,puertoBTR);
            if(compGraf=="LF")
                udpsocket->writeDatagram(catchSend.toLatin1(),direccionApp,puertoLF);

            numCatchSend = 0;

            ui->textTestGrap->appendPlainText(catchSend);
            catchSend="";
        }
    }
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

void siviso::on_lf_clicked()
{
    QString s;
    s = "OFF";
    udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoBTR);
    udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoBTG);
    s = "ON";
    udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoLF);
    compGraf="LF";
    s = "RP";
    udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoLF);
    ui->setColorUp->setVisible(true);
    ui->setColorDw->setVisible(true);
}

void siviso::on_btr_clicked()
{
    QString s;
    s = "OFF";
    udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoBTG);
    udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoLF);
    s = "ON";
    udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoBTR);
    compGraf="BTR";
    s = "RP";
    udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoBTR);
    ui->setColorUp->setVisible(true);
    ui->setColorDw->setVisible(true);
}

void siviso::on_btg_clicked()
{
    QString s;
    s = "OFF";
    udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoBTR);
    udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoLF);
    s = "ON";
    udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoBTG);
    compGraf="BTG";
    s = "RP";
    udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoBTG);
    ui->setColorUp->setVisible(false);
    ui->setColorDw->setVisible(false);
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

void siviso::on_edo_mar_valueChanged(int arg1)
{
    mysignal->set_edo_mar(arg1);

    ui->view->appendPlainText("edo_mar: ");
    QString s = QString::number(arg1);
    ui->view->appendPlainText(s);
}

void siviso::on_prob_falsa_valueChanged(double arg1)
{
    mysignal->set_prob_falsa(arg1);

    ui->view->appendPlainText("prob_falsa: ");
    QString s = QString::number(arg1);
    ui->view->appendPlainText(s);
}

void siviso::on_prob_deteccion_valueChanged(double arg1)
{
    mysignal->set_prob_deteccion(arg1);

    ui->view->appendPlainText("prob_deteccion: ");
    QString s = QString::number(arg1);
    ui->view->appendPlainText(s);
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
    ui->textTestGrap->appendPlainText(s);\
}

void siviso::on_gan_sen_valueChanged(int arg1)
{
    mysignal->set_ganancia_sensor(arg1);

    ui->view->appendPlainText("ganancia_sensor: ");
    QString s = QString::number(arg1);
    ui->view->appendPlainText(s);
    QByteArray ba ="GAIN "+s.toLatin1()+"\n";
    serialPortUSB->write(ba);
}

//boton para enviar la informacion
void siviso::on_pushButton_send_clicked()
{
    //QString s = mysignal->send_to_sensor();               //guarda en la variable "s" lo que tiene en sensor
    QString s = "END COMMUNICATION";
    ui->view->appendPlainText("send: " + s);
    udpsocket->writeDatagram(s.toLatin1(),direccionSPP,puertoSPP);
    serialPortDB9->write("s");
    //serialPortUSB->write("END COMMUNICATION\n");
}



/*void siviso::on_pushButton_clicked()
{


}*/

void siviso::on_it_valueChanged(int arg1)
{
    mysignal->set_it(arg1);

    ui->view->appendPlainText("It: ");
    QString s = QString::number(arg1);
    ui->view->appendPlainText(s);
}



void siviso::on_rec_clicked()
{
    QString s;
    if(bRec){
        bRec=false;
        ui->rec->setText("Stop");
        s = "REC_ON";
        udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoREC);

    }else{
        bRec=true;
        ui->rec->setText("Rec");
        s = "REC_OFF";
        udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoREC);
    }
}

void siviso::on_play_clicked()
{
    if(bPlay){
        bPlay=false;
        ui->play->setText("Stop");
        proceso3->startDetached("pactl load-module module-loopback");
        //proceso3->startDetached("java -jar recSound.jar");
        //QSound::play("2016-09-14_14:27:16.wav");
    }else{
        bPlay=true;
        ui->play->setText("Play");
        proceso3->startDetached("pactl unload-module module-loopback");
    }
}

void siviso::on_setColorUp_valueChanged(int value)
{
    colorUp = value;
    if(colorUp <= colorDw){
        colorDw = colorUp-1;
        ui->setColorDw->setValue(colorDw);
    }

    QFile file("resource/colorUp.txt");
    if(file.open(QIODevice::WriteOnly)){
        QTextStream stream(&file);
        stream<<value;
    } else {
        qDebug();
    }
    file.close();

    QString s;
    s = "RP";
    if(compGraf=="BTR")
        udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoBTR);
    if(compGraf=="LF")
        udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoLF);
}

void siviso::on_setColorDw_valueChanged(int value)
{
    colorDw = value;
    if(colorDw >= colorUp){
        colorUp = colorDw+1;
        ui->setColorUp->setValue(colorUp);
    }

    QFile file("resource/colorDw.txt");
    if(file.open(QIODevice::WriteOnly)){
        QTextStream stream(&file);
        stream<<value;
    } else {
        qDebug();
    }
    file.close();

    QString s;
    s = "RP";
    if(compGraf=="BTR")
        udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoBTR);
    if(compGraf=="LF")
        udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoLF);
}

void siviso::on_closeJars_clicked()
{
    QString s;
    s = "EXIT";
    udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoBTR);
    udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoLF);
    udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoBTG);
}

void siviso::on_openJars_clicked()
{
    QString s;
    s = "EXIT";
    udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoBTR);
    udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoLF);
    udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoBTG);

    proceso1->startDetached("java -jar Lofar.jar");
    proceso2->startDetached("java -jar BTR.jar");
    proceso3->startDetached("java -jar Btg.jar");
}
