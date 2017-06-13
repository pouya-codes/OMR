#ifndef OMRPROCESS_H
#define OMRPROCESS_H

#include <QDialog>
#include "answersheet.h"

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

    void on_pushButton_2_clicked();


private:
    Ui::OMRProcess *ui;
    AnswerSheet* answerSheet ;

};

#endif // OMRPROCESS_H
