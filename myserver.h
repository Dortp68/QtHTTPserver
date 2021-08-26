#ifndef MYSERVER_H
#define MYSERVER_H
#include <QTcpServer>
#include <QObject>
#include <QTcpSocket>
#include <QIODevice>
#include <QFile>
#include <QDir>
#include <QBuffer>
#include <algorithm>


class MyServer : public QTcpServer
{
    Q_OBJECT
public:
    MyServer(quint16 nPort = 1234, QObject * parent = 0);
    void incomingConnection(qintptr handle);
    bool fileLoad(QString &path, QTcpSocket *socket);//функция для отправки файлов клиенту
    void GetData(const QString &path1, const QString &path2);
signals:
public slots:
    void onReadyRead();//Слот для получения информации от клиента
    void onDisconnected();
private:
    QMap<QString, QString> dbase = { {"admin", "123"}, {"user", "qwe1"} };//База данных с логинами и паролями пользователей
    QVector<QString> students;
};

#endif // MYSERVER_H
