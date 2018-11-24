#ifndef OMRPROCESS_H
#define OMRPROCESS_H

#include <QDialog>
#include "answersheet.h"
#include <QSqlTableModel>
#include "ui_omrprocess.h"
#include "asmOpenCV.h"
#include <QtSql/qsqldatabase.h>
#include <QItemSelectionModel>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <QFileDialog>
#include <fstream>
#include <QMessageBox>
#include <QSortFilterProxyModel>
#include <thread>
#include <future>
#include <QMouseEvent>
#include <QSettings>
#include <QString>
#include <QFileInfo>
#include <QDesktopServices>



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
    bool columnQustaion ;

private slots:
    void on_pushButton_clicked();
    void ProcessImagePath(std::string path,std::string pathOrginal,std::string pathProcessed,std::string pathError) ;
    cv::Mat ProcessImage(cv::String imagePath,std::string pathOrginal,std::string pathProcessed,std::string pathError,int thread_no) ;
    void on_tableViewSelectionModel_currentRowChanged(QModelIndex index1,QModelIndex index2);

    void on_pushButton_2_clicked();
    void getTableNames() ;
    void queryData();
    void handleAfterEdit( QModelIndex index ,QModelIndex index2 ,QVector<int> vector) ;
    void lableClicked(QMouseEvent* event);



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

    void on_checkBoxTwoChoices_clicked();

    void on_checkBoxApsent_clicked();

    void on_checkBoxLowColors_clicked();

    void on_lineEditTableName_textChanged(const QString &arg1);

    void on_LineEditMain_textChanged(const QString &arg1);

    void on_pushButtonStop_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_MainPathOpen_clicked();

    void on_pushButtonSingleFile_clicked();

    void on_pushButton_ErrorPathOpen_clicked();

    void on_pushButton_MainPathBrowse_clicked();

    void on_pushButton_OrginalPathOpen_clicked();

    void on_pushButton_ProcessedPathOpen_clicked();

    void on_pushButton_OrginalPathBrowse_clicked();

    void on_pushButton_ProcessedPathBrowse_clicked();

    void on_pushButton_ErrorPathBrowse_clicked();

private:
    QSettings mySetting ;
    const QString DEFAULT_RESULT_PATH_KEY = "defult_result_dir";
    const QString DEFAULT_ORGINAL_PATH_KEY = "defult_orginal_dir";
    const QString DEFAULT_PROCESSED_PATH_KEY = "defult_processed_dir";
    const QString DEFAULT_ERROR_PATH_KEY = "defult_error_dir";
    const QString DEFAULT_DBNAME_KEY = "defult_db_name";

    const QString DEFAULT_OPEN_PATH_KEY = "defult_open_dir";

    Ui::OMRProcess *ui;
    AnswerSheet* answerSheet ;
    bool running ;
    QSqlTableModel *dataModel  ;
    cv::Mat labelMat ;
    int selectedRowId  =-1;
    int editSelectedRowId =-2;
    QString selectedRowAnswers ;
    QString selectedRowOrginalPath ;
    QString selectedRowProcessedPath ;
    std::vector<std::vector<cv::Rect>> selectedRowAreas;
    cv::Mat selectedRowImage;

};

#endif // OMRPROCESS_H
