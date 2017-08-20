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

private:
    Ui::CreateForm *ui;
};

#endif // CREATEFORM_H
