#ifndef DEVICEDIALOG_H
#define DEVICEDIALOG_H

// devicedialog.h
// 12/2/2011

#include "project/common/acwindow.h"
#include <QtCore/QStringList>

QT_BEGIN_NAMESPACE
class QComboBox;
class QRadioButton;
class QToolButton;
QT_END_NAMESPACE

class DeviceDialog : public AcWindow
{
  Q_OBJECT
  Q_DISABLE_COPY(DeviceDialog)
  typedef DeviceDialog Self;
  typedef AcWindow Base;

public:
  explicit DeviceDialog(QWidget *parent = 0);

signals:
  void deviceSelected(const QString &path, bool isAudioCD);

  // - Helpers -
public:
  static QStringList devices(); // TODO: move to somewhere else

  // - Properties -
public:
  QString currentPath() const;
  //void setMessage(const QString &text);

  // - Events -
public:
  virtual void setVisible(bool visible); ///< \override

  // - Slots -
public slots:
  void refresh();
protected slots:
  void ok();

  void updateButtons();
  void updateComboBox();

private:
  void createLayout();

private:
  QToolButton *okButton_;
  QComboBox *pathComboBox_; // device path
  QRadioButton *autoRadioButton_, // device type
               *dvdRadioButton_,
               *cdRadioButton_;
};

#endif // DEVICEDIALOG_H
