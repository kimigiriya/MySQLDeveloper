#include "connectiondialog.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCheckBox>
#include <QMessageBox>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSettings>
#include <QIntValidator>
#include <QDebug>

ConnectionDialog::ConnectionDialog(QWidget *parent) : QDialog(parent) {
    setWindowTitle(tr("Database Connection"));
    setMinimumWidth(400);

    // Инициализация элементов интерфейса
    useDsnCheckBox = new QCheckBox(tr("Use Data Source Name (DSN)"), this);
    useDsnCheckBox->setChecked(true);

    dsnLabel = new QLabel(tr("DSN Name:"), this);
    hostNameLabel = new QLabel(tr("Host Name:"), this);
    userNameLabel = new QLabel(tr("User Name:"), this);
    passwordLabel = new QLabel(tr("Password:"), this);
    serviceNameLabel = new QLabel(tr("Database Name:"), this);
    portLabel = new QLabel(tr("Port:"), this);

    dsnEdit = new QLineEdit(this);
    hostNameEdit = new QLineEdit(this);
    userNameEdit = new QLineEdit(this);
    passwordEdit = new QLineEdit(this);
    passwordEdit->setEchoMode(QLineEdit::Password);
    serviceNameEdit = new QLineEdit(this);
    portEdit = new QLineEdit(QString::number(1521), this);
    portEdit->setValidator(new QIntValidator(1, 65535, this));

    saveButton = new QPushButton(tr("Save"), this);
    testButton = new QPushButton(tr("Test"), this);
    connectButton = new QPushButton(tr("Connect"), this);
    localDbButton = new QPushButton(tr("Local DB"), this);

    // Настройка layout
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout();

    formLayout->addRow(useDsnCheckBox);
    formLayout->addRow(dsnLabel, dsnEdit);
    formLayout->addRow(hostNameLabel, hostNameEdit);
    formLayout->addRow(userNameLabel, userNameEdit);
    formLayout->addRow(passwordLabel, passwordEdit);
    formLayout->addRow(serviceNameLabel, serviceNameEdit);
    formLayout->addRow(portLabel, portEdit);

    QHBoxLayout *buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(saveButton);
    buttonLayout->addWidget(testButton);
    buttonLayout->addWidget(connectButton);
    buttonLayout->addWidget(localDbButton);

    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);

    // Загрузка настроек и обновление интерфейса
    loadSettings();
    updateVisibility();

    // Подключение сигналов
    connect(saveButton, &QPushButton::clicked, this, &ConnectionDialog::saveSettings);
    connect(testButton, &QPushButton::clicked, this, &ConnectionDialog::testConnection);
    connect(connectButton, &QPushButton::clicked, this, &ConnectionDialog::connectToDatabase);
    connect(localDbButton, &QPushButton::clicked, this, &ConnectionDialog::useLocalDatabase);
    connect(useDsnCheckBox, &QCheckBox::stateChanged, this, &ConnectionDialog::updateVisibility);
}

ConnectionParams ConnectionDialog::getConnectionParams() const {
    ConnectionParams params;
    params.useDsn = useDsnCheckBox->isChecked();
    params.dsnName = dsnEdit->text();
    params.hostName = hostNameEdit->text();
    params.userName = userNameEdit->text();
    params.password = passwordEdit->text();
    params.serviceName = serviceNameEdit->text();
    params.port = portEdit->text().toInt();
    return params;
}

void ConnectionDialog::saveSettings() {
    QSettings settings("MyOrg", "DBApp");
    ConnectionParams params = getConnectionParams();

    settings.setValue("useDsn", params.useDsn);
    settings.setValue("dsnName", params.dsnName);
    settings.setValue("hostName", params.hostName);
    settings.setValue("userName", params.userName);
    settings.setValue("password", params.password);
    settings.setValue("serviceName", params.serviceName);
    settings.setValue("port", params.port);

    QMessageBox::information(this, tr("Settings Saved"), tr("Connection settings saved successfully."));
}

void ConnectionDialog::testConnection() {

    bool success = useDsnCheckBox->isChecked() ? testDsnConnection() : testDirectConnection();

    if (success) {
        QMessageBox::information(this, tr("Connection Successful"),
                                 tr("Successfully connected to the database!"));
    }
}

bool ConnectionDialog::testDsnConnection() {
    //qDebug() << "TNS_ADMIN=" << qEnvironmentVariable("TNS_ADMIN");
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC", "test_connection");
    QString connectionString = QString("DRIVER={Oracle in instantclient_23_7};DSN=%1;UID=%2;PWD=%3")
                                   .arg(dsnEdit->text())
                                   .arg(userNameEdit->text())
                                   .arg(passwordEdit->text());
    db.setDatabaseName(connectionString);

    if (!db.open()) {
        QMessageBox::critical(this, tr("Connection Failed"),
                              tr("Failed to connect using DSN:\n%1").arg(db.lastError().text()));
        QSqlDatabase::removeDatabase("test_connection");
        return false;
    }

    db.close();
    QSqlDatabase::removeDatabase("test_connection");
    return true;
}

bool ConnectionDialog::testDirectConnection() {
    QSqlDatabase db = QSqlDatabase::addDatabase("QODBC", "test_connection");
    ConnectionParams params = getConnectionParams();

    QString connectionString = QString("DRIVER={Oracle in instantclient_23_7};"
                                       "SERVER=(DESCRIPTION=(ADDRESS=(PROTOCOL=TCP)"
                                       "(HOST=82.179.14.185)(PORT=1521))"
                                       "(CONNECT_DATA=(SERVICE_NAME=nmics)));"
                                       "UID=stud14;PWD=stud14;");
    db.setDatabaseName(connectionString);

    if (!db.open()) {
        QMessageBox::critical(this, tr("Connection Failed"),
                              tr("Failed to connect directly:\n%1").arg(db.lastError().text()));
        QSqlDatabase::removeDatabase("test_connection");
        return false;
    }

    db.close();
    QSqlDatabase::removeDatabase("test_connection");
    return true;
}

void ConnectionDialog::connectToDatabase() {
    emit connected(getConnectionParams());
    accept();
}

void ConnectionDialog::useLocalDatabase() {
    emit useLocalDb();
    accept();
}

void ConnectionDialog::loadSettings() {
    QSettings settings("MyOrg", "DBApp");

    useDsnCheckBox->setChecked(settings.value("useDsn", true).toBool());
    dsnEdit->setText(settings.value("dsnName").toString());
    hostNameEdit->setText(settings.value("hostName").toString());
    userNameEdit->setText(settings.value("userName").toString());
    passwordEdit->setText(settings.value("password").toString());
    serviceNameEdit->setText(settings.value("serviceName").toString());
    portEdit->setText(settings.value("port", "1521").toString());

    updateVisibility();
}

void ConnectionDialog::updateVisibility() {
    bool useDsn = useDsnCheckBox->isChecked();

    dsnLabel->setVisible(useDsn);
    dsnEdit->setVisible(useDsn);

    hostNameLabel->setVisible(!useDsn);
    hostNameEdit->setVisible(!useDsn);
    serviceNameLabel->setVisible(!useDsn);
    serviceNameEdit->setVisible(!useDsn);
    portLabel->setVisible(!useDsn);
    portEdit->setVisible(!useDsn);
}
