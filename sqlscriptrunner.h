#ifndef SQLSCRIPTRUNNER_H
#define SQLSCRIPTRUNNER_H

#include <QObject>
#include <QSqlDatabase>
#include <QString>

class SqlScriptRunner : public QObject
{
    Q_OBJECT
public:
    explicit SqlScriptRunner(QObject *parent = nullptr);

    bool executeScript(const QString &script, QSqlDatabase &db, QString &error);
    bool executeScriptFromFile(const QString &filePath, QSqlDatabase &db, QString &error);

signals:
    void executionStarted();
    void executionFinished(bool success);
    void progressChanged(int percent);

private:
    QString expandScriptIncludes(const QString &script, QString &error);
};

#endif // SQLSCRIPTRUNNER_H
