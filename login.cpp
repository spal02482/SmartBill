#include "login.h"
#include "ui_login.h"

/*
 * Login Constructor:
 * 1) Login Window is constructed.
 * 2) Here the default database connection is Added and opened and is used throughout the duration of the application.
 * 3) Placeholder text for username and password is set.
*/
Login::Login(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
    this->setFixedSize(670, 370);
    qDebug() << "Login: Constructor: " << "Login Window constructed";

    db = std::make_unique<QSqlDatabase>(QSqlDatabase::addDatabase("QSQLITE"));
    db->setDatabaseName(QCoreApplication::applicationDirPath() + "/translations/smartbill.db");
    qDebug() << "Login: Constructor: " << "Database Connection Added";

    if (db->open()) {
        qDebug() << "Login: Constructor: " << "Connection opened to Database";
    }
    else {
        qDebug() << "Login: Constructor: " << "Couldn't open database connection";
    }

    ui->UserName->setPlaceholderText(tr("Username Here"));
    ui->PassWord->setPlaceholderText(tr("Password Here"));
}

/*
 * Login Destructor:
 * 1) Login window is destroyed.
 * 2) Connection to the database is closed and removed.
 */
Login::~Login()
{
    db->close();
    qDebug() << "Login: Destructor: " << "Connection to Database closed";

    QSqlDatabase::removeDatabase(QSqlDatabase::database().connectionName());
    qDebug() << "Login: Destructor: " <<  "Database Connection Removed";

    delete ui;
    qDebug() << "Login: Destructor: " <<  "Login Window destroyed";
}

/*
 * This slot is used for checking the credentials of the user.
 * Message 'Wrong Password' is shown, when the password is wrong.
 * In case the user doesn't exist in the database, login fails.
 *
 * If the credentials are right, smartbill window is created, and
 * Login window is closed but not deleted, it still resides in the
 * memory.
 */

void Login::on_LoginBtn_clicked()
{
    QString username = ui->UserName->text();
    QString password = ui->PassWord->text();
    qDebug() << "Login: on_LoginBtn_clicked(): " << username << " " << password;

    QSqlQuery query;
    query.prepare("SELECT PassWord FROM UserInfo WHERE UserName = ?");
    query.addBindValue(username);
    bool querySuccess = query.exec();

    if (querySuccess == true) {
        query.next();
        if (query.isValid()) {
            if (query.value(0).toString() == password) {
                smartbill = new SmartBill();
                smartbill->showMaximized();
                smartbill->show();
                this->close();
            }
            else {
                QMessageBox::critical(this, tr("Login"), tr("Wrong Password"));
            }
        }
        else {
            QMessageBox::critical(this, tr("Login"), tr("Login failed"));
        }
    }
    else {
         qDebug() << "Login: on_LoginBtn_clicked(): " << query.lastError();
    }
    query.finish();
}

void Login::on_changeCredPushButton_clicked()
{
    changeCredentials = new QDialog(this);
    changeCredentials->setWindowModality(Qt::ApplicationModal);
    changeCredentials->resize(255, 240);
    changeCredentials->setFixedSize(255, 240);

    QLabel *label_1, *label_2, *label_3, *label_4;
    label_1 = new QLabel("Old User Name", changeCredentials);
    oldusername = new QLineEdit(changeCredentials);
    label_2 = new QLabel("Old Password", changeCredentials);
    oldpassword = new QLineEdit(changeCredentials);
    label_3 = new QLabel("New User Name", changeCredentials);
    newusername = new QLineEdit(changeCredentials);
    label_4 = new QLabel("New Password", changeCredentials);
    newpassword = new QLineEdit(changeCredentials);
    submitInfo = new QPushButton("submit", changeCredentials);

    label_1->setGeometry(5, 5, 90, 15);
    label_2->setGeometry(5, 55, 90, 15);
    label_3->setGeometry(5, 105, 90, 15);
    label_4->setGeometry(5, 155, 90, 15);
    oldusername->setGeometry(5, 25, 245, 25);
    oldpassword->setGeometry(5, 75, 245, 25);
    newusername->setGeometry(5, 125, 245, 25);
    newpassword->setGeometry(5, 175, 245, 25);
    submitInfo->setGeometry(5, 205, 70, 30);

    oldpassword->setEchoMode(QLineEdit::Password);
    newpassword->setEchoMode(QLineEdit::Password);

    QObject::connect(submitInfo, SIGNAL(clicked()), this, SLOT(submitLoginInfo()));
    changeCredentials->open();
}

void Login::submitLoginInfo()
{
    QString oldusername_s, oldpassword_s, newusername_s, newpassword_s;

    oldusername_s = oldusername->text();
    oldpassword_s = oldpassword->text();
    newusername_s = newusername->text();
    newpassword_s = newpassword->text();

    QSqlQuery query;
    query.prepare("SELECT PassWord from UserInfo WHERE UserName = ?");
    query.addBindValue(oldusername_s);
    query.exec();

    if (!query.next()) {
        qDebug() << "Login: submitLoginInfo(): " << query.lastError();
    }

    if (query.value(0).toString() == oldpassword_s) {
        query.finish();
        query.prepare("UPDATE UserInfo SET UserName = ?, PassWord = ?");
        query.addBindValue(newusername_s);
        query.addBindValue(newpassword_s);
        query.exec();
    }

    changeCredentials->close();
}
