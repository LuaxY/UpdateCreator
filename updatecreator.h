#ifndef UPDATECREATOR_H
#define UPDATECREATOR_H

#include <QWidget>
#include <QSettings>

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

private slots:
    void onClickBrowseUpdatePathButton();
    void onClickApplyConfigurationButton();
};

#endif // UPDATECREATOR_H
