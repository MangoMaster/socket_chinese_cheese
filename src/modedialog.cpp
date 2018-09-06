#include "modedialog.h"
#include "ui_modedialog.h"

ModeDialog::ModeDialog(QWidget *parent)
    : QDialog(parent)
{
    ui->setupUi(this);
}

ModeDialog::~ModeDialog()
{
    delete ui;
}