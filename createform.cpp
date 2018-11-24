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

    QString DEFAULT_DIR_KEY("answerShit_dir");
    QSettings MySettings;

    QString fileName = QFileDialog::getOpenFileName(
                this, "Select a file", MySettings.value(DEFAULT_DIR_KEY).toString()=="" ? QDir::currentPath() : MySettings.value(DEFAULT_DIR_KEY).toString(),"Images (*.jpg);;All Files (*)");

    if(fileName!="") {
        QFileInfo SampleFile(fileName);

        MySettings.setValue(DEFAULT_DIR_KEY,SampleFile.absoluteDir().absolutePath());

        cv::Mat img = cv::imread(fileName.toStdString().c_str()) ;
        if(!img.empty())
        {
            answerSheet = new AnswerSheet(img) ;

            cv::resize(img, resized_img_cv,cv::Size(ui->label_image->width(),ui->label_image->height())) ;
            qt_img = ASM::cvMatToQImage( resized_img_cv );
            ui->label_image->setPixmap(QPixmap::fromImage(qt_img));
            ui->pb_removeColors->setEnabled(true);
            ui->pushButton_clearOmitedColors->setEnabled(true);
            ui->frameBarcode->setEnabled(true);
            ui->frameEyes->setEnabled(true);
            ui->frameQuestions->setEnabled(true);
            ui->frame_4->setEnabled(true);
            ui->frame_3->setEnabled(true);
            ui->pushButton_2->setEnabled(true);
            ui->le_barcode_height->setEnabled(true);
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
    answerSheet->DetectEyes(ui->le_EyePad->text().toInt(),ui->le_darkness->text().toInt(),ui->spinBox_min_squre->text().toInt(),ui->spinBox_max_squre->text().toInt()) ;
    cv::resize(answerSheet->getImage(), resized_img_cv,cv::Size(ui->label_image->width(),ui->label_image->height())) ;
    qt_img = ASM::cvMatToQImage(resized_img_cv);
    ui->label_image->setPixmap(QPixmap::fromImage(qt_img));
}

void CreateForm::setparameters() {
    referenceEye = ui->le_refrenceEye->text().toInt() ;
    distanceWidth = ui->le_distanceRefwidth->text().toInt() ;
    distanceHeight = ui->le_distanceRefHeight->text().toInt() ;
    choiceWidth = ui->le_choiceWidth->text().toInt() ;
    choiceHeigh = ui->le_choiceHeight->text().toInt() ;
    choiceNumber = ui->le_numberOfChoices->text().toInt() ;
    distanceChoiceChoice = ui->le_distanceChoiceChoice->text().toInt() ;
    numberOfQuestions = ui->le_numberOfquestions->text().toInt() ;
    columnDistance = ui->le_distanceColumns->text().toInt() ;
    rowDistance = ui->spinBox_rowDistance->text().toInt() ;

    barcodeX = ui->le_barcode_x->text().toInt() ;
    barcodeY = ui->le_barcode_y->text().toInt() ;
    barcodeWidth = ui->le_barcode_width->text().toInt() ;
    barcodeHeight = ui->le_barcode_height->text().toInt() ;


    has_code = ui->checkBox_code->isChecked() ;
    code_refrenceEye = ui->SpinBox_code_refrenceEye->text().toInt() ;
    code_numCode = ui->SpinBox_code_numChoice->text().toInt() ;
    code_distanceWidth = ui->SpinBox_code_width->text().toInt() ;
    code_distanceHeight = ui->SpinBox_code_height->text().toInt() ;
    code_distanceChoice = ui->SpinBox_code_distance->text().toInt() ;
}

void CreateForm::on_pushButton_clicked()
{
    setparameters() ;

    cv::Mat ans = answerSheet->DrawChoices(referenceEye,distanceWidth,distanceHeight,choiceWidth,choiceHeigh,
                                           choiceNumber,distanceChoiceChoice,numberOfQuestions,columnDistance,
                                           barcodeX,barcodeY,barcodeWidth,barcodeHeight,row_question_order,rowDistance,
                                           has_code,code_refrenceEye,code_numCode,code_distanceWidth,code_distanceHeight,code_distanceChoice);
    cv::resize(ans, resized_img_cv,cv::Size(ui->label_image->width(),ui->label_image->height())) ;
    qt_img = ASM::cvMatToQImage(resized_img_cv);
    ui->label_image->setPixmap(QPixmap::fromImage(qt_img));

}

void CreateForm::on_pushButton_2_clicked()
{
    OMRProcess omrprocess ;
    omrprocess.columnQustaion =ui->radioButton_column->isChecked() ;
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

void CreateForm::on_radioButton_row_clicked()
{
    ui->radioButton_column->setChecked(false);
    row_question_order =true ;

}

void CreateForm::on_radioButton_column_clicked()
{
    ui->radioButton_row->setChecked(false);
    row_question_order = false ;
}

void CreateForm::on_pushButton_save_clicked()
{
    QString outfile = QFileDialog::getSaveFileName(this, tr("Save Sheet"),
                                                   "/home","OMR Files (*.omr)");

    if(outfile!="") {
        setparameters() ;
        QSettings settings(outfile, QSettings::IniFormat);

        settings.setValue(referenceEye_key         , referenceEye) ;
        settings.setValue(distanceWidth_key        , distanceWidth ) ;
        settings.setValue(distanceHeight_key       , distanceHeight ) ;
        settings.setValue(choiceWidth_key          , choiceWidth ) ;
        settings.setValue(choiceHeigh_key          , choiceHeigh ) ;
        settings.setValue(choiceNumber_key         , choiceNumber ) ;
        settings.setValue(distanceChoiceChoice_key , distanceChoiceChoice ) ;
        settings.setValue(numberOfQuestions_key    , numberOfQuestions ) ;
        settings.setValue(columnDistance_key       , columnDistance ) ;
        settings.setValue(rowDistance_key          , rowDistance ) ;

        settings.setValue(barcodeX_key             , barcodeX ) ;
        settings.setValue(barcodeY_key             , barcodeY ) ;
        settings.setValue(barcodeWidth_key         , barcodeWidth ) ;
        settings.setValue(barcodeHeight_key        , barcodeHeight ) ;

        settings.setValue( has_code_key            ,  has_code ) ;
        settings.setValue(code_refrenceEye_key     , code_refrenceEye ) ;
        settings.setValue(code_numCode_key         , code_numCode ) ;
        settings.setValue(code_distanceWidth_key   , code_distanceWidth ) ;
        settings.setValue(code_distanceHeight_key  , code_distanceHeight ) ;
        settings.setValue(code_distanceChoice_key  , code_distanceChoice ) ;

        settings.setValue(row_question_order_key  , row_question_order ) ;


    }
}

void CreateForm::on_pushButton_clearOmitedColors_clicked()
{
    answerSheet->clearOmitedColors();
}

void CreateForm::on_pushButton_load_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(
                this, "Select a file", "/home","OMR Files (*.omr)");
    if(fileName!="") {

        QSettings settings(fileName, QSettings::IniFormat);

        ui->le_refrenceEye->setValue(settings.value(referenceEye_key).toInt());
        ui->le_distanceRefwidth->setValue(settings.value(distanceWidth_key).toInt()) ;
        ui->le_distanceRefHeight->setValue(settings.value(distanceHeight_key).toInt());
        ui->le_choiceWidth->setValue(settings.value(choiceWidth_key).toInt()) ;
        ui->le_choiceHeight->setValue(settings.value(choiceHeigh_key).toInt()) ;
        ui->le_numberOfChoices->setValue(settings.value(choiceNumber_key).toInt()) ;
        ui->le_distanceChoiceChoice->setValue(settings.value(distanceChoiceChoice_key).toInt());
        ui->le_numberOfquestions->setValue(settings.value(numberOfQuestions_key).toInt()) ;
        ui->le_distanceColumns->setValue(settings.value(columnDistance_key).toInt()) ;
        ui->spinBox_rowDistance->setValue(settings.value(rowDistance_key).toInt()) ;

        ui->le_barcode_x->setValue(settings.value(barcodeX_key).toInt());
        ui->le_barcode_y->setValue(settings.value(barcodeY_key).toInt());
        ui->le_barcode_width->setValue(settings.value(barcodeWidth_key).toInt());
        ui->le_barcode_height->setValue(settings.value(barcodeHeight_key).toInt());

        ui->checkBox_code->setChecked(settings.value(has_code_key).toBool()) ;
        ui->SpinBox_code_refrenceEye->setValue(settings.value(code_refrenceEye_key).toInt()) ;
        ui->SpinBox_code_numChoice->setValue(settings.value(code_numCode_key).toInt()) ;
        ui->SpinBox_code_width->setValue(settings.value(code_distanceWidth_key).toInt()) ;
        ui->SpinBox_code_height->setValue(settings.value(code_distanceHeight_key).toInt()) ;
        ui->SpinBox_code_distance->setValue(settings.value(code_distanceChoice_key).toInt()) ;

        row_question_order = settings.value(row_question_order_key).toBool() ;

        if(row_question_order)
        {
            ui->radioButton_column->setChecked(false);
            ui->radioButton_row->setChecked(true);
        }

        else {
            ui->radioButton_column->setChecked(true);
            ui->radioButton_row->setChecked(false);
        }


        setparameters() ;

    }
}
