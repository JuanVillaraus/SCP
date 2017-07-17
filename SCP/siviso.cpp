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
    proceso4 = new QProcess(this);
    proceso5 = new QProcess(this);
    proceso6 = new QProcess(this);
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
    //udpsocket->bind(QHostAddress::LocalHost, puertolocal);
    udpsocket->bind(5002, QUdpSocket::ShareAddress);
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
    ui->textTestGrap->setVisible(false);
    ui->view->setVisible(false);
    ui->pushButton_info->setVisible(false);
    ui->closeJars->setVisible(false);
    ui->openJars->setVisible(false);
    ui->startCom->setVisible(false);
    ui->endCom->setVisible(false);
    ui->textSend->setVisible(false);
    ui->send->setVisible(false);
    ui->OpenPort->setVisible(false);
    ui->sendr->setVisible(false);

    serialPortUSB->write("GAIN 3\n");

    colorUp=255;
    colorDw=0;
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
    //proceso3->startDetached("java -jar Btg.jar");
    proceso4->startDetached("java -jar ConexionPP.jar");

    //ui->btOpenPort->setVisible(false);
    //ui->toolButton->setVisible(false);
    ui->btg->setDisabled(true);


    serialPortDB9->setPortName("/dev/ttyS0");
    if(serialPortDB9->open(QIODevice::ReadWrite))
        ui->view->appendPlainText("Puerto serial db9 abierto\n");
    else
        ui->view->appendPlainText("Error de conexion con el puerto serial db9\n");
    serialPortDB9->setBaudRate(QSerialPort::Baud9600);
    serialPortDB9->setDataBits(QSerialPort::Data8);
    serialPortDB9->setStopBits(QSerialPort::OneStop);
    serialPortDB9->setParity(QSerialPort::NoParity);
    serialPortDB9->setFlowControl(QSerialPort::NoFlowControl);

    serialPortGPS->setPortName("/dev/ttyS2");
    if(serialPortGPS->open(QIODevice::ReadWrite))
        ui->view->appendPlainText("Puerto serial GPS abierto\n");
    else
        ui->view->appendPlainText("Error de conexion con el puerto serial GPS\n");
    serialPortGPS->setBaudRate(QSerialPort::Baud4800);
    serialPortGPS->setDataBits(QSerialPort::Data8);
    serialPortGPS->setStopBits(QSerialPort::OneStop);
    serialPortGPS->setParity(QSerialPort::NoParity);
    serialPortGPS->setFlowControl(QSerialPort::NoFlowControl);
    serialPortUSB->setFlowControl(QSerialPort::NoFlowControl);

    //serialPortUSB->setPortName("/dev/ttyUSB1");
    /*if(serialPortUSB->open(QIODevice::ReadWrite))
    {
        ui->view->appendPlainText("Puerto serial abierto\n");

        serialPortUSB->setBaudRate(QSerialPort::Baud115200);
        serialPortUSB->setDataBits(QSerialPort::Data8);
        serialPortUSB->setStopBits(QSerialPort::OneStop);
        serialPortUSB->setParity(QSerialPort::NoParity);
        serialPortUSB->setFlowControl(QSerialPort::NoFlowControl);
        serialPortUSB->write("START COMMUNICATION\n");
        serialPortUSB->write("SPEED 1500\n");
    }
    else
    {
        ui->view->appendPlainText("Error de conexion con el puerto serial USB\n");
    }*/


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
    udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoComPP);
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
    proceso4->close();
    proceso5->close();
    proceso6->close();
}

void siviso::on_toolButton_clicked()
{
    if(bToolButton){
        bToolButton=false;
        ui->textTestGrap->setVisible(false);
        ui->view->setVisible(false);
        ui->pushButton_info->setVisible(false);
        ui->closeJars->setVisible(false);
        ui->openJars->setVisible(false);
        ui->startCom->setVisible(false);
        ui->endCom->setVisible(false);
        ui->textSend->setVisible(false);
        ui->send->setVisible(false);
        ui->OpenPort->setVisible(false);
        ui->sendr->setVisible(false);
    }else{
        bToolButton=true;
        ui->textTestGrap->setVisible(true);
        ui->view->setVisible(true);
        ui->pushButton_info->setVisible(true);
        ui->closeJars->setVisible(true);
        ui->openJars->setVisible(true);
        ui->startCom->setVisible(true);
        ui->endCom->setVisible(true);
        ui->textSend->setVisible(true);
        ui->send->setVisible(true);
        ui->OpenPort->setVisible(true);
        ui->sendr->setVisible(true);
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

void siviso::on_btOpenPort_clicked()
{
    QString s = "OFF";
    udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoBTR);
    udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoBTG);
    udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoLF);
    proceso6->startDetached("nautilus /home/siviso/repositorio/SCP/build-SCP-Desktop_Qt_5_6_0_GCC_64bit-Debug/resource/audio/");

    this->close();

    //util = createUnoService("org.universolibre.util.EasyDev");
    //archivo = util.getSelectedFiles("/home/mau/docs", False, Array("*.pdf", "PDF"));
    //util.msgbox(archivo);

    //QString nombreArchivo = QFileDialog::getOpenFileName(this, "Abrir Archivos", "/home", "Archivos PDF (*.pdf)");
    //nombreArchivo.prepend("file:///");
    //QDesktopServices::openUrl(QUrl(nombreArchivo));



    /*serialPortDB9->setPortName("/dev/ttyS0");
    if(serialPortDB9->open(QIODevice::ReadWrite))
        ui->view->appendPlainText("Puerto serial db9 abierto\n");
    else
        ui->view->appendPlainText("Error de conexion con el puerto serial db9\n");
    serialPortDB9->setBaudRate(QSerialPort::Baud9600);
    serialPortDB9->setDataBits(QSerialPort::Data8);
    serialPortDB9->setStopBits(QSerialPort::OneStop);
    serialPortDB9->setParity(QSerialPort::NoParity);
    serialPortDB9->setFlowControl(QSerialPort::NoFlowControl);

    serialPortGPS->setPortName("/dev/ttyS2");
    if(serialPortGPS->open(QIODevice::ReadWrite))
        ui->view->appendPlainText("Puerto serial GPS abierto\n");
    else
        ui->view->appendPlainText("Error de conexion con el puerto serial GPS\n");
    serialPortGPS->setBaudRate(QSerialPort::Baud4800);
    serialPortGPS->setDataBits(QSerialPort::Data8);
    serialPortGPS->setStopBits(QSerialPort::OneStop);
    serialPortGPS->setParity(QSerialPort::NoParity);
    serialPortGPS->setFlowControl(QSerialPort::NoFlowControl);

    serialPortUSB->setPortName("/dev/ttyUSB1");
    if(serialPortUSB->open(QIODevice::ReadWrite))
        ui->view->appendPlainText("Puerto serial abierto\n");
    else
        ui->view->appendPlainText("Error de conexion con el puerto serial USB\n");
    serialPortUSB->setBaudRate(QSerialPort::Baud115200);
    serialPortUSB->setDataBits(QSerialPort::Data8);
    serialPortUSB->setStopBits(QSerialPort::OneStop);
    serialPortUSB->setParity(QSerialPort::NoParity);
    serialPortUSB->setFlowControl(QSerialPort::NoFlowControl);

    serialPortUSB->write("START COMMUNICATION\n");
    serialPortUSB->write("SPEED 1500\n");*/
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
        QString info = datagram.data();
        ui->textTestGrap->appendPlainText(" port-> " + QString("%1").arg(senderPort));
        ui->textTestGrap->appendPlainText(info);
        //s = " ";

        QString s;
        if(info == "runBTG"){
            puertoBTG = senderPort;
            s = "OFF";
            udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoBTG);
        }else if(info == "runBTR"){
            puertoBTR = senderPort;
            s = "LONG";
            udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoBTR);
            s = "OFF";
            udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoBTR);
        }else if(info == "runLF"){
            puertoLF = senderPort;
            s = "LONG";
            udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoLF);
            s = "OFF";
            udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoLF);
        }else if(info == "runREC"){
            puertoREC = senderPort;
        } else if(puertoSPP == senderPort){
            udpsocket->writeDatagram(info.toLatin1(),direccionApp,puertoBTR);
            udpsocket->writeDatagram(info.toLatin1(),direccionApp,puertoLF);
        } else if(info == "runConxPP"){
            puertoComPP = senderPort;
            s = "USB_DW";
            udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoComPP);
        }else if(info == "BTR"){
            serialPortUSB->write("BTR\n");
        }else if(info == "LOFAR"){
            serialPortUSB->write("LOFAR\n");
        } else if(info == "USB"){
            serialPortUSB->setPortName("/dev/ttyUSB1");
            if(serialPortUSB->open(QIODevice::ReadWrite)){
                s = "USB_UP";
                udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoComPP);
                ui->view->appendPlainText("Puerto serial abierto\n");
                serialPortUSB->setBaudRate(QSerialPort::Baud115200);
                serialPortUSB->setDataBits(QSerialPort::Data8);
                serialPortUSB->setStopBits(QSerialPort::OneStop);
                serialPortUSB->setParity(QSerialPort::NoParity);
                serialPortUSB->setFlowControl(QSerialPort::NoFlowControl);
            }else{
                s = "USB_DW";
                udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoComPP);
                ui->view->appendPlainText("Error de conexion con el puerto serial USB\n");
            }
        } else if(info == "CONX"){
            serialPortUSB->write("START COMMUNICATION\n");
            serialPortUSB->write("SPEED 1500\n");
        } else if(info == "SENSOR"){
            serialPortUSB->write("VOLTAJE\n");
        }else if(info == "REC"){
            ui->rec->setText("Grabar");
            ui->rec->setDisabled(false);
        }
    }
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
            d = d/.025;
            batiP = QString::number(d);
            d = batiT.toDouble();
            d *= 10;
            batiT = QString::number(d);
            catchSend = batiP + "," + batiT + "," + batiS + ";";
            ui->textTestGrap->appendPlainText("esto enviaré a BTG: " + catchSend);
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

    QString sCom;
    QString catchCmd;
    double carga;

    buffer[nDatos] = '\0';
    ui->view->appendPlainText(buffer);

    QString str;
    str=QString(buffer);
    int n =str.size();
    //ui->textTestGrap->appendPlainText(QString::number(n));


    numCatchSend += n;
    for(int x=0;x<str.size();x++){
        if(str[x]=='1'||str[x]=='2'||str[x]=='3'||str[x]=='4'||str[x]=='5'||str[x]=='6'||str[x]=='7'||str[x]=='8'||str[x]=='9'||str[x]=='0'||str[x]==','||str[x]=='.'){
            if(!bSensor){
                catchSend += str[x];
            }else{
                catchCarga += str[x];
            }
        }
        if(str[x]==';'){
            if(bSensor){
                carga = ((((catchCarga.toDouble()*100)/25.2)-80)*100)/20;
                ui->textTestGrap->appendPlainText("el voltaje es:" + catchCarga);
                ui->textTestGrap->appendPlainText("el % delvoltaje es:" + static_cast<int>(carga));
                if(carga>0){
                    ui->carga->setNum(carga);
                }else{
                    ui->carga->setNum(00.00);
                }
            } else {
                sCom="INFO";
                udpsocket->writeDatagram(sCom.toLatin1(),direccionApp,puertoComPP);
                sCom="";
                catchCarga += str[x];
                ui->textTestGrap->appendPlainText("esto enviare: "+catchSend);
                if(compGraf=="BTR")
                    udpsocket->writeDatagram(catchSend.toLatin1(),direccionApp,puertoBTR);
                if(compGraf=="LF")
                    udpsocket->writeDatagram(catchSend.toLatin1(),direccionApp,puertoLF);
                numCatchSend = 0;
                catchSend="";
            }
        } else if(str[x]=='#'){
            bSensor = true;
            catchCarga = "";
        } else if(str[x]=='!'||str[x]=='A'||str[x]=='C'||str[x]=='D'||str[x]=='E'||str[x]=='F'||str[x]=='H'||str[x]=='I'||str[x]=='K'||str[x]=='L'||str[x]=='M'||str[x]=='N'||str[x]=='O'||str[x]=='P'||str[x]=='R'||str[x]=='S'||str[x]=='T'||str[x]=='U'||str[x]=='V'){
            if(str[x]!='!'){
                catchCmd += str[x];
            } else {
                sCom="INFO";
                udpsocket->writeDatagram(sCom.toLatin1(),direccionApp,puertoComPP);
                sCom="";
                ui->textTestGrap->appendPlainText("comando: " + catchCmd);
                if(catchCmd == "STARTOK"){
                    sCom="CONX_UP";
                    udpsocket->writeDatagram(sCom.toLatin1(),direccionApp,puertoComPP);
                    sCom="";
                } else if(catchCmd == "OK"){
                    sCom="CONF";
                    udpsocket->writeDatagram(sCom.toLatin1(),direccionApp,puertoComPP);
                    sCom="";
                /*} else if(catchCmd == "FINISHCOMMUNICATION"){
                    sCom="CONX_DW";
                    udpsocket->writeDatagram(sCom.toLatin1(),direccionApp,puertoComPP);
                    sCom="";*/
                } else if(catchCmd == "VELOCIDADOK"){
                    sCom="CONX_UP";
                    udpsocket->writeDatagram(sCom.toLatin1(),direccionApp,puertoComPP);
                    sCom="";
                } else if(catchCmd == "COMMUNICATIONERRORP"){

                } else if(catchCmd == "COMMUNICATIONERRORA"){

                }
                catchCmd = "";
            }
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

void siviso::on_it_valueChanged(int arg1)
{
    mysignal->set_it(arg1);

    ui->view->appendPlainText("It: ");
    QString s = QString::number(arg1);
    ui->view->appendPlainText(s);
}



void siviso::on_rec_clicked()
{
    ui->rec->setText("Grabando");
    ui->rec->setDisabled(true);
    QString s = "REC";
    udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoComPP);
}

void siviso::on_play_clicked()
{
    if(bPlay){
        bPlay=false;
        ui->play->setText("Reproducir");
        proceso3->startDetached("pactl unload-module module-loopback");
    }else{
        bPlay=true;
        ui->play->setText("Alto");
        proceso3->startDetached("pactl load-module module-loopback");
        //proceso3->startDetached("java -jar recSound.jar");
        //QSound::play("2016-09-14_14:27:16.wav");
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
    udpsocket->writeDatagram(s.toLatin1(),direccionApp,puertoComPP);
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
    //proceso3->startDetached("java -jar Btg.jar");
}

void siviso::on_startCom_clicked()
{
    serialPortUSB->write("START COMMUNICATION P\n");
    serialPortUSB->write("START COMMUNICATION A\n");
}

void siviso::on_endCom_clicked()
{
    serialPortUSB->write("END COMMUNICATION\n");
}

void siviso::on_send_clicked()
{
    QString s;
    s = ui->textSend->text();
    serialPortUSB->write(s.toLatin1());
    ui->textSend->clear();
    //QByteArray datagram = "Broadcast message " + s.toLatin1();
    s+="\n";
    serialPortUSB->write(s.toLatin1());
    //udpsocket->writeDatagram(datagram.data(), datagram.size(),QHostAddress::Broadcast, 5002);
}

void siviso::on_OpenPort_clicked()
{
    serialPortDB9->setPortName("/dev/ttyS0");
    if(serialPortDB9->open(QIODevice::ReadWrite))
        ui->view->appendPlainText("Puerto serial db9 abierto\n");
    else
        ui->view->appendPlainText("Error de conexion con el puerto serial db9\n");
    serialPortDB9->setBaudRate(QSerialPort::Baud9600);
    serialPortDB9->setDataBits(QSerialPort::Data8);
    serialPortDB9->setStopBits(QSerialPort::OneStop);
    serialPortDB9->setParity(QSerialPort::NoParity);
    serialPortDB9->setFlowControl(QSerialPort::NoFlowControl);

    serialPortGPS->setPortName("/dev/ttyS2");
    if(serialPortGPS->open(QIODevice::ReadWrite))
        ui->view->appendPlainText("Puerto serial GPS abierto\n");
    else
        ui->view->appendPlainText("Error de conexion con el puerto serial GPS\n");
    serialPortGPS->setBaudRate(QSerialPort::Baud4800);
    serialPortGPS->setDataBits(QSerialPort::Data8);
    serialPortGPS->setStopBits(QSerialPort::OneStop);
    serialPortGPS->setParity(QSerialPort::NoParity);
    serialPortGPS->setFlowControl(QSerialPort::NoFlowControl);

    serialPortUSB->setPortName("/dev/ttyUSB1");
    if(serialPortUSB->open(QIODevice::ReadWrite))
        ui->view->appendPlainText("Puerto serial abierto\n");
    else
        ui->view->appendPlainText("Error de conexion con el puerto serial USB\n");
    serialPortUSB->setBaudRate(QSerialPort::Baud115200);
    serialPortUSB->setDataBits(QSerialPort::Data8);
    serialPortUSB->setStopBits(QSerialPort::OneStop);
    serialPortUSB->setParity(QSerialPort::NoParity);
    serialPortUSB->setFlowControl(QSerialPort::NoFlowControl);

    serialPortUSB->write("START COMMUNICATION\n");
    serialPortUSB->write("SPEED 1500\n");
}

void siviso::on_sendr_clicked()
{
    QString s;
    s = ui->textSend->text();
    serialPortUSB->write(s.toLatin1());
    ui->textSend->clear();
    //QByteArray datagram = "Broadcast message " + s.toLatin1();
    s+="\n\r";
    serialPortUSB->write(s.toLatin1());
}

void siviso::on_vol_dw_clicked()
{
    proceso3->startDetached("amixer sset Master 5%-");
}

void siviso::on_vol_up_clicked()
{
    proceso3->startDetached("amixer sset Master 5%+");
}
