#include "updatecreator.h"
#include "ui_updatecreator.h"

#include <QFileDialog>
#include <QDirIterator>
#include <QFileSystemModel>
#include <QMessageBox>
#include <QCryptographicHash>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

#include <QDebug>

UpdateCreator::UpdateCreator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UpdateCreator)
{
    ui->setupUi(this);

    connect(ui->BrowseUpdatePathButton, SIGNAL(clicked()), this, SLOT(onClickBrowseUpdatePathButton()));
    connect(ui->DeployUpdateButton, SIGNAL(clicked()), this, SLOT(onClickDeployUpdateButton()));
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


}

void UpdateCreator::onClickDeployUpdateButton()
{
    QString dir = ui->UpdateDirectoryPathLine->text();
    QDirIterator it(dir, QStringList() << "*.*", QDir::Files, QDirIterator::Subdirectories);

    QJsonObject json;
    QJsonArray files;

    json["version"] = 0;

    while (it.hasNext())
    {
        QString path = it.next();
        QFile file(path);

        if(!file.open(QIODevice::ReadOnly))
        {
            continue;
        }

        QCryptographicHash hash(QCryptographicHash::Md5);
        QByteArray data = file.readAll();
        hash.addData(data);
        file.close();

        QString filePath = path.remove(dir + "/");
        QString md5 = hash.result().toHex().data();

        QJsonObject fileObject;

        fileObject["name"] = filePath;²
        fileObject["md5"] = md5;

        files.append(fileObject);

        // TODO: upload file
    }

    json["files"] = files;

    QJsonDocument saveDoc(json);

    // TODO: upload updates.json saveDoc.toJson()

    // TODO: update info.json
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
