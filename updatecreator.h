#ifndef UPDATECREATOR_H
#define UPDATECREATOR_H

#include <QWidget>

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
    Ui::UpdateCreator *ui;
};

#endif // UPDATECREATOR_H
