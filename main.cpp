#include "mainwindow.h"
#include "connectiondialog.h"
#include <QApplication>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    ConnectionDialog dlg;

    w.setWindowIcon(QIcon("C:\\Users\\KM-Git\\Downloads\\MyDeveloper\\icon_bd.ico"));

    QObject::connect(&dlg, &ConnectionDialog::connected, [&](const ConnectionParams &params) {
        QSqlDatabase db = QSqlDatabase::addDatabase("QODBC");
        QString connStr = QString("DRIVER={Oracle in instantclient_23_7};SERVER=%1,%2;DATABASE=%3;UID=%4;PWD=%5;")
                              .arg(params.hostName).arg(params.port)
                              .arg(params.serviceName).arg(params.userName).arg(params.password);
        db.setDatabaseName(connStr);

        if (db.open()) {
            w.setDatabase(db);
            w.show();
        } else {
            QMessageBox::critical(nullptr, "Error", "Connection failed: " + db.lastError().text());
        }
    });

    QObject::connect(&dlg, &ConnectionDialog::useLocalDb, [&]() {
        QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
        db.setDatabaseName("local_db.sqlite");

        if (db.open()) {
            QSqlQuery q(db);
            q.exec("CREATE TABLE IF NOT EXISTS LECTURER(id INTEGER PRIMARY KEY, name TEXT)");
            w.setDatabase(db);
            w.show();
        } else {
            QMessageBox::critical(nullptr, "Error", "Cannot open local database");
        }
    });

    if (dlg.exec() == QDialog::Rejected) {
        return 0;
    }

    return a.exec();
}
