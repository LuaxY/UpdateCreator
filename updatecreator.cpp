#include "updatecreator.h"
#include "ui_updatecreator.h"

#include <QFileDialog>
#include <QDirIterator>
#include <QFileSystemModel>
#include <QDebug>

#include "aws/s3/S3Client.h"
#include "aws/s3/model/GetObjectRequest.h"

UpdateCreator::UpdateCreator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UpdateCreator)
{
    ui->setupUi(this);

    connect(ui->btnBrowseUpdatePath, SIGNAL(clicked()), this, SLOT(onClickBrowseUpdatePath()));

    Aws::S3::S3Client s3Client;

    Aws::S3::Model::GetObjectRequest getObjectRequest;
    getObjectRequest.SetBucket("araklys");
    getObjectRequest.SetKey("1/files/test");
    /*getObjectRequest.SetResponseStreamFactory(
        [](){
            return Aws::New(ALLOCATION_TAG, DOWNLOADED_FILENAME, std::ios_base::out | std::ios_base::in | std::ios_base::trunc);
        });*/
    auto getObjectOutcome = s3Client.GetObject(getObjectRequest);
    if(getObjectOutcome.IsSuccess())
    {
        qDebug() << "File downloaded from S3 to location " << getObjectOutcome.GetResult().GetBody();
    }
    else
    {
        qDebug() << "File download failed from s3 with error " << getObjectOutcome.GetError().GetMessage().c_str();
    }
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
