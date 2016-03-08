#ifndef UPDATECREATOR_H
#define UPDATECREATOR_H

#include <QWidget>
#include <QSettings>
#include <QByteArray>

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
    int fileCount = 0;
    int version = 0;

    int getCurrentVersion();
    void uploadFileToCDN(QByteArray data);
    QByteArray buildPostRequest(QString name, QByteArray fileData);

private slots:
    void onClickBrowseUpdatePathButton();
    void onClickDeployUpdateButton();
    void onClickApplyConfigurationButton();
};

#endif // UPDATECREATOR_H
