#include "sqlscriptrunner.h"
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>

SqlScriptRunner::SqlScriptRunner(QObject *parent) : QObject(parent) {}

bool SqlScriptRunner::executeScript(const QString &script, QSqlDatabase &db, QString &error)
{
    emit executionStarted();

    QString expandedScript = expandScriptIncludes(script, error);
    if (!error.isEmpty()) {
        emit executionFinished(false);
        return false;
    }

    QStringList queries = expandedScript.split(';', Qt::SkipEmptyParts);
    int totalQueries = queries.size();
    int currentQuery = 0;

    foreach (const QString &queryStr, queries) {
        QString trimmedQuery = queryStr.trimmed();
        if (trimmedQuery.isEmpty()) continue;

        QSqlQuery query(db);
        if (!query.exec(trimmedQuery)) {
            error = QString("Error executing query: %1\n%2")
            .arg(query.lastError().text())
                .arg(trimmedQuery);
            qDebug() << "Query error:" << error;
            emit executionFinished(false);
            return false;
        }

        currentQuery++;
        emit progressChanged((currentQuery * 100) / totalQueries);
    }

    emit executionFinished(true);
    return true;
}

QString SqlScriptRunner::expandScriptIncludes(const QString &script, QString &error)
{
    QString result = script;
    QRegularExpression includeRegex("@'(.*?)'");
    QRegularExpressionMatchIterator it = includeRegex.globalMatch(script);

    while (it.hasNext()) {
        QRegularExpressionMatch match = it.next();
        QString filePath = match.captured(1);

        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            error = QString("Cannot open included script file: %1").arg(filePath);
            return QString();
        }

        QTextStream in(&file);
        QString includedScript = in.readAll();
        file.close();

        result.replace(match.captured(0), includedScript);
    }

    return result;
}

bool SqlScriptRunner::executeScriptFromFile(const QString &filePath, QSqlDatabase &db, QString &error)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        error = QString("Cannot open script file: %1").arg(filePath);
        return false;
    }

    QTextStream in(&file);
    QString script = in.readAll();
    file.close();

    return executeScript(script, db, error);
}
