#include "updatecreator.h"
#include "ui_updatecreator.h"

#include <QFileDialog>
#include <QDirIterator>
#include <QFileSystemModel>
#include <QMessageBox>

#include <QDebug>

UpdateCreator::UpdateCreator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UpdateCreator)
{
    ui->setupUi(this);

    connect(ui->BrowseUpdatePathButton, SIGNAL(clicked()), this, SLOT(onClickBrowseUpdatePathButton()));
    connect(ui->ApplyConfigurationButton, SIGNAL(clicked()), this, SLOT(onClickApplyConfigurationButton()));

    settings = new QSettings("config.ini", QSettings::IniFormat);

    ui->CDNLinkLine->setText(settings->value("updates/cdn").toString());
    ui->UploadLinkLine->setText(settings->value("updates/upload").toString());
    ui->ChannelComboBox->setCurrentText(settings->value("updates/channel").toString());

    ui->AWSPublicKeyLine->setText(settings->value("aws/public").toString());
    ui->AWSPrivateKeyLine->setText(settings->value("aws/private").toString());
}

UpdateCreator::~UpdateCreator()
{
    delete ui;
}

void UpdateCreator::onClickBrowseUpdatePathButton()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Update Directory"), ui->UpdateDirectoryPathLine->text(), QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
    ui->UpdateDirectoryPathLine->setText(dir);

    QDirIterator it(dir, QStringList() << "*.*", QDir::Files, QDirIterator::Subdirectories);
    QFileSystemModel* model = new QFileSystemModel;

    model->setRootPath(dir);
    ui->ListFilesTreeView->setModel(model);
    ui->ListFilesTreeView->setRootIndex(model->index(dir));

    /*while (it.hasNext())
    {
        ui->ListFilesTreeView->ins
        qDebug() << it.next();
    }*/
}

void UpdateCreator::onClickApplyConfigurationButton()
{
    settings->setValue("updates/cdn",     ui->CDNLinkLine->text());
    settings->setValue("updates/upload",  ui->UploadLinkLine->text());
    settings->setValue("updates/channel", ui->ChannelComboBox->currentText());

    settings->setValue("aws/public",  ui->AWSPublicKeyLine->text());
    settings->setValue("aws/private", ui->AWSPrivateKeyLine->text());

    settings->sync();

    QMessageBox::information(this, "Configuration saved", "Configuration was saved in config.ini file");
}
