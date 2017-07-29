#include "createform.h"
#include "ui_createform.h"
#include "QFileDialog"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/ml.hpp>
#include "asmOpenCV.h"
#include "answersheet.h"
#include "omrprocess.h"



QImage qt_img;
cv::Mat img,resized_img_cv ;
AnswerSheet* answerSheet ;




CreateForm::CreateForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateForm)
{
    ui->setupUi(this);
    std::cout << CV_VERSION << std::endl ;
}

CreateForm::~CreateForm()
{
    delete ui;
}

void CreateForm::on_pb_openImage_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,
           tr("Open Address Book"), "/home/pouya/",
           tr("Images (*.jpg);;All Files (*)"));
    if(fileName!="") {
        cv::Mat img = cv::imread(fileName.toStdString().c_str()) ;
        if(!img.empty())
        {
            answerSheet = new AnswerSheet(img) ;

            cv::resize(img, resized_img_cv,cv::Size(ui->label_image->width(),ui->label_image->height())) ;
            qt_img = ASM::cvMatToQImage( resized_img_cv );
            ui->label_image->setPixmap(QPixmap::fromImage(qt_img));


        }

    }
}

void CreateForm::on_pb_removeColors_clicked()

{
    cv::resize(answerSheet->RemoveColors() , resized_img_cv,cv::Size(ui->label_image->width(),ui->label_image->height())) ;
    qt_img = ASM::cvMatToQImage(resized_img_cv );
    ui->label_image->setPixmap(QPixmap::fromImage(qt_img));
    answerSheet->RemoveColors();
}

void CreateForm::on_pb_findEyes_clicked()
{
    answerSheet->DetectEyes(std::stoi(ui->le_EyePad->text().toStdString())) ;
    cv::resize(answerSheet->getImage(), resized_img_cv,cv::Size(ui->label_image->width(),ui->label_image->height())) ;
    qt_img = ASM::cvMatToQImage(resized_img_cv);
    ui->label_image->setPixmap(QPixmap::fromImage(qt_img));



}

void CreateForm::on_pushButton_clicked()
{
    int referenceEye = std::stoi(ui->le_refrenceEye->text().toStdString()) ;
    int distanceWidth = std::stoi(ui->le_distanceRefwidth->text().toStdString()) ;
    int distanceHeight = std::stoi(ui->le_distanceRefHeight->text().toStdString()) ;
    int choiceWidth = std::stoi(ui->le_choiceWidth->text().toStdString()) ;
    int choiceHeigh = std::stoi(ui->le_choiceHeight->text().toStdString()) ;
    int choiceNumber = std::stoi(ui->le_numberOfChoices->text().toStdString()) ;
    int distanceChoiceChoice = std::stoi(ui->le_distanceChoiceChoice->text().toStdString()) ;
    int numberOfQuestions = std::stoi(ui->le_numberOfquestions->text().toStdString()) ;
    int columnDistance = std::stoi(ui->le_distanceColumns->text().toStdString()) ;

    int barcodeX = std::stoi(ui->le_barcode_x->text().toStdString()) ;
    int barcodeY = std::stoi(ui->le_barcode_y->text().toStdString()) ;
    int barcodeWidth = std::stoi(ui->le_barcode_width->text().toStdString()) ;
    int barcodeHeight = std::stoi(ui->le_barcode_height->text().toStdString()) ;

    cv::Mat ans = answerSheet->DrawChoices(referenceEye,distanceWidth,distanceHeight,choiceWidth,choiceHeigh,
                                           choiceNumber,distanceChoiceChoice,numberOfQuestions,columnDistance,
                                           barcodeX,barcodeY,barcodeWidth,barcodeHeight);
    cv::resize(ans, resized_img_cv,cv::Size(ui->label_image->width(),ui->label_image->height())) ;
    qt_img = ASM::cvMatToQImage(resized_img_cv);
    ui->label_image->setPixmap(QPixmap::fromImage(qt_img));

}

void CreateForm::on_pushButton_2_clicked()
{
    OMRProcess omrprocess ;
    omrprocess.setModal(true);
    omrprocess.exec();
    omrprocess.setAnswerSheet(answerSheet);

}
