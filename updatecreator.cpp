#include "updatecreator.h"
#include "ui_updatecreator.h"

UpdateCreator::UpdateCreator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UpdateCreator)
{
    ui->setupUi(this);
}

UpdateCreator::~UpdateCreator()
{
    delete ui;
}
