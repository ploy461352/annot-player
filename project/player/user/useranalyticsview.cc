// useranalyticsview.cc
// 5/7/2012

#include "useranalyticsview.h"
#include "global.h"
#include "tr.h"
#include "datamanager.h"
#include "logger.h"
#include "module/annotcloud/annothtml.h"
#include <QtGui>
#include <QtWebKit>

using namespace Logger;

#define DEBUG "annotationanalyticsview"
#include "module/debug/debug.h"

#define WINDOW_SIZE     QSize(640, 640)
#define BASE_URL        G_HOMEPAGE "/api/player/"
#define EMPTY_URL       BASE_URL "empty.html"

using namespace AnnotCloud;

// - Construction -

#define WINDOW_FLAGS \
  Qt::Dialog | \
  Qt::CustomizeWindowHint | \
  Qt::WindowTitleHint | \
  Qt::WindowSystemMenuHint | \
  Qt::WindowMinMaxButtonsHint | \
  Qt::WindowCloseButtonHint

UserAnalyticsView::UserAnalyticsView(DataManager *data, QWidget *parent)
  : Base(parent, WINDOW_FLAGS), data_(data), userId_(0)
{
  Q_ASSERT(data_);
  setWindowTitle(tr("User Analytics"));
  resize(WINDOW_SIZE);

  webView()->setRenderHints(
    QPainter::Antialiasing |
    QPainter::TextAntialiasing |
    //QPainter::HighQualityAntialiasing |
    QPainter::SmoothPixmapTransform
  );

  setupActions();

  // Shortcuts
  new QShortcut(QKeySequence("Esc"), this, SLOT(fadeOut()));
}

void
UserAnalyticsView::setupActions()
{
  QAction *a = webView()->pageAction(QWebPage::Reload);
  if (a)
    connect(a, SIGNAL(triggered()), SLOT(refresh()));

  a = webView()->pageAction(QWebPage::ReloadAndBypassCache);
  if (a)
    connect(a, SIGNAL(triggered()), SLOT(refresh()));
}

// - Properties -

AnnotationList
UserAnalyticsView::userAnnotatinons() const
{
  AnnotationList ret;
  if (userId_)
    foreach (const Annotation &a, data_->annotations())
      if (a.userId() == userId_)
        ret.append(a);
  return ret;
}

// - Actions -

void
UserAnalyticsView::refresh()
{
  DOUT("enter: userId =" << userId_);
  invalidateAnnotations();
  DOUT("exit");
}

void
UserAnalyticsView::setUrl(const QUrl &url)
{ webView()->load(url); }

void
UserAnalyticsView::setContent(const QString &html)
{
  QString mimeType,
          baseUrl = BASE_URL;
  webView()->setContent(html.toUtf8(), mimeType, baseUrl);
}

void
UserAnalyticsView::invalidateAnnotations()
{
  QString title = tr("User");
  AnnotationList a = userAnnotatinons();
  if (a.isEmpty())
    setUrl(QUrl(EMPTY_URL));
  else {
    title.append(QString(" (%1)").arg(QString::number(userId_, 16)));
    QString html = AnnotationHtmlParser::globalInstance()->toHtml(a);
    setContent(html);
  }

  setWindowTitle(title);
}

void
UserAnalyticsView::setVisible(bool visible)
{
  if (visible) {
    refresh();
    if (isVisible())
      raise();
  }
  Base::setVisible(visible);
}

// EOF
