#ifndef UPDATECREATOR_H
#define UPDATECREATOR_H

#include <QWidget>
#include <QSettings>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QFileSystemModel>
#include <QDir>
#include <QJsonArray>

namespace Ui {
class UpdateCreator;
}

class UpdateCreator : public QWidget
{
    Q_OBJECT

public:
    explicit UpdateCreator(QWidget *parent = 0);
    ~UpdateCreator();

private:
    Ui::UpdateCreator* ui;
    QSettings* settings;
    int fileCount;
    int version;
    QNetworkAccessManager* networkManager;
    QFileSystemModel* model;

    int getCurrentVersion();
    QJsonArray generateFileTree(QDir dir, int& i);
    QByteArray buildPostRequest(QString name, QByteArray fileData);

private slots:
    void onClickBrowseUpdatePathButton();
    void onClickDeployUpdateButton();
    void onClickApplyConfigurationButton();
};

#endif // UPDATECREATOR_H
