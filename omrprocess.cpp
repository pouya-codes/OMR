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
#include <thread>
#include <future>





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


cv::Mat OMRProcess::ProcessImage(cv::String imagePath,std::string pathOrginal,std::string pathProcessed,std::string pathError)  {
    const cv::Mat image = cv::imread(imagePath);
    if(image.empty())
    {
        std::cout << "can not open " << imagePath << "\n";
        return cv::Mat();
    }


    cv::Mat process_image = answerSheet->ProcessImage(image,ui->lineEditTableName->text(),pathOrginal,pathProcessed,pathError) ;
    return process_image ;

}

void OMRProcess::ProcessImagePath(std::string path,std::string pathOrginal,std::string pathProcessed,std::string pathError) {
    std::string inpath = path + "/*.jpg";
    std::vector<cv::String> fn;
    cv::glob(inpath, fn, false);
    size_t count = fn.size();
    if (ui->checkBoxMultiThread->isChecked()) {
        int num_threads = 8 ;
//        if (count<num_threads)
//            num_threads=count ;
        size_t filecounter ;
        for (filecounter = 0; filecounter<count; filecounter+=num_threads){
            if(count- filecounter <num_threads)
                break ;
            std::thread threads[num_threads];
            for (int t=0; t<num_threads; ++t)
                threads[t] = std::thread(&OMRProcess::ProcessImage,this,fn[filecounter+t],pathOrginal,pathProcessed,pathError);
            for (auto& th : threads) th.join();
            queryData() ;
            ui->tableView->scrollToBottom();
            QCoreApplication::processEvents();
        }
        int remainFiles = count-filecounter ;
        if (remainFiles!=0) {
                std::thread threads[remainFiles];
                for (int i=0; i<remainFiles; ++i)
                    threads[i] = std::thread(&OMRProcess::ProcessImage,this,fn[filecounter+i],pathOrginal,pathProcessed,pathError);
                for (auto& th : threads) th.join();
                //                ui->labelStatus->setText("تعداد کل تصاویر:"+ QString(count) + " باقی مانده "+ QString(std::abs(count-i) ));
                queryData() ;
                ui->tableView->scrollToBottom();
                QCoreApplication::processEvents();

        }

    }
    else {
        for (size_t i=0; i<count; i++){
            cv::Mat resultImage =ProcessImage(fn[i],pathOrginal,pathProcessed,pathError) ;
            if ( !resultImage.empty()) {
                cv::resize(resultImage, resultImage,cv::Size(ui->label->width(),ui->label->height())) ;
                QImage qt_img = ASM::cvMatToQImage( resultImage );
                ui->label->setPixmap(QPixmap::fromImage(qt_img));
                queryData() ;
                ui->tableView->scrollToBottom();
                QCoreApplication::processEvents();
            }
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

        ProcessImagePath(source.toStdString(),
                         createRecurciveDirectory(ui->lineEditOrginalPath->text().toStdString()) ,
                         createRecurciveDirectory(ui->lineEditProcessedPath->text().toStdString()),
                         createRecurciveDirectory(ui->lineEditErrorPath->text().toStdString()));

    }

    running=false ;

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
    QString orginal_path = query.value(0).toString();
    QString processed_path = query.value(1).toString();
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

    if (ui->checkBoxTwoChoices->isChecked()) {
        dataModel->setFilter("answers like '%*%'");
    }

    if (ui->checkBoxApsent->isChecked()) {
        dataModel->setFilter("LENGTH(REPLACE(answers, ' ', '')) > 0  AND  LENGTH(REPLACE(answers, ' ', '')) < 4");
    }

    if (ui->checkBoxLowColors->isChecked()) {
        dataModel->setFilter("maxblackness <= 75 and LENGTH(REPLACE(answers, ' ', '')) != 0");
    }

    dataModel->select();
    dataModel->removeColumn(2);
    dataModel->removeColumn(1);
    dataModel->removeColumn(4);
    dataModel->setHeaderData(0, Qt::Orientation::Horizontal, tr("ID"));
    dataModel->setHeaderData(1,  Qt::Orientation::Horizontal, tr("Code"));
    dataModel->setHeaderData(2,  Qt::Orientation::Horizontal, tr("Answers"));
    dataModel->setHeaderData(3,  Qt::Orientation::Horizontal, tr("Date"));

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
    QString queryString = "SELECT orginalFilePath,processedFilePath FROM " + ui->lineEditTableName->text() + " WHERE id = " + id ;
    query.exec(queryString);
    query.first() ;
    QString orginal_path = query.value(0).toString();
    QString processed_path = query.value(1).toString();
    //    QString barcode = query.value(2).toString();

    QString barcodenew = index.sibling(index.row(), 1).data().toString();

    QStringList temp = orginal_path.split('/') ;
    QString newOrginalFilePath = "" ;
    for (int var = 0; var < temp.length()-1; ++var) {
        newOrginalFilePath += temp.at(var) +"/" ;
    }
    newOrginalFilePath +=barcodenew +".jpg" ;

    temp = processed_path.split('/') ;
    QString newProcessedFilePath = "" ;
    for (int var = 0; var < temp.length()-1; ++var) {
        newProcessedFilePath += temp.at(var) +"/" ;
    }
    newProcessedFilePath +=barcodenew +".jpg" ;

    std::rename(orginal_path.toStdString().c_str(),newOrginalFilePath.toStdString().c_str()) ;
    std::rename(processed_path.toStdString().c_str(),newProcessedFilePath.toStdString().c_str()) ;


    queryString = "UPDATE " + ui->lineEditTableName->text() + " SET code ='" +barcodenew + "',orginalFilePath ='"+newOrginalFilePath + "',processedFilePath='" +newProcessedFilePath + "' WHERE id = " + id ; // update barcode
    query.exec(queryString) ;
    //    std::cout << id.toStdString()  << "-" << barcode.toStdString()  << "-" << barcodenew.toStdString()<<  "-" << queryString.toStdString() << std::endl ;
}

void OMRProcess::on_pushButton_3_clicked()
{
    QModelIndexList selection = ui->tableView->selectionModel()->selectedIndexes();
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "حذف اطلاعات","آیا موارد انتخاب شده حذف شود؟",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply==QMessageBox::Yes) {
        QString queryStringDelete = "DELETE FROM " + ui->lineEditTableName->text() ;
        QString queryStringIDS =" WHERE id = "  ;
        for(int i=0; i< selection.count(); i++)
        {
            QModelIndex index = selection.at(i);
            QString id = index.sibling(index.row(), 0).data().toString();
            queryStringIDS+= id ;
            if (i!= selection.count()-1)
                queryStringIDS+= " or id = " ;

        }
        if (ui->checkBoxOmitOrginalImage->isChecked() || ui->checkBoxOmitProcessedImage->isChecked()){
            QString queryStringPath = "SELECT orginalFilePath,processedFilePath FROM "+ ui->lineEditTableName->text() +queryStringIDS;
            QSqlQuery queryPath;
            queryPath.exec(queryStringPath) ;
            while (queryPath.next()) {
                QString orgpath = queryPath.value(0).toString();
                QString procpath = queryPath.value(1).toString();
                if(ui->checkBoxOmitOrginalImage->isChecked()) std::remove(orgpath.toStdString().c_str()) ;
                if(ui->checkBoxOmitProcessedImage->isChecked()) std::remove(procpath.toStdString().c_str()) ;
            };

        }
        QSqlQuery query;
        if (query.exec(queryStringDelete +queryStringIDS ))
            QMessageBox::information(this , "عملیات موفق","رکوردها با موفقیت حذف شد.") ;
        queryData();
    }



}

void OMRProcess::on_checkBoxTwoChoices_clicked()
{
    queryData();
}

void OMRProcess::on_checkBoxApsent_clicked()
{
    queryData();
}


void OMRProcess::on_checkBoxLowColors_clicked()
{
    queryData();
}

void OMRProcess::on_lineEditTableName_textChanged(const QString &arg1)
{
    ui->lineEditErrorPath->setText(ui->LineEditMain->text() +"/Error/"+arg1);
    ui->lineEditOrginalPath->setText(ui->LineEditMain->text() +"/Orginal/"+arg1);
    ui->lineEditProcessedPath->setText(ui->LineEditMain->text() +"/Processed/"+arg1);
}

void OMRProcess::on_LineEditMain_textChanged(const QString &arg1)
{
    ui->lineEditErrorPath->setText(arg1 +"/Error");
    ui->lineEditOrginalPath->setText(arg1 +"/Orginal");
    ui->lineEditProcessedPath->setText(arg1 +"/Processed");

}
