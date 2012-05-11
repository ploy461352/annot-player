#ifndef MINIPLAYER_H
#define MINIPLAYER_H

// miniplayer.h
// 7/11/2011

#include "playerui.h"
#include <QtGui/QDockWidget>

class MiniPlayerUi : public PlayerUi
{
  Q_OBJECT
  Q_DISABLE_COPY(MiniPlayerUi)
  typedef MiniPlayerUi Self;
  typedef PlayerUi Base;

  QPoint dragPos_;

public:
  explicit MiniPlayerUi(SignalHub *hub, Player *player, ServerAgent *server, QWidget *parent = 0);

public:
  virtual void setTitle(const QString &title); ///< \override

public slots:
  void updateTitle();
  virtual void setVisible(bool visible); ///< \override

  // - Events -
protected:
  void forward(QEvent *event);

  virtual void mousePressEvent(QMouseEvent *event); ///< \override
  virtual void mouseMoveEvent(QMouseEvent *event); ///< \override
  virtual void mouseReleaseEvent(QMouseEvent *event); ///< \override

  virtual void mouseDoubleClickEvent(QMouseEvent *event); ///< \override

  virtual void contextMenuEvent(QContextMenuEvent *event); ///< \override

  virtual void keyPressEvent(QKeyEvent *event); ///< \override
  virtual void keyReleaseEvent(QKeyEvent *event); ///< \override

protected slots:
  virtual void dragEnterEvent(QDragEnterEvent *event); ///< \override
  virtual void dragMoveEvent(QDragMoveEvent *event); ///< \override
  virtual void dragLeaveEvent(QDragLeaveEvent *event); ///< \override
  virtual void dropEvent(QDropEvent *event); ///< \override

signals:
  void dragEnterEventReceived(QDragEnterEvent *event);
  void dragMoveEventReceived(QDragMoveEvent *event);
  void dragLeaveEventReceived(QDragLeaveEvent *event);
  void dropEventReceived(QDropEvent *event);

private:
  void createLayout();
};

#endif // MINIPLAYER_H

/*
class MiniPlayerDock : public QDockWidget
{
  Q_OBJECT
  Q_DISABLE_COPY(MiniPlayerDock)
  typedef MiniPlayerDock Self;
  typedef QDockWidget Base;

  QPoint dragPos_;

public:
  explicit MiniPlayerDock(QWidget *parent = 0);

public:
  virtual void setVisible(bool visible); // stop polling when hidden

signals:
  void toggleFullScreenModeRequested();
  void toggleMiniModeRequested();

protected:
  void forward(QEvent *event);

  virtual void mousePressEvent(QMouseEvent *event);
  virtual void mouseMoveEvent(QMouseEvent *event);
  virtual void mouseReleaseEvent(QMouseEvent *event);

  virtual void mouseDoubleClickEvent(QMouseEvent *event);
  virtual void closeEvent(QCloseEvent *event);
  virtual void contextMenuEvent(QContextMenuEvent *event);

  virtual void dragEnterEvent(QDragEnterEvent *event);
  virtual void dragMoveEvent(QDragMoveEvent *event);
  virtual void dragLeaveEvent(QDragLeaveEvent *event);
  virtual void dropEvent(QDropEvent *event);

  virtual void keyPressEvent(QKeyEvent *event);
  virtual void keyReleaseEvent(QKeyEvent *event);
};
*/

