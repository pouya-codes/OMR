#ifndef CREATEFORM_H
#define CREATEFORM_H

#include <QDialog>

namespace Ui {
class CreateForm;
}

class CreateForm : public QDialog
{
    Q_OBJECT

public:
    explicit CreateForm(QWidget *parent = 0);
    ~CreateForm();

private slots:
    void on_pb_openImage_clicked();

    void on_pb_removeColors_clicked();

    void on_pb_findEyes_clicked();

    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_radioButton_clicked(bool checked);

    void on_radioButton_2_clicked(bool checked);

    void on_radioButton_3_clicked(bool checked);

    void on_radioButton_row_clicked();

    void on_radioButton_column_clicked();

    void on_pushButton_save_clicked();

    void on_pushButton_clearOmitedColors_clicked();

    void on_pushButton_load_clicked();

private:
    Ui::CreateForm *ui;
    bool row_question_order = true;
    void setparameters() ;

    int referenceEye ;
    int distanceWidth ;
    int distanceHeight ;
    int choiceWidth ;
    int choiceHeigh ;
    int choiceNumber ;
    int distanceChoiceChoice ;
    int numberOfQuestions ;
    int columnDistance ;
    int rowDistance ;

    int barcodeX ;
    int barcodeY ;
    int barcodeWidth ;
    int barcodeHeight ;


    bool has_code ;
    int code_refrenceEye ;
    int code_numCode ;
    int code_distanceWidth ;
    int code_distanceHeight ;
    int code_distanceChoice ;

    QString referenceEye_key         = "referenceEye_key"  ;
    QString distanceWidth_key        = "distanceWidth_key" ;
    QString distanceHeight_key       = "distanceHeight_key" ;
    QString choiceWidth_key          = "choiceWidth_key" ;
    QString choiceHeigh_key          = "choiceHeigh_key" ;
    QString choiceNumber_key         = "choiceNumber_key" ;
    QString distanceChoiceChoice_key = "distanceChoiceChoice_key" ;
    QString numberOfQuestions_key    = "numberOfQuestions_key" ;
    QString columnDistance_key       = "columnDistance_key" ;
    QString rowDistance_key          = "rowDistance_key" ;

    QString barcodeX_key             = "barcodeX_key" ;
    QString barcodeY_key             = "barcodeY_key" ;
    QString barcodeWidth_key         = "barcodeWidth_key" ;
    QString barcodeHeight_key        = "barcodeHeight_key" ;

    QString  has_code_key            = "has_code_key" ;
    QString code_refrenceEye_key     = "code_refrenceEye_key" ;
    QString code_numCode_key         = "code_numCode_key" ;
    QString code_distanceWidth_key   = "code_distanceWidth_key" ;
    QString code_distanceHeight_key  = "code_distanceHeight_key" ;
    QString code_distanceChoice_key  = "code_distanceChoice_key" ;

    QString row_question_order_key   = "row_question_order_key" ;

};

#endif // CREATEFORM_H
