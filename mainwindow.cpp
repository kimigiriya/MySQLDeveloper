#include "mainwindow.h"
#include "sqlscriptrunner.h"
#include <QVBoxLayout>
#include <QSqlQuery>
#include <QDebug>
#include <QSqlError>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    queryInput(new QTextEdit(this)),
    executeButton(new QPushButton("Execute Query", this)),
    tableView(new QTableView(this)),
    queryModel(new QSqlQueryModel(this)),
    statusBarLabel(new QLabel(this)),
    scriptRunner(new SqlScriptRunner(this))
{
    setupUI();
    setupConnections();
}

MainWindow::~MainWindow()
{
    delete queryModel;
}

void MainWindow::setupUI()
{
    setWindowTitle("Database GUI");

    // Настройка таблицы
    tableView->setModel(queryModel);
    tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Interactive);
    tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    tableView->setSelectionBehavior(QAbstractItemView::SelectRows);

    // Основной layout
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(new QLabel("SQL Query:", this));
    layout->addWidget(queryInput);
    layout->addWidget(executeButton);
    layout->addWidget(tableView);

    // Центральный виджет
    QWidget *centralWidget = new QWidget(this);
    centralWidget->setLayout(layout);
    setCentralWidget(centralWidget);

    // Status bar
    statusBar()->addWidget(statusBarLabel);
    statusBarLabel->setText("Disconnected");

    // Изначально кнопка неактивна
    executeButton->setEnabled(false);
}

void MainWindow::setupConnections()
{
    connect(executeButton, &QPushButton::clicked, this, &MainWindow::executeQuery);
    connect(scriptRunner, &SqlScriptRunner::executionFinished,
            this, &MainWindow::onScriptExecutionFinished);
}

void MainWindow::setDatabase(const QSqlDatabase &db)
{
    database = db;
    executeButton->setEnabled(database.isOpen());
    statusBarLabel->setText(database.isOpen() ? "Connected" : "Disconnected");
}

void MainWindow::executeQuery()
{
    if (!database.isOpen()) {
        QMessageBox::warning(this, "Error", "Database is not connected!");
        return;
    }

    QString queryText = queryInput->toPlainText().trimmed();
    if (queryText.isEmpty()) {
        QMessageBox::warning(this, "Error", "Query is empty!");
        return;
    }

    QString error;
    if (!scriptRunner->executeScript(queryText, database, error)) {
        QMessageBox::critical(this, "Error", error);
    }
}

void MainWindow::onScriptExecutionFinished(bool success)
{
    if (success) {
        QString queryText = queryInput->toPlainText().trimmed();

        if (queryText.startsWith("SELECT", Qt::CaseInsensitive)) {
            queryModel->setQuery(queryText, database);

            if (queryModel->lastError().isValid()) {
                QMessageBox::critical(this, "Error",
                                      "Failed to display results: " +
                                          queryModel->lastError().text());
            } else {
                tableView->resizeColumnsToContents();
                statusBarLabel->setText(QString("Query OK. Rows: %1").arg(queryModel->rowCount()));
            }
        } else {
            statusBarLabel->setText("Command executed successfully");
        }
    }
}
