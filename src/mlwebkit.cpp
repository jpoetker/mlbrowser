#include <QGraphicsSceneContextMenuEvent>
#include <QWebSettings>
#include <QDesktopWidget>
#include <QDebug>
#include <QApplication>
#include <QMetaObject>

#include "mlwebkit.h"

#ifdef _PLAYER_
#include "mlplayer.h"
#endif

#ifdef _SSLERROR_
#include "mlsslerror.h"
#endif

//TODO : move to command line argument 
#ifdef _DEBUG_TOOLS_
#define INSPECTOR_PORT 9999
#endif

#if defined (_PLAYER_) || defined (_PROPERTYCHANGER_) || defined (_DEBUG_TOOLS_)
#include <QWebFrame>
#endif

class GraphicsWebView : public QGraphicsWebView
{
public:
	GraphicsWebView()
	{
	}

protected:
	void contextMenuEvent(QGraphicsSceneContextMenuEvent* ev)
	{

		qDebug () << "received a context event";

		if ( ev != NULL )
			ev->ignore();	

/*
QContextMenuEvent::Mouse
QContextMenuEvent::Keyboard
QContextMenuEvent::Other
*/

	}
};

class WebPage : public QWebPage
{
public:

protected:
	void javaScriptConsoleMessage(const QString& message, int lineNumber, const QString& source)
	{
		QString logEntry = source + " [" + QString::number(lineNumber) + "]:" + message.toUtf8().constData();
		qDebug()<<logEntry;
	}

	void javaScriptAlert(QWebFrame*, const QString& message)
	{
		QString logEntry = message.toUtf8().constData();
		qDebug()<<logEntry;
	}

	bool shouldInterruptJavaScript()
	{
		return false;
	}

/*
	virtual QString userAgentForUrl(const QUrl &url ) const
	{
		Q_UNUSED(url);

		qDebug () << "user agent : " << "mlwebkit/1.0";
		return QString("mlwebkit/1.0");
	}
*/
};

MLWebKit& MLWebKit::instance()
{
	static MLWebKit mlwebkit;
	return mlwebkit;
} 

MLWebKit::MLWebKit() 
{
	view.setScene(&scene);

//TODO : use proper cast
	QApplication* pApp = (QApplication *)QApplication::instance();
	Q_ASSERT(pApp != NULL);

	QDesktopWidget* pDesktop = QApplication::desktop();
	Q_ASSERT(pDesktop != NULL);

#ifdef _SSLERROR_
	QObject::connect (page.networkAccessManager(), &QNetworkAccessManager::sslErrors, sslerror, &SSLError::handleSslErrors);
#endif

#ifdef QT_OPENGL_LIB
	pWidget = NULL;
//	pWidget = new QGLWidget();
//	pWidget = new QGLWidget(view);
#endif

	// Configuration, settings and alike
//	scene.setItemIndexMethod( QGraphicsScene::NoIndex);
//	view.setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);
	view.setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
//	view.setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
//	view.setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);

	// Disable some 'browser features / elements'
	view.setFrameShape(QFrame::NoFrame);
	view.setHorizontalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );
	view.setVerticalScrollBarPolicy ( Qt::ScrollBarAlwaysOff );

//	view.setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	view.setWindowFlags(Qt::FramelessWindowHint);
	view.showFullScreen();

	// Overrule the 'foreground' and 'background' defaults with transparent colors
	QPalette palette;
	palette.setBrush(QPalette::Active, QPalette::Window, Qt::SolidPattern);
	palette.setBrush(QPalette::Active, QPalette::Base, Qt::SolidPattern);
	palette.setBrush(QPalette::Inactive, QPalette::Window, Qt::SolidPattern);
	palette.setBrush(QPalette::Inactive, QPalette::Base, Qt::SolidPattern);
	palette.setColor(QPalette::Active, QPalette::Window, QColor(0, 0, 0, 0));
	palette.setColor(QPalette::Active, QPalette::Base, QColor(0, 0, 0, 0));
	palette.setColor(QPalette::Inactive, QPalette::Window, QColor(0, 0, 0, 0));
	palette.setColor(QPalette::Inactive, QPalette::Base, QColor(0, 0, 0, 0));

	pApp->setPalette(palette);

	qDebug () << "geometry : " << pDesktop->screenGeometry().size();

	// Proper (re)sizing, full screen

//TODO : implement check
	webview.resize(QApplication::desktop()->screenGeometry().size());
	webview.setPage(&page);

	// Set the keyboard and mouse focus
	webview.setFocus();

	// Some extra settings
	pSettings = webview.settings();

	Q_ASSERT(pSettings != NULL);

//TODO : enable via comamnd line arguments
#ifdef _DEBUG_TOOLS_
	pSettings->setAttribute(QWebSettings::DeveloperExtrasEnabled, true);
#endif

	pSettings->setAttribute(QWebSettings::AcceleratedCompositingEnabled, true);
	pSettings->setAttribute(QWebSettings::WebGLEnabled, false);
	pSettings->setAttribute(QWebSettings::PluginsEnabled, false);

	pSettings->setAttribute(QWebSettings::OfflineWebApplicationCacheEnabled, true);
	pSettings->setAttribute(QWebSettings::LocalStorageEnabled, true);

	pSettings->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls, true);
	pSettings->setAttribute(QWebSettings::LocalContentCanAccessFileUrls, true);
//	pSettings->setAttribute(QWebSettings::FrameFlatteningEnabled, true);
	pSettings->setAttribute(QWebSettings::WebSecurityEnabled, false);
	pSettings->setAttribute(QWebSettings::SpatialNavigationEnabled, false);

	// Overrule the cache settings
/*
	pSettings->setMaximumPagesInCache(0);
	pSettings->setObjectCacheCapacities(0, 0, 0);
	pSettings->QWebSettings::clearMemoryCaches();
*/

	// Finalize
#ifdef QT_OPENGL_LIB
//	view.setViewport(pWidget);

//TODO : check buffer settings (platform)
//TODO : check 'old' solution
//TODO : command line arguments
	view.setViewport(new QGLWidget(QGL::DirectRendering | QGL::DoubleBuffer));
#endif

	scene.addItem(&webview);

#ifdef _DEBUG_TOOLS_
//TODO : print return value
	page.setProperty("_q_webInspectorServerPort", INSPECTOR_PORT);
#endif

	// Set visibility
	webview.show();
}

MLWebKit::~MLWebKit()
{
}

void MLWebKit::load(QUrl url)
{
	qDebug () << "load ( url ) : " << url;
	webview.load(url);
}

void MLWebKit::show()
{
	qDebug () << "show ()";
	view.show();
}

void MLWebKit::hide()
{
	qDebug () << "hide ()";
	view.hide();
}

#if defined (_PLAYER_) || defined (_PROPERTYCHANGER_) || defined (_DEBUG_TOOLS_)
void MLWebKit::attach_object(QObject* pObject, const QString name)
{
	qDebug () << "attach object to bridge";

	if(pObject == NULL)
	{
		qWarning () << "invalid object provided for webkit bridge";	
		return;
	}

	QWebFrame*  pFrame = page.mainFrame();

	if (pFrame != NULL )
	{
		qDebug () << "change (NULL) parent to pFrame";
		pObject->setParent(pFrame);

		qDebug () << "add webkit bridge for object " << pObject;
//TODO: connect to slot to keep the object accessible when page has changed
		pFrame->addToJavaScriptWindowObject(name, pObject);
	}
	else
		qWarning () << "unable to add webkit bridge for object " << pObject;	
}
#endif

#ifdef _DEBUG_TOOLS_
void MLWebKit::collect_garbage()
{
	QWebSettings::garbagecollectnow();
// TODO: check if gc is available
}

void MLWebKit::clear_caches()
{
	QWebSettings::clearMemoryCaches();
}

void MLWebKit::toggle_inspector()
{
	qDebug () << "toggle web inspector";

//TODO : print return value

	if (page.property("_q_webInspectorServerPort") == INSPECTOR_PORT)
		page.setProperty("_q_webInspectorServerPort", -1);
	else
		page.setProperty("_q_webInspectorServerPort", INSPECTOR_PORT);

}
#endif
