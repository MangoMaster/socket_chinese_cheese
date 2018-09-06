#ifndef MODE_DIALOG_H_
#define MODE_DIALOG_H_

#include <QtWidgets/QDialog>
#include <QtCore/QSignalMapper>
#include <QtGui/QRegExpValidator>

namespace Ui
{
class ModeDialog;
}

class ModeDialog : public QDialog
{
    Q_OBJECT

  public:
    explicit ModeDialog(QWidget *parent = nullptr);
    ~ModeDialog();
    friend class CheeseModel;

  private:
    Ui::ModeDialog *ui;
};

#endif // MODE_DIALOG_H_