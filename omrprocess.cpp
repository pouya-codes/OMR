#include "omrprocess.h"
#include "ui_omrprocess.h"
#include "asmOpenCV.h"
#include <QtSql/qsqldatabase.h>
#include <QItemSelectionModel>
#include <QSqlQuery>
#include <sqlite3.h>
#include <QSqlQueryModel>
#include <zbar.h>
#include <QFileDialog>
#include <boost/algorithm/string.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
#include <QMessageBox>
#include <QSortFilterProxyModel>





OMRProcess::OMRProcess(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OMRProcess)
{
    ui->setupUi(this);
    running = false ;

    getTableNames();


}


OMRProcess::~OMRProcess()
{
    delete ui;
}

void OMRProcess::setAnswerSheet(AnswerSheet* answerSheet_) {
    answerSheet = answerSheet_ ;
}




void OMRProcess::ProcessImage(std::string path,std::string pathOrginal,std::string pathProcessed,std::string pathError) {
    std::string inpath = path + "/*.jpg";
    std::vector<cv::String> fn;
    cv::glob(inpath, fn, false);
    size_t count = fn.size();
    for (size_t i=0; i<count; i++){
        const cv::Mat image = cv::imread(fn[i]);
        if(image.empty())
        {
            std::cout << "can not open " << fn[i] << "\n";
            continue;
        }

        cv::Mat process_image = answerSheet->ProcessImage(image,ui->lineEditTableName->text(),pathOrginal,pathProcessed,pathError) ;
        if(! process_image.empty())
        {
            cv::resize(process_image, process_image,cv::Size(ui->label->width(),ui->label->height())) ;
            QImage qt_img = ASM::cvMatToQImage( process_image );
            ui->label->setPixmap(QPixmap::fromImage(qt_img));

            queryData() ;
            QCoreApplication::processEvents();

        }
    }
}

void OMRProcess::getTableNames() {
    ui->comboBoxDbNames->clear();
    QSqlQuery query;
    query.exec("SELECT name FROM sqlite_master WHERE type = 'table' and name!='sqlite_sequence'");
    while (query.next()) {
        QString tb_name = query.value(0).toString();
        ui->comboBoxDbNames->addItem(tb_name);
    };

}

std::string createRecurciveDirectory(std::string path) {
    std::vector<std::string> strs;
    boost::split(strs, path, boost::is_any_of("/"));
    std::string outpath = "" ;
    for (uint idx = 0; idx < strs.size(); ++idx) {
        outpath+=strs[idx] +"/" ;
        if(!boost::filesystem::exists(outpath)) {
            boost::filesystem::create_directory(outpath) ;
        }
    }
    return outpath ;


}

void OMRProcess::on_pushButton_clicked()
{
    running=true ;
    QSqlQuery query;
    query.prepare("SELECT name FROM sqlite_master WHERE type = 'table' and name = ?");
    query.bindValue(0, ui->lineEditTableName->text());
    query.exec() ;
    if (!query.first()) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(this, "ایجاد جدول جدید","جدول موجود نیست آیا ایجاد شود؟",
                                      QMessageBox::Yes|QMessageBox::No);
        if (reply==QMessageBox::Yes) {
            if (answerSheet->createTable(ui->lineEditTableName->text())) {
                QMessageBox::information(this , "عملیات موفق","جدول با موفقیت ایجاد شد.") ;
            }
            getTableNames();
            ui->comboBoxDbNames->setCurrentIndex(ui->comboBoxDbNames->findText(ui->lineEditTableName->text())) ;
        }
        else {
            running= false ;
            return ;
        }
    }
    QString source = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
                                                       "/home",
                                                       QFileDialog::ShowDirsOnly
                                                       | QFileDialog::DontResolveSymlinks);

    if(source!="") {

        ProcessImage(source.toStdString(),
                     createRecurciveDirectory(ui->lineEditOrginalPath->text().toStdString()) ,
                     createRecurciveDirectory(ui->lineEditProcessedPath->text().toStdString()),
                     createRecurciveDirectory(ui->lineEditErrorPath->text().toStdString()));
        running=false ;
    }



}

void OMRProcess::on_pushButton_2_clicked()
{
    QString outfile = QFileDialog::getSaveFileName(this, tr("Save Results"),
                                                   "/home","Text Files (*.txt)");

    if(outfile!="") {
        std::string outfilename = outfile.toStdString() +".txt";
        std::ofstream out ;
        out.open(outfilename);

        QString querytxt ;
        querytxt = "SELECT code,answers FROM " + ui->lineEditTableName->text() ;
        QSqlQuery query;
        query.exec(querytxt) ;
        while (query.next()) {
            QString code = query.value(0).toString();
            QString answers = query.value(1).toString();
            out << code.toStdString() << "," << answers.toStdString() << std::endl ;
        };
        out.close();
        QMessageBox::information(this , "عملیات موفق","نتایج با موفقیت ذخیره شد.") ;

    }

}

void OMRProcess::setPicture (int id) {
    if (running) return ;

    QString querytxt ;
    querytxt = "SELECT orginalFilePath,processedFilePath FROM " + ui->lineEditTableName->text() + " WHERE id = ?";
    QSqlQuery query;
    query.prepare(querytxt);
    query.bindValue(0, id);
    query.exec() ;
    query.first() ;
    QString processed_path = query.value(0).toString();
    QString orginal_path = query.value(1).toString();
    cv::Mat image = ui->radioButtonProcessed->isChecked() ? cv::imread(processed_path.toStdString().c_str()) :cv::imread(orginal_path.toStdString().c_str()) ;
    //    std::cout << processed_path.toStdString() << id << std::endl ;
    if(! image.empty())
    {
        cv::resize(image, image,cv::Size(ui->label->width(),ui->label->height())) ;
        QImage qt_img = ASM::cvMatToQImage( image );
        ui->label->setPixmap(QPixmap::fromImage(qt_img));
    }
    else {
        ui->label->setText("تصویر یافت نشد");
    }


}

void OMRProcess::on_tableView_clicked(const QModelIndex &index)
{

    int id = index.sibling(index.row(), 0).data().toInt();
    setPicture(id) ;

}

void OMRProcess::on_tableView_activated(const QModelIndex &index)
{

    int id = index.sibling(index.row(), 0).data().toInt();
    setPicture(id) ;
}

void OMRProcess::on_tableView_entered(const QModelIndex &index)
{

    int id = index.sibling(index.row(), 0).data().toInt();
    setPicture(id) ;
}



void OMRProcess::on_comboBoxDbNames_currentIndexChanged(const QString &arg1)
{
    if (!running)ui->lineEditTableName->setText(arg1);
    queryData();

}


void OMRProcess::on_radioButtonProcessed_clicked(bool checked)
{
    if(checked) ui->radioButtonOrginal->setChecked(false);
}

void OMRProcess::on_radioButtonOrginal_clicked(bool checked)
{
    if(checked) ui->radioButtonProcessed->setChecked(false);
}

void OMRProcess::queryData() {


    dataModel = new QSqlTableModel();
    connect(dataModel, SIGNAL( dataChanged(QModelIndex,QModelIndex,QVector<int>)), SLOT(handleAfterEdit(QModelIndex,QModelIndex,QVector<int>)));

    dataModel->setTable(ui->lineEditTableName->text());
    dataModel->setEditStrategy(QSqlTableModel::OnManualSubmit);

    if (ui->checkBoxInvalid->isChecked())
        dataModel->setFilter(QString("code = ' ' "));

    if (ui->lineEditSearch->text()!="") {
        QString filter_txt = "code like '" + ui->lineEditSearch->text() + "%'";
        dataModel->setFilter(filter_txt);
    }

    if (ui->checkBoxDuplicated->isChecked()) {
        QString filter_txt = "code in (select code FROM "+ ui->lineEditTableName->text() +" GROUP BY code HAVING count(*) >1)" ;
        dataModel->setFilter(filter_txt);
    }

    dataModel->select();
    dataModel->removeColumn(2);
    dataModel->removeColumn(1);
    dataModel->setHeaderData(0, Qt::Orientation::Horizontal, tr("ID"));
    dataModel->setHeaderData(1,  Qt::Orientation::Horizontal, tr("Code"));
    dataModel->setHeaderData(2,  Qt::Orientation::Horizontal, tr("Answers"));

    ui->tableView->setModel(dataModel);

}

void OMRProcess::on_checkBoxDuplicated_clicked()
{
    queryData();
}

void OMRProcess::on_checkBoxInvalid_clicked()
{
    queryData();
}

void OMRProcess::on_pushButtonOmitTable_clicked()
{

    QMessageBox::StandardButton reply;
    QString message = "آیا جدول " + ui->lineEditTableName->text() + " حدف شود؟ این عملیات قابل بازگشت نیست." ;
    reply = QMessageBox::question(this, "حذف جدول",message,
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply==QMessageBox::Yes) {
        answerSheet->deleteTable(ui->lineEditTableName->text());
        getTableNames();
        queryData();
    }

}

void OMRProcess::on_lineEditSearch_textChanged(const QString &arg1)
{
    queryData();
}
void OMRProcess::handleAfterEdit( QModelIndex index ,QModelIndex index2 ,QVector<int> vector) {
    QString id = index.sibling(index.row(), 0).data().toString();
    QSqlQuery query;
    QString queryString = "SELECT * FROM " + ui->lineEditTableName->text() + " WHERE id = " + id ;
    query.exec(queryString);
    query.first() ;
    QString orginal_path = query.value(1).toString();
    QString processed_path = query.value(2).toString();
    QString barcode = query.value(3).toString();
    query.exec() ;
    //TODO reanme orginal and processed file , update barcode
    QString barcodenew = index.sibling(index.row(), 1).data().toString();
    std::cout << id.toStdString()  << "-" << barcode.toStdString()  << "-" << barcodenew.toStdString()<< std::endl ;
}

void OMRProcess::on_pushButton_3_clicked()
{
    QModelIndexList selection = ui->tableView->selectionModel()->selectedIndexes();
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "حذف اطلاعات","آیا موارد انتخاب شده حذف شود؟",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply==QMessageBox::Yes) {
        QString queryString = "DELETE FROM " + ui->lineEditTableName->text() +" WHERE id = " ;
        for(int i=0; i< selection.count(); i++)
        {
            QModelIndex index = selection.at(i);
            QString id = index.sibling(index.row(), 0).data().toString();
            queryString+= id ;
            if (i!= selection.count()-1)
                queryString+= " or id = " ;

        }
        QSqlQuery query;
        if (query.exec(queryString))
            QMessageBox::information(this , "عملیات موفق","رکوردها با موفقیت حذف شد.") ;
        queryData();
    }



}
