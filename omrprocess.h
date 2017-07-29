#ifndef OMRPROCESS_H
#define OMRPROCESS_H

#include <QDialog>
#include "answersheet.h"
#include <QSqlTableModel>

namespace Ui {
class OMRProcess;
}

class OMRProcess : public QDialog
{
    Q_OBJECT

public:
    explicit OMRProcess(QWidget *parent = 0);
    ~OMRProcess();
    void setAnswerSheet(AnswerSheet* answerSheet_);

private slots:
    void on_pushButton_clicked();
    void ProcessImage(std::string path,std::string pathOrginal,std::string pathProcessed,std::string pathError) ;
    void on_pushButton_2_clicked();
    void getTableNames() ;
    void queryData();
    void handleAfterEdit( QModelIndex index ,QModelIndex index2 ,QVector<int> vector) ;


    void on_tableView_clicked(const QModelIndex &index);

    void on_tableView_activated(const QModelIndex &index);

    void on_tableView_entered(const QModelIndex &index);

    void setPicture (int id) ;

    void on_comboBoxDbNames_currentIndexChanged(const QString &arg1);


    void on_radioButtonProcessed_clicked(bool checked);

    void on_radioButtonOrginal_clicked(bool checked);

    void on_checkBoxDuplicated_clicked();

    void on_checkBoxInvalid_clicked();

    void on_pushButtonOmitTable_clicked();

    void on_lineEditSearch_textChanged(const QString &arg1);

    void on_pushButton_3_clicked();

private:
    Ui::OMRProcess *ui;
    AnswerSheet* answerSheet ;
    bool running ;
     QSqlTableModel *dataModel  ;

};

#endif // OMRPROCESS_H
