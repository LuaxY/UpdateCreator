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

    void uploadFileToCDN(QString path, QString name);
    QByteArray buildPostRequest(QString path, QString name);

private slots:
    void onClickBrowseUpdatePathButton();
    void onClickDeployUpdateButton();
    void onClickApplyConfigurationButton();
};

#endif // UPDATECREATOR_H
