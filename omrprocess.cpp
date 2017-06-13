#include "omrprocess.h"
#include "ui_omrprocess.h"
#include "asmOpenCV.h"
#include <QtSql/qsqldatabase.h>
#include <QSqlQuery>
#include <sqlite3.h>
#include <QSqlQueryModel>
#include <zbar.h>
#include <QFileDialog>

OMRProcess::OMRProcess(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OMRProcess)
{
    ui->setupUi(this);
}

OMRProcess::~OMRProcess()
{
    delete ui;
}

void OMRProcess::setAnswerSheet(AnswerSheet* answerSheet_) {
    answerSheet = answerSheet_ ;
}

void OMRProcess::on_pushButton_clicked()
{

//    QString source = QFileDialog::getExistingDirectory(this, tr("Open Directory"),
//                                                "/home",
//                                                QFileDialog::ShowDirsOnly
//                                                | QFileDialog::DontResolveSymlinks);
//    if(source!="") {
        std::vector<cv::String> fn;
        cv::glob("/home/pouya/omr2/*.jpg", fn, false);

        size_t count = fn.size();

        for (size_t i=0; i<count; i++){

            cv::Mat image = cv::imread(fn[i]);
            if(image.empty())
            {
                std::cout << "can not open " << fn[i] << endl;

            }

            cv::Mat process_image = answerSheet->ProcessImage(image) ;
            if(! process_image.empty())
            {

                cv::resize(process_image, image,cv::Size(ui->label->width(),ui->label->height())) ;
                QImage qt_img = ASM::cvMatToQImage( image );
                ui->label->setPixmap(QPixmap::fromImage(qt_img));

                QSqlQueryModel * model = new QSqlQueryModel();


                QSqlQuery query;
                query.exec("SELECT id,code,answers FROM results");
                model->setQuery(query);
                ui->tableView->setModel(model);

            }



        }
//    }



}

void OMRProcess::on_pushButton_2_clicked()
{


}

