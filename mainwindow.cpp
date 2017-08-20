#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "answersheet.h"
#include "createform.h"
#include <QtSql/qsqldatabase.h>
#include <QSqlQuery>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    createFrom = std::make_shared<CreateForm>(this) ;
    createFrom->show();

}

void MainWindow::on_pushButton_2_clicked()
{
    //    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    //    db.setDatabaseName("OMR_Results.db");
    //    bool ok = db.open();
    //    if(ok) {
    //        QSqlQuery query;
    //        std::cout<< "OK" << std::endl;
    //        query.exec("create table results "
    //                  "(id integer PRIMARY KEY AUTOINCREMENT, "
    //                  "orginalFilePath varchar(300), "
    //                  "processedFilePath varchar(300), "
    //                  "code varchar(30) ,"
    //                  "answers varchar(300),date varchar(50),maxblackness integer) "
    //                   );
    //        db.commit();
    //        db.close();

    //    }
}

void MainWindow::on_pushButton_3_clicked()
{
    exit(0) ;
}
