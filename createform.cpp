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
#include <QSettings>



QImage qt_img;
cv::Mat img,resized_img_cv ;
AnswerSheet* answerSheet ;
QSettings MySettings;

CreateForm::CreateForm(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CreateForm)
{

    ui->setupUi(this);
    ui->comboBoxBarcode->addItem("تشخیص خودکار") ;
    //    this->setFixedSize(window()->maximumWidth()*2,window()->maximumHeight()*2);
}

CreateForm::~CreateForm()
{
    delete ui;
}

void CreateForm::on_pb_openImage_clicked()
{

    const QString DEFAULT_DIR_KEY("answerShit_dir");



    QString fileName = QFileDialog::getOpenFileName(
                this, "Select a file", MySettings.value(DEFAULT_DIR_KEY).toString(),"Images (*.jpg);;All Files (*)");

    if(fileName!="") {
        QDir CurrentDir;
        MySettings.setValue(DEFAULT_DIR_KEY,
                            CurrentDir.absoluteFilePath(fileName));

        cv::Mat img = cv::imread(fileName.toStdString().c_str()) ;
        if(!img.empty())
        {
            answerSheet = new AnswerSheet(img) ;

            cv::resize(img, resized_img_cv,cv::Size(ui->label_image->width(),ui->label_image->height())) ;
            qt_img = ASM::cvMatToQImage( resized_img_cv );
            ui->label_image->setPixmap(QPixmap::fromImage(qt_img));
            ui->pb_removeColors->setEnabled(true);
            ui->frameBarcode->setEnabled(true);
            ui->frameEyes->setEnabled(true);
            ui->frameQuestions->setEnabled(true);
            ui->pushButton_2->setEnabled(true);
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
    answerSheet->DetectEyes(ui->le_EyePad->text().toInt(),ui->le_darkness->text().toInt()) ;
    cv::resize(answerSheet->getImage(), resized_img_cv,cv::Size(ui->label_image->width(),ui->label_image->height())) ;
    qt_img = ASM::cvMatToQImage(resized_img_cv);
    ui->label_image->setPixmap(QPixmap::fromImage(qt_img));
}

void CreateForm::on_pushButton_clicked()
{
    int referenceEye = ui->le_refrenceEye->text().toInt() ;
    int distanceWidth = ui->le_distanceRefwidth->text().toInt() ;
    int distanceHeight = ui->le_distanceRefHeight->text().toInt() ;
    int choiceWidth = ui->le_choiceWidth->text().toInt() ;
    int choiceHeigh = ui->le_choiceHeight->text().toInt() ;
    int choiceNumber = ui->le_numberOfChoices->text().toInt() ;
    int distanceChoiceChoice = ui->le_distanceChoiceChoice->text().toInt() ;
    int numberOfQuestions = ui->le_numberOfquestions->text().toInt() ;
    int columnDistance = ui->le_distanceColumns->text().toInt() ;

    int barcodeX = ui->le_barcode_x->text().toInt() ;
    int barcodeY = ui->le_barcode_y->text().toInt() ;
    int barcodeWidth = ui->le_barcode_width->text().toInt() ;
    int barcodeHeight = ui->le_barcode_height->text().toInt() ;

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

void CreateForm::on_radioButton_clicked(bool checked)
{
    if(checked) {
        ui->radioButton_2->setChecked(false);
        ui->radioButton_3->setChecked(false);
    }
}

void CreateForm::on_radioButton_2_clicked(bool checked)
{
    if(checked) {
        ui->radioButton->setChecked(false);
        ui->radioButton_3->setChecked(false);
    }
}

void CreateForm::on_radioButton_3_clicked(bool checked)
{
    if(checked) {
        ui->radioButton->setChecked(false);
        ui->radioButton_2->setChecked(false);
    }
}
