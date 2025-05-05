#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlQueryModel>
#include <QTextEdit>
#include <QPushButton>
#include <QTableView>
#include <QLabel>
#include <QStatusBar>
#include <QHeaderView>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>

class SqlScriptRunner;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void setDatabase(const QSqlDatabase &db);

private slots:
    void executeQuery();
    void onScriptExecutionFinished(bool success);

private:
    QTextEdit *queryInput;
    QPushButton *executeButton;
    QTableView *tableView;
    QSqlQueryModel *queryModel;
    QLabel *statusBarLabel;
    QSqlDatabase database;
    SqlScriptRunner *scriptRunner;

    void setupUI();
    void setupConnections();
};

#endif // MAINWINDOW_H
