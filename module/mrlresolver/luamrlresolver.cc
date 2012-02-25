// luamrlresolver.cc
// 2/2/2012

#include "luamrlresolver.h"
#include "mrlresolversettings.h"
#include "module/luaresolver/luaresolver.h"
#include <QtCore>
#include <QtNetwork>

//#define DEBUG "luaemrlresolver"
#include "module/debug/debug.h"

// TODO: move to project source project instead of hard code here
#ifdef Q_OS_WIN
  #define LUA_PATH QCoreApplication::applicationDirPath() + "/lua/luascript"
#elif defined Q_OS_MAC
  #define LUA_PATH QCoreApplication::applicationDirPath() + "/lua"
#elif defined Q_OS_LINUX
  #define LUA_PATH "/usr/share/annot/player/lua"
#endif // Q_OS_

#ifdef LUA_PATH
  #define LUAPACKAGE_PATH LUA_PATH "/?.lua"
  #define LUASCRIPT_PATH  LUA_PATH "/luascript.lua"
#else
  #define LUAPACKAGE_PATH ""
  #define LUASCRIPT_PATH  "luascript.lua"
#endif // LUA_PATH

// - Tasks -

namespace { namespace task_ {

  class ResolveMedia : public QRunnable
  {
    LuaMrlResolver *r_;
    QString ref_;
    virtual void run() { r_->resolveMedia(ref_, false); } // \override, async = false
  public:
    ResolveMedia(const QString &ref, LuaMrlResolver *r)
      : r_(r), ref_(ref) { Q_ASSERT(r_); }
  };

  class ResolveSubtitle : public QRunnable
  {
    LuaMrlResolver *r_;
    QString ref_;
    virtual void run() { r_->resolveSubtitle(ref_, false); } // \override, async = false
  public:
    ResolveSubtitle(const QString &ref, LuaMrlResolver *r)
      : r_(r), ref_(ref) { Q_ASSERT(r_); }
  };

} } // anonymous namespace task_

// - Analysis -

bool
LuaMrlResolver::matchMedia(const QString &href) const
{
  DOUT("enter");
  bool ret = href.startsWith("http://", Qt::CaseInsensitive) &&
  (
    href.contains("bilibili.tv/", Qt::CaseInsensitive) ||
    href.contains("acfun.tv/", Qt::CaseInsensitive) ||
    href.contains("nicovideo.jp/") ||
    href.contains("youku.com/", Qt::CaseInsensitive) ||
    href.contains("video.sina.com.cn/", Qt::CaseInsensitive) ||
    href.contains("tudou.com/", Qt::CaseInsensitive) ||
    href.contains("6.cn/", Qt::CaseInsensitive) ||
    href.contains("mikufans.cn/", Qt::CaseInsensitive)
  );
  DOUT("exit: ret =" << ret);
  return ret;
}

bool
LuaMrlResolver::matchSubtitle(const QString &href) const
{
  DOUT("enter");
  bool ret = href.startsWith("http://", Qt::CaseInsensitive) &&
  (
    href.contains("bilibili.tv/", Qt::CaseInsensitive) ||
    href.contains("acfun.tv/", Qt::CaseInsensitive) ||
    href.contains("nicovideo.jp/")
  );
  DOUT("exit: ret =" << ret);
  return ret;
}

bool
LuaMrlResolver::resolveMedia(const QString &href, bool async)
{
  if (async) {
    if (!checkSiteAccount(href))
      return false;
    QThreadPool::globalInstance()->start(new task_::ResolveMedia(href, this));
    return true;
  }
  if (!checkSiteAccount(href))
    return false;
  DOUT("enter: href =" << href);
  QString url = cleanUrl(href);
  DOUT("url =" << url);

  MrlResolverSettings *settings = MrlResolverSettings::globalInstance();

  //LuaResolver *lua = makeResolver();
  QNetworkCookieJar *jar = new QNetworkCookieJar;
  LuaResolver lua(LUASCRIPT_PATH, LUAPACKAGE_PATH);
  lua.setCookieJar(jar);
  if (settings->hasNicovideoAccount())
    lua.setNicovideoAccount(settings->nicovideoAccount().username,
                            settings->nicovideoAccount().password);
  if (settings->hasBilibiliAccount())
    lua.setBilibiliAccount(settings->bilibiliAccount().username,
                           settings->bilibiliAccount().password);

  int siteid;
  QString refurl, title, suburl;
  QStringList mrls;
  QList<qint64> durations, sizes;
  bool ok = lua.resolve(url, &siteid, &refurl, &title, &suburl, &mrls, &durations, &sizes);
  if (!ok) {
    emit errorReceived(tr("failed to resolve URL") + ": " + url);
    DOUT("exit: LuaResolver returned false");
    return false;
  }

  if (mrls.isEmpty()) {
    if (href.contains("nicovideo.jp", Qt::CaseInsensitive))
      emit errorReceived(tr("failed to resolve URL using nicovideo account") + ": " + url);
    else if (href.contains("bilibili.tv", Qt::CaseInsensitive))
      emit errorReceived(tr("failed to resolve URL using bilibili account") + ": " + url);
    else
      emit errorReceived(tr("failed to resolve URL") + ": " + url);
    DOUT("exit: mrls is empty");
    return false;
  }

  MediaInfo mi;
  const char *encoding = 0;
  mi.suburl = formatUrl(suburl);
  mi.refurl = formatUrl(refurl);
  if (mi.refurl.contains("acfun.tv", Qt::CaseInsensitive))
    encoding = "GBK";
  //else if (mi.refurl.contains("nicovideo.jp", Qt::CaseInsensitive))
  //  encoding = "SHIFT-JIS";
  mi.title = formatTitle(title, encoding);

  switch (mrls.size()) { // Specialization for better performance
  case 0: break;
  case 1:
    {
      MrlInfo m(formatUrl(mrls.first()));
      if (!durations.isEmpty())
        m.duration = durations.front();
      if (!sizes.isEmpty())
        m.size = sizes.first();
      mi.mrls.append(m);
    } break;

  default:
    for (int i = 0; i < mrls.size(); i++) {
      MrlInfo m(formatUrl(mrls[i]));
      if (durations.size() > i)
        m.duration = durations[i];
      if (sizes.size() > i)
        m.size = sizes[i];
      mi.mrls.append(m);
    }
  }
  jar->setParent(0);
  emit mediaResolved(mi, jar);
  DOUT("exit: title =" << mi.title);
  return true;
}

bool
LuaMrlResolver::resolveSubtitle(const QString &href, bool async)
{
  if (async) {
    if (!checkSiteAccount(href))
      return false;
    QThreadPool::globalInstance()->start(new task_::ResolveSubtitle(href, this));
    return true;
  }
  if (!checkSiteAccount(href))
    return false;
  DOUT("enter: href =" << href);
  QString url = cleanUrl(href);
  DOUT("url =" << url);

  MrlResolverSettings *settings = MrlResolverSettings::globalInstance();

  //LuaResolver *lua = makeResolver();
  LuaResolver lua(LUASCRIPT_PATH, LUAPACKAGE_PATH);
  lua.setCookieJar(new QNetworkCookieJar); // potential memory leak on error
  if (settings->hasNicovideoAccount())
    lua.setNicovideoAccount(settings->nicovideoAccount().username,
                            settings->nicovideoAccount().password);
  if (settings->hasBilibiliAccount())
    lua.setBilibiliAccount(settings->bilibiliAccount().username,
                           settings->bilibiliAccount().password);

  // LuaResolver::resolve(const QString &href, QString *refurl, QString *title, QString *suburl, QStringList *mrls) const
  int siteid;
  QString suburl;
  bool ok = lua.resolve(url, &siteid, 0, 0, &suburl);

  if (!ok) {
    emit errorReceived(tr("failed to resolve URL") + ": " + url);
    DOUT("exit: LuaResolver returned false");
    return false;
  }

  if (suburl.isEmpty()) {
    if (href.contains("nicovideo.jp", Qt::CaseInsensitive))
      emit errorReceived(tr("failed to resolve URL using nicovideo account") + ": " + url);
    else if (href.contains("bilibili.tv", Qt::CaseInsensitive))
      emit errorReceived(tr("failed to resolve URL using bilibili account") + ": " + url);
    else
      emit errorReceived(tr("failed to resolve URL") + ": " + url);
    DOUT("exit: suburl is empty");
    return false;
  }

  suburl = formatUrl(suburl);
  if (!suburl.isEmpty())
    emit subtitleResolved(suburl);
  DOUT("exit: suburl =" << suburl);
  return true;
}

// - Helper -

//LuaResolver*
//LuaMrlResolver::makeResolver(QObject *parent)
//{
//  LuaResolver *ret = new LuaResolver(LUASCRIPT_PATH, LUAPACKAGE_PATH, 0, parent);
//  if (hasNicovideoAccount())
//    ret->setNicovideoAccount(nicovideoUsername_, nicovideoPassword_);
//  if (hasBilibiliAccount())
//    ret->setBilibiliAccount(bilibiliUsername_, bilibiliPassword_);
//  return ret;
//}

QString
LuaMrlResolver::decodeText(const QString &text, const char *encoding)
{
  if (text.isEmpty())
    return QString();
  if (encoding) {
    QTextCodec *tc = QTextCodec::codecForName(encoding);
    if (tc) {
      QTextDecoder *dc = tc->makeDecoder();
      if (dc)
        return dc->toUnicode(text.toLocal8Bit());
    }
  }
  return text;
}

QString
LuaMrlResolver::formatTitle(const QString &title, const char *encoding)
{
  QString ret = encoding ? decodeText(title, encoding) : title;
  if (ret.isEmpty())
    return ret;

  ret.remove(QRegExp(" ‐ ニコニコ動画\\(原宿\\)$"));
  ret.remove(QRegExp(" - 嗶哩嗶哩 - .*"));
  ret.remove(QRegExp(" - 优酷视频 - .*"));
  ret.remove(QRegExp(" - AcFun.tv$"));
  ret.replace("&lt;", "<");
  ret.replace("&gt;", ">");
  ret.replace("&quot;", "'");
  ret = ret.trimmed();
  return ret;
}

QString
LuaMrlResolver::formatUrl(const QString &href)
{
  QString ret = href.trimmed();
  if (ret.isEmpty())
    return ret;

  //ret = ret.replace("http://www.", "http://", Qt::CaseInsensitive);
  //ret = ret.remove(QRegExp("/$"));
  return ret;
}

QString
LuaMrlResolver::cleanUrl(const QString &url)
{
  QString ret = url;
  if (ret.contains("nicovideo.jp/watch/"))
    ret.remove(QRegExp("\\?.*"));
  return ret;
}

bool
LuaMrlResolver::checkSiteAccount(const QString &href)
{
  DOUT("enter");
  if (href.contains("nicovideo.jp", Qt::CaseInsensitive) &&
      !MrlResolverSettings::globalInstance()->hasNicovideoAccount()) {
    emit errorReceived(tr("nicovideo.jp account is required to resolve URL") + ": " + href);
    DOUT("exit: ret = false, nico account required");
    return false;
  }
  DOUT("exit: ret = true");
  return true;
}

// EOF