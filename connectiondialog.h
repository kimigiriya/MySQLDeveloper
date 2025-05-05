#ifndef CONNECTIONDIALOG_H
#define CONNECTIONDIALOG_H

#include <QDialog>
#include <QSettings>

struct ConnectionParams {
    QString hostName;
    QString userName;
    QString password;
    QString serviceName;
    QString dsnName;
    int port = 1521;
    bool useDsn = false;
};

class QCheckBox;
class QLabel;
class QLineEdit;
class QPushButton;

class ConnectionDialog : public QDialog {
    Q_OBJECT

public:
    explicit ConnectionDialog(QWidget *parent = nullptr);
    ConnectionParams getConnectionParams() const;

signals:
    void connected(const ConnectionParams& params);
    void useLocalDb();

private slots:
    void saveSettings();
    void testConnection();
    void connectToDatabase();
    void useLocalDatabase();
    void updateVisibility();

private:
    void loadSettings();
    bool testDsnConnection();
    bool testDirectConnection();

    QCheckBox *useDsnCheckBox;
    QLabel *dsnLabel;
    QLabel *hostNameLabel;
    QLabel *userNameLabel;
    QLabel *passwordLabel;
    QLabel *serviceNameLabel;
    QLabel *portLabel;

    QLineEdit *dsnEdit;
    QLineEdit *hostNameEdit;
    QLineEdit *userNameEdit;
    QLineEdit *passwordEdit;
    QLineEdit *serviceNameEdit;
    QLineEdit *portEdit;

    QPushButton *saveButton;
    QPushButton *testButton;
    QPushButton *connectButton;
    QPushButton *localDbButton;
};

#endif // CONNECTIONDIALOG_H
