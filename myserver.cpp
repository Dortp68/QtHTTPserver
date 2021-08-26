#include "myserver.h"

MyServer::MyServer(quint16 nPort, QObject * parent) : QTcpServer(parent)
{
    if(listen(QHostAddress::Any, nPort))// прослушивание порта
    {
        qDebug() << "Listening";
        GetData("student_file_1.txt", "student_file_2.txt");
    }
    else
    {
        qDebug() << "Error" << errorString();
    }

}

void MyServer::incomingConnection(qintptr handle)
{
    QTcpSocket* socket = new QTcpSocket();
    socket->setSocketDescriptor(handle);// установка дескриптора сокета

    connect(socket, SIGNAL(readyRead()), this, SLOT(onReadyRead()));//Когда поступает новый блок данных, вызывается сигнал readyRead(), который соединён со слотом onReadyRead()
    connect(socket, SIGNAL(disconnected()), this, SLOT(onDisconnected()));// соедниения сигнала disconnected() со слотом onDisconnected()

}

bool MyServer::fileLoad(QString &path, QTcpSocket *socket) //передача файла клиенту
{
    if(path == "/students")
    {
        QTextStream os(socket);
        os.setAutoDetectUnicode(true);
        os << "HTTP/1.0 200 Ok\r\n"
                             "Content-Type: text/html; charset=\"utf-8\"\r\n"
                             "\r\n"
                             "<h1>Students</h1>\n"
                                 "<body>\n"
                                  "<p>\n";
        for(auto items : students)
        {
            qDebug() << items;
            os <<items << "<br>\n";
        }
        os<<"</p>\n"<<"</body>\n";
        socket->close();
        return true;
    }
    if (path.indexOf(".") < 0)
    {
        path += ".html";
    }
    QFile file(QDir::toNativeSeparators("..\\Httpserver" + path));

    if(file.open(QIODevice::ReadOnly))
    {
        QByteArray q = file.readAll();
        QString response = "HTTP/1.1 200 OK\r\n\r\n";
        socket->write(response.toLatin1());
        socket->write(q);
        file.close();
        return true;
    }
    else
    {
        qDebug() << "Error, file does not open";
        QString response = "HTTP/1.1 404 Not found\r\n\r\n";
        socket->write(response.toLatin1());
        return false;
    }
}

void MyServer::GetData(const QString &path1, const QString &path2)
{
    QFile file1(QDir::toNativeSeparators("..\\Httpserver\\" + path1));
    QFile file2(QDir::toNativeSeparators("..\\Httpserver\\" + path2));
    if(file1.open(QIODevice::ReadOnly) && file2.open(QIODevice::ReadOnly))
    {
        QVector<QString> list1;
        while(!file1.atEnd())
        {
            list1.append(file1.readLine());
        }
        QString list2;
        while(!file2.atEnd())
        {
            list2 = file2.readLine();
            auto result = std::find_if(list1.begin(), list1.end(),
                                       [&list2](QString tmp){
                    auto iter1 = std::find(tmp.begin(), tmp.end(), ' ');
                    auto iter2 = std::find(list2.begin(), list2.end(), ' ');
                    iter1 = std::next(iter1);
                    iter2 = std::next(iter2);
                    return(std::equal(iter1, tmp.end(), iter2, list2.end()));});
            if(result == list1.end())
            {
                list1.append(list2);
            }
        }
        file1.close();
        file2.close();
        std::sort(list1.begin(), list1.end(),[](QString str1, QString str2){
            auto iter1 = std::find(str1.begin(), str1.end(), ' ');
            auto iter2 = std::find(str2.begin(), str2.end(), ' ');
            iter1 = std::next(iter1);
            iter2 = std::next(iter2);
            return (*iter1) < (*iter2);
        });
        students = list1;
    }
    else
    {
        qDebug() << "Error, file does not open";
    }
}

void MyServer::onReadyRead()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (socket->canReadLine())
    {
        QStringList All = QString(socket->readAll()).split("\r\n"); // считывается весь блок данных целиком и разбивается на строки
        QStringList tokens = All[0].split(" ");// в первой строке содержится запрос
        if (tokens[0] == "GET")//обработка get запросов
        {
            fileLoad(tokens[1], socket);            
        }
        else if (tokens[0] == "POST")//обработка post запросов
        {
            QString username = All[All.size()-1].split("&")[0].split("=")[1];
            QString password = All[All.size()-1].split("&")[1].split("=")[1];
            QMap<QString, QString>::iterator i = dbase.find(username);
            if((i != dbase.end()) && (i.value() == password))
            {
                QString req = "\\success.png";
                fileLoad(req, socket);
            }
            else
            {
                fileLoad(tokens[1], socket);

                QString error = "Incorrect login or password";
                socket->write(error.toLatin1());
            }
        }
    }
    socket->disconnectFromHost();//закрытие сокета

}

void MyServer::onDisconnected()//закрытие сокета
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    socket->close();
    socket->deleteLater();
    qDebug() << "close socket";
}
