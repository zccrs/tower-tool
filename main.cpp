#include <QCoreApplication>
#include <QCommandLineParser>
#include <QNetworkProxy>
#include <QDate>
#include <QRegularExpression>
#include <QFile>
#include <QProcess>

#include "weekly.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    a.setApplicationName("tower auto commit");
    a.setApplicationVersion("1.0.0");

//    QNetworkProxy proxy;

//    proxy.setHostName("127.0.0.1");
//    proxy.setPort(8888);
//    proxy.setType(QNetworkProxy::HttpProxy);
//    proxy.setApplicationProxy(proxy);

    QCommandLineOption option_email(QStringList() << "e" << "email", "email address.", "email");
    QCommandLineOption option_pass(QStringList() << "p" << "password", "password.", "password");
    QCommandLineOption option_data(QStringList() << "d" << "data", "weekly data (format=JSON).", "data");
    QCommandLineOption option_file(QStringList() << "f" << "file-path", "the contents of the file as weekly data (text-encoding=UTF-8, format=JSON).", "file-path");
    QCommandLineOption option_command(QStringList() << "c" << "command", "exec command, it returns the contents as weekly data (text-encoding=UTF-8, format=JSON).", "command");
    QCommandLineOption option_date(QStringList() << "D" << "date", "date, format=yyyy-M-d, default is system current date.", "date");
    QCommandLineOption option_wi(QStringList() << "w" << "week-index", "get week index by date.");

    QCommandLineParser parser;

    option_date.setDefaultValue(QDate::currentDate().toString("yyyy-M-d"));

    parser.setApplicationDescription(a.applicationName());
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOption(option_email);
    parser.addOption(option_pass);
    parser.addOption(option_data);
    parser.addOption(option_command);
    parser.addOption(option_file);
    parser.addOption(option_date);
    parser.addOption(option_wi);

    parser.process(a);

    if(parser.isSet(option_wi)) {
        int year;
        int week_index = QDate::fromString(parser.value(option_date), "yyyy-M-d").weekNumber(&year);

        const QString str = QString::number(year) + " " + QString::number(week_index);

        PrintError() << str;

        return 0;
    }

    Weekly weekly;

    if(parser.isSet(option_email) && parser.isSet(option_pass)) {
        QByteArray data;

        if(parser.isSet(option_data)) {
            data = parser.value(option_data).toUtf8();
        } else if(parser.isSet(option_command)) {
            QProcess process;

            process.start(parser.value(option_command));

            if(process.waitForFinished()) {
                const QByteArray d = process.readAllStandardOutput();

                data = d;
            } else {
                PrintError() << process.errorString();

                return -1;
            }
        } else if(parser.isSet(option_file)) {
            QFile file(parser.value(option_file));

            if(file.open(QIODevice::ReadOnly)) {
                data = file.readAll();
            } else {
                PrintError() << file.errorString();

                return -1;
            }
        }

        if(weekly.commitWeekly(parser.value(option_email), parser.value(option_pass), data, parser.value(option_date))) {
            return a.exec();
        }
    }

    //Weekly weekly;

    //weekly.commitWeekly("zhangjide@linuxdeepin.com", "button-yspa1314", "");

    //const QByteArray data = "name=\"report_guid\" value=\"d627de58a898480e99920254d5a751e4\"\n\n<input type=\"hidden\" name=\"report_guid\" value=\"d627de58a898480e99920254d5a751e4\" />";

//    QFile file("/home/zhang/test_tower_html.txt");

//    QByteArray data;

//    if(file.open(QIODevice::ReadOnly)) {
//        data = file.readAll();

//        //qDebug() << QString::fromUtf8(data);
//    }

//    QRegularExpression rx("name=\"(\\w+?)\"\\s*value=\"(\\w+?)\".*?\\s*?.*?必填");
//    QRegularExpressionMatchIterator match = rx.globalMatch(QString::fromUtf8(data));

//    //qDebug() << QString(data).indexOf(rx);
//    qDebug() << rx.isValid();
//    while(match.hasNext()) {
//        qDebug() << match.next().capturedTexts();
//    }

    return -1;
}

