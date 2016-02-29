#include "updatecreator.h"
#include "ui_updatecreator.h"

#include <QFileDialog>
#include <QDirIterator>
#include <QFileSystemModel>
#include <QDebug>

UpdateCreator::UpdateCreator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UpdateCreator)
{
    ui->setupUi(this);

    connect(ui->btnBrowseUpdatePath, SIGNAL(clicked()), this, SLOT(onClickBrowseUpdatePath()));
}

UpdateCreator::~UpdateCreator()
{
    delete ui;
}

void UpdateCreator::onClickBrowseUpdatePath()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Update Directory"), ui->lineUpdateDirectoryPath->text(), QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
    ui->lineUpdateDirectoryPath->setText(dir);

    QDirIterator it(dir, QStringList() << "*.*", QDir::Files, QDirIterator::Subdirectories);
    QFileSystemModel* model = new QFileSystemModel;

    model->setRootPath(dir);
    ui->treeViewListFiles->setModel(model);
    ui->treeViewListFiles->setRootIndex(model->index(dir));

    /*while (it.hasNext())
    {
        ui->treeViewListFiles->ins
        qDebug() << it.next();
    }*/

}
