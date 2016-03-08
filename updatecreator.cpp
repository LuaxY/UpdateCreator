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
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

#include <QDebug>

#define BOUND "margin"
#define VERSION 0

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

    while (it.hasNext())
    {
        it.next();
        fileCount++;
    }

    fileCount += 2; // +2 for update.json and info.json
}

void UpdateCreator::onClickDeployUpdateButton()
{
    QString dir = ui->UpdateDirectoryPathLine->text();
    QDirIterator it(dir, QStringList() << "*.*", QDir::Files, QDirIterator::Subdirectories);

    QJsonObject json;
    QJsonArray files;

    json["version"] = VERSION;

    int i = 0;

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

        QString fileName = path;
        QString md5 = hash.result().toHex().data();
        QJsonObject fileObject;

        fileName.remove(dir + "/");

        fileObject["name"] = fileName;
        fileObject["md5"] = md5;

        files.append(fileObject);

        uploadFileToCDN(buildPostRequest(fileName, data));

        i++;
        ui->ProcessProgressBar->setValue(i * 100 / fileCount);
    }

    json["files"] = files;

    QJsonDocument saveDoc(json);
    uploadFileToCDN(buildPostRequest("update.json", saveDoc.toJson()));

    i++;
    ui->ProcessProgressBar->setValue(i * 100 / fileCount);

    // TODO: update info.json

    i++;
    ui->ProcessProgressBar->setValue(i * 100 / fileCount);
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

void UpdateCreator::uploadFileToCDN(QByteArray data)
{
    QUrl url = QUrl(ui->UploadLinkLine->text());
    QNetworkAccessManager* networkManager = new QNetworkAccessManager(this);

    QString bound = BOUND;

    QNetworkRequest request(url);
    request.setRawHeader(QByteArray("Content-Type"), QByteArray("multipart/form-data; boundary=" + bound.toLocal8Bit()));
    request.setRawHeader(QByteArray("Content-Length"), QString::number(data.length()).toLocal8Bit());

    QNetworkReply* reply = networkManager->post(request, data);

    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
}

QByteArray UpdateCreator::buildPostRequest(QString name, QByteArray fileData)
{
    QString bound = BOUND;
    QByteArray data;

    // Version
    data.append("--" + bound + "\r\n");
    data.append("Content-Disposition: form-data; name=\"version\"\r\n\r\n");
    data.append(QString::number(VERSION) + "\r\n");

    // AWS Public Key
    data.append("--" + bound + "\r\n");
    data.append("Content-Disposition: form-data; name=\"public\"\r\n\r\n");
    data.append(ui->AWSPublicKeyLine->text());
    data.append("\r\n");

    // AWS Private Key
    data.append("--" + bound + "\r\n");
    data.append("Content-Disposition: form-data; name=\"private\"\r\n\r\n");
    data.append(ui->AWSPrivateKeyLine->text());
    data.append("\r\n");

    // File name
    data.append("--" + bound + "\r\n");
    data.append("Content-Disposition: form-data; name=\"path\"\r\n\r\n");
    data.append(name);
    data.append("\r\n");

    // File
    data.append("--" + bound + "\r\n");
    data.append("Content-Disposition: form-data; name=\"file\"; filename=\"");
    data.append(name);
    data.append("\"\r\n");
    data.append("Content-Type: application/octet-stream\r\n\r\n");
    data.append(fileData);
    data.append("\r\n");
    data.append("--" + bound + "--\r\n");

    return data;
}
