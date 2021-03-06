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
	static MLWebKit*  mlwebkit=NULL;

	if (mlwebkit == NULL)
		mlwebkit  = new MLWebKit();
	Q_ASSERT(mlwebkit!=NULL);

	return *mlwebkit;
} 

MLWebKit::MLWebKit() 
{
#ifdef _DEBUG_TOOLS_
	setObjectName("browser");
#endif

	webview.setPage(&page);
	view.setScene(&scene);
	scene.addItem(&webview);

	// Set the keyboard and mouse focus
	webview.setFocus();

#ifdef QT_OPENGL_LIB
	pWidget = new QGLWidget(QGL::DirectRendering | QGL::DoubleBuffer);

//TODO : check buffer settings (platform)
//TODO : control via command line arguments

	view.setViewport(pWidget);
#endif

	initialize();
	reset();

	// Set visibility
	webview.show();
}

bool MLWebKit::initialize(void) 
{
	QApplication* pApp = dynamic_cast<QApplication*>(QApplication::instance());
	Q_ASSERT(pApp != NULL);

	QDesktopWidget* pDesktop = QApplication::desktop();
	Q_ASSERT(pDesktop != NULL);

	// Some extra settings
	pSettings = webview.settings();
	Q_ASSERT(pSettings != NULL);

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

	// Proper (re)sizing, full screen
	qDebug () << "geometry : " << pDesktop->screenGeometry().size();

//TODO : implement check
	webview.resize(QApplication::desktop()->screenGeometry().size());

/*
	pSettings->setAttribute(QWebSettings::PluginsEnabled, false);
	pSettings->setAttribute(QWebSettings::LocalContentCanAccessRemoteUrls, true);
	pSettings->setAttribute(QWebSettings::LocalContentCanAccessFileUrls, true);
	pSettings->setAttribute(QWebSettings::FrameFlatteningEnabled, true);
	pSettings->setAttribute(QWebSettings::SpatialNavigationEnabled, false);
*/

#ifdef _EXTENDED_QWEBSETTINGS_
	// Requires qt5webkit-qwebsettings-websecurity.patch, a copy can be found at https://github.com/msieben/buildroot-rpi/blob/master/package/qt5/qt5webkit/release-patches/qt5webkit-qwebsettings-websecurity.patch
	pSettings->setAttribute(QWebSettings::WebSecurityEnabled, false);
#endif

#ifdef _DEBUG_TOOLS_
	clearCaches();

	QWebFrame* pFrame = page.mainFrame();
	Q_ASSERT(pFrame!=NULL);

//TODO : check return values
	QObject::disconnect(pFrame,SIGNAL(javaScriptWindowObjectCleared()),this,SLOT(attachObjects()));
	QObject::connect(pFrame,SIGNAL(javaScriptWindowObjectCleared()),this,SLOT(attachObjects()));
#endif

#ifdef _SSLERROR_
//TODO : check networkAccessManager is valid
//TODO : check return value
	QObject::disconnect(page.networkAccessManager(), &QNetworkAccessManager::sslErrors, sslerror, &SSLError::handleSslErrors);
	QObject::connect(page.networkAccessManager(), &QNetworkAccessManager::sslErrors, sslerror, &SSLError::handleSslErrors);
#endif

	return true;
}

bool MLWebKit::reset(void) 
{
	qWarning () << "Unset parameters default their values";

	QStringList arguments = QCoreApplication::arguments();                                                                                 

	QString value[3]={"", "", ""};
	int index = -1;
	bool ok = false;

	index=arguments.indexOf("--screen-update-mode");
	value[0]=QString("full-screen");
	if (index > -1 && index < arguments.size()-1)                                                                                                              
	{
		value[0]=arguments.at(index+1);
	}
	qDebug () << "Option : screen-update-mode : " << value[0];

	if ( value[0].compare("smart") )
	{
		view.setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
	}
	else if ( value[0].compare("minimal") )
	{
		view.setViewportUpdateMode(QGraphicsView::MinimalViewportUpdate);
	}
	else if ( value[0].compare("bounding-rectangle") )
	{
		view.setViewportUpdateMode(QGraphicsView::BoundingRectViewportUpdate);                                                         
	}
	else /*if ( value[0].compare("full-screen") )*/
	{
		view.setViewportUpdateMode(QGraphicsView::FullViewportUpdate);
	}

	index=arguments.indexOf("--tiled-backing-store");
	value[0]=QString("yes");
	if (index > -1 && index < arguments.size()-1)                                                                                                              
	{
		value[0]=arguments.at(index+1);
	}
	qDebug () << "Option : tiled-backing-store : " << (value[0].compare("yes", Qt::CaseSensitive)==0);
	pSettings->setAttribute(QWebSettings::TiledBackingStoreEnabled,(value[0].compare("yes", Qt::CaseSensitive)==0));

	index=arguments.indexOf("--accelerated-compositing");
	value[0]=QString("yes");
	if (index > -1 && index < arguments.size()-1)                                                                                                              
	{
		value[0]=arguments.at(index+1);
	}
	qDebug () << "Option : accelerated-compositing : " << (value[0].compare("yes", Qt::CaseSensitive)==0);
	pSettings->setAttribute(QWebSettings::AcceleratedCompositingEnabled,(value[0].compare("yes", Qt::CaseSensitive)==0));

	index=arguments.indexOf("--webgl");
	value[0]=QString("no");
	if (index > -1 && index < arguments.size()-1)                                                                                                              
	{
		value[0]=arguments.at(index+1);
	}
	qDebug () << "Option : webgl : " << (value[0].compare("yes", Qt::CaseSensitive)==0);
	pSettings->setAttribute(QWebSettings::WebGLEnabled, (value[0].compare("yes", Qt::CaseSensitive)==0));

	index=arguments.indexOf("--max-cached-pages");
	value[0]=QString("0");
	if (index > -1 && index < arguments.size()-1)                                                                                                              
	{
		value[0]=arguments.at(index+1);
	}
	qDebug () << "Option : max-cached-pages : " << value[0];
	pSettings->setMaximumPagesInCache(value[0].toLong(&ok)); // 0 on error

	index=arguments.indexOf("--object-cache");
	value[0]=QString("0");
	value[1]=QString("0");
	value[2]=QString("0");
	if (index > -1 && index < arguments.size()-3)                                                                                                              
	{
		value[0]=arguments.at(index+1);
		value[1]=arguments.at(index+2);
		value[2]=arguments.at(index+3);
	}
	qDebug () << "Option : object-cache : " << value[0] << " " << value[1] << " " << value [2];
	pSettings->setObjectCacheCapacities(value[0].toLong(&ok), value[1].toLong(&ok), value[2].toLong(&ok)); // 0 on error

	index=arguments.indexOf("--offline-application-cache");
	value[0]=QString("no");
	if (index > -1 && index < arguments.size()-1)                                                                                                              
	{
		value[0]=arguments.at(index+1);
	}
	qDebug () << "Option : offline-application-cache : " << (value[0].compare("yes", Qt::CaseSensitive)==0);
	pSettings->setAttribute(QWebSettings::OfflineWebApplicationCacheEnabled, (value[0].compare("yes", Qt::CaseSensitive)==0));

	index=arguments.indexOf("--local-storage");
	value[0]=QString("no");
	if (index > -1 && index < arguments.size()-1)                                                                                                              
	{
		value[0]=arguments.at(index+1);
	}
	qDebug () << "Option : local storage : " << (value[0].compare("yes", Qt::CaseSensitive)==0);
	pSettings->setAttribute(QWebSettings::LocalStorageEnabled, (value[0].compare("yes", Qt::CaseSensitive)==0));

#ifdef _DEBUG_TOOLS_
	index=arguments.indexOf("--developer-options");
	value[0]=QString("no");
	if (index > -1 && index < arguments.size()-1)                                                                                                              
	{
		value[0]=arguments.at(index+1);
	}
	qDebug () << "Option : developer-options : " << (value[0].compare("yes", Qt::CaseSensitive)==0);
	pSettings->setAttribute(QWebSettings::DeveloperExtrasEnabled, (value[0].compare("yes", Qt::CaseSensitive)==0));

	if (pSettings->testAttribute(QWebSettings::DeveloperExtrasEnabled) == true)
	{
		index=arguments.indexOf("--webinspector-port");
		value[0]=QString("0");
		if (index > -1 && index < arguments.size()-1)                                                                                                              
		{
			value[0]=arguments.at(index+1);
		}
		qDebug () << "Option : webinspector : " << value[0];
		page.setProperty("_q_webInspectorServerPort", value[0]);

		// Requires qt5webkit-add-compositing-visuals-to-qwebsettings.patch, a copy can be found at https://github.com/msieben/patches/blob/master/qtwebkit/untested/qt5webkit-add-compositing-visuals-to-qwebsettings.patch
		index=arguments.indexOf("--compositing-visuals");
		value[0]=QString("no");
		if (index > -1 && index < arguments.size()-1)                                                                                                              
		{
			value[0]=arguments.at(index+1);
		}
		qDebug () << "Option : compositng-visuals : " << (value[0].compare("yes", Qt::CaseSensitive)==0);
		pSettings->setAttribute(QWebSettings::RepaintCounter, (value[0].compare("yes", Qt::CaseSensitive)==0));
		pSettings->setAttribute(QWebSettings::DebugBorder, (value[0].compare("yes", Qt::CaseSensitive)==0));
	}
#endif

	index=arguments.indexOf("--url");
	value[0]=QString("http://www.google.com");
	if (index > -1 && index < arguments.size()-1)                                                                                                              
	{
		value[0]=arguments.at(index+1);
	}
	qDebug () << "Option : url : " << value[0];
	webview.setUrl(value[0]);
	
	return true;
}

/*virtual*/MLWebKit::~MLWebKit()
{	
	QWebFrame* pFrame = page.mainFrame();
	Q_ASSERT(pFrame!=NULL);

	QObject::disconnect(pFrame,SIGNAL(javaScriptWindowObjectCleared()),this,SLOT(attachObjects()));
#ifdef _SSLERROR_
	QObject::disconnect (page.networkAccessManager(), &QNetworkAccessManager::sslErrors, sslerror, &SSLError::handleSslErrors);
#endif

#ifdef _DEBUG_TOOLS_
	clearCaches();
#endif

	if (pWidget != NULL)
		delete pWidget;
}

void MLWebKit::load()
{
	load(webview.url());
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

void MLWebKit::destroy()
{
	delete this;
}

#if defined (_PLAYER_) || defined (_PROPERTYCHANGER_) || defined (_DEBUG_TOOLS_)
void MLWebKit::attachObjects(void)
{
//TODO : unload connected objects to avoid multiple 

	QWebFrame* pFrame = page.mainFrame();

	if (pFrame != NULL )
		foreach(QObject* pObject, list)
		{
			qDebug () << "create/connect webkit bridge for object " << pObject;
			pFrame->addToJavaScriptWindowObject(pObject->objectName(), pObject);
		}
}

void MLWebKit::attachObject(QObject& pObject)
{
//TODO : check if object has not already been added

	QWebFrame* pFrame = page.mainFrame();

	if (pFrame != NULL )
	{
		pObject.setParent(pFrame);
		list.append(&pObject);
	}
	else
		qWarning () << "unable to add object to webkit bridge" << &pObject;	
}
#endif

#ifdef _DEBUG_TOOLS_
void MLWebKit::collectGarbage()
{
// TODO: check if gc is available
	// Requires qt5webki-add-js-garbage-collection-api.patch, a copy can be found at https://github.com/msieben/buildroot-rpi/blob/master/package/qt5/qt5webkit/release-patches/qt5webkit-add-js-garbage-collection-api.patch
	QWebSettings::garbageCollectNow();
}

void MLWebKit::clearCaches()
{
	QWebSettings::clearMemoryCaches();
}
#endif
