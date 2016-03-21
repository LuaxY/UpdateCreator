#include "updatecreator.h"
#include "ui_updatecreator.h"

#include <QFileDialog>
#include <QDirIterator>
#include <QMessageBox>
#include <QCryptographicHash>
#include <QJsonObject>
#include <QJsonDocument>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QHttpMultiPart>
#include <QHttpPart>

#include <QDebug>

UpdateCreator::UpdateCreator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::UpdateCreator),
    fileCount(0),
    version(0)
{
    ui->setupUi(this);

    networkManager = new QNetworkAccessManager(this);

    connect(ui->BrowseUpdatePathButton, SIGNAL(clicked()), this, SLOT(onClickBrowseUpdatePathButton()));
    connect(ui->DeployUpdateButton, SIGNAL(clicked()), this, SLOT(onClickDeployUpdateButton()));
    connect(ui->ApplyConfigurationButton, SIGNAL(clicked()), this, SLOT(onClickApplyConfigurationButton()));

    settings = new QSettings("config.ini", QSettings::IniFormat);

    ui->CDNLinkLine->setText(settings->value("updates/cdn").toString());
    ui->ChannelComboBox->setCurrentText(settings->value("updates/channel").toString());
    ui->WindowsPrefixLine->setText(settings->value("prefix/win").toString());
    ui->MacPrefixLine->setText(settings->value("prefix/mac").toString());

    version = getCurrentVersion();
}

UpdateCreator::~UpdateCreator()
{
    delete ui;
    delete networkManager;
    delete model;
}

void UpdateCreator::onClickBrowseUpdatePathButton()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Open Update Directory"), ui->UpdateDirectoryPathLine->text(), QFileDialog::ShowDirsOnly| QFileDialog::DontResolveSymlinks);
    ui->UpdateDirectoryPathLine->setText(dir);

    QDirIterator it(dir, QStringList() << "*.*", QDir::Files, QDirIterator::Subdirectories);
    model = new QFileSystemModel;

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
    QString dirPath = ui->UpdateDirectoryPathLine->text();

    if (dirPath == "")
    {
        QMessageBox::warning(this, "Invalid update directory", "Please select valid update directory");
        return;
    }

    QDir dirCommon(dirPath + "/common");
    QDir dirWin(dirPath + "/win");
    QDir dirMac(dirPath + "/mac");

    if (!dirCommon.exists() || !dirWin.exists() || !dirMac.exists())
    {
        QMessageBox::warning(this, "Invalid update directory", "Please create 'common', 'win' and 'mac' subfolder.");
        return;
    }

    ui->DeployUpdateButton->setEnabled(false);

    QJsonObject updateJson;
    QJsonObject infoJson;
    QJsonArray files;
    int i = 0;

    version = getCurrentVersion();
    version++;

    updateJson["version"] = version;
    infoJson["version"]   = version;

    updateJson["common"] = generateFileTree(dirCommon, i);
    updateJson["win"] = generateFileTree(dirWin, i);
    updateJson["mac"] = generateFileTree(dirMac, i);

    QJsonObject prefixObject;
    prefixObject["win"] = ui->WindowsPrefixLine->text();
    prefixObject["mac"] = ui->MacPrefixLine->text();
    updateJson["prefix"] = prefixObject;

    // Generate update.json
    QFile updateFile("update.json");
    QJsonDocument updateDoc(updateJson);

    updateFile.open(QIODevice::WriteOnly);
    updateFile.write(updateDoc.toJson());
    updateFile.close();

    i++;
    ui->ProcessProgressBar->setValue(i * 100 / fileCount);

    // Generate info.json
    QFile infoFile("info.json");
    QJsonDocument infoDoc(infoJson);

    infoFile.open(QIODevice::WriteOnly);
    infoFile.write(infoDoc.toJson());
    infoFile.close();

    i++;
    ui->ProcessProgressBar->setValue(i * 100 / fileCount);

    ui->DeployUpdateButton->setEnabled(true);
}

void UpdateCreator::onClickApplyConfigurationButton()
{
    version = getCurrentVersion();

    settings->setValue("updates/cdn",     ui->CDNLinkLine->text());
    settings->setValue("updates/channel", ui->ChannelComboBox->currentText());
    settings->setValue("prefix/win",      ui->WindowsPrefixLine->text());
    settings->setValue("prefix/mac",      ui->MacPrefixLine->text());
    settings->sync();

    QMessageBox::information(this, "Configuration saved", "Configuration was saved in config.ini file");
}

int UpdateCreator::getCurrentVersion()
{
    QNetworkRequest request;

    request.setUrl(ui->CDNLinkLine->text() + "/info.json");
    QNetworkReply* reply = networkManager->get(request);

    QEventLoop loop;
    connect(reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();

    QJsonDocument jsonDoc = QJsonDocument::fromJson(reply->readAll());

    if (jsonDoc.isNull())
    {
        QMessageBox::warning(this, "Invalid CDN url", "CDN url is invalid or not set, please check configure tab");
        return 0;
    }

    QJsonObject json = jsonDoc.object();

    reply->deleteLater();
    loop.deleteLater();

    return json["version"].toInt();
}

QJsonArray UpdateCreator::generateFileTree(QDir dir, int &i)
{
    QJsonArray tree;
    QString dirPath = dir.absolutePath();
    QDirIterator it(dirPath, QStringList() << "*", QDir::Files, QDirIterator::Subdirectories);

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

        fileName.remove(dirPath + "/");

        fileObject["name"] = fileName;
        fileObject["md5"]  = md5;

        tree.append(fileObject);

        qDebug() << fileName;

        i++;
        ui->ProcessProgressBar->setValue(i * 100 / fileCount);
    }

    return tree;
}
