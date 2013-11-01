#ifndef MLWEBKIT_H
#define MLWEBKIT_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsWebView>
#include <QWebPage>

#ifdef QT_OPENGL_LIB
#include <QGLWidget>
#endif

#ifdef _DEBUG_TOOLS_
#include <QWebSettings>
#endif

#ifdef _SSLERROR_
#include "mlsslerror.h"
#endif

#ifdef _DEBUG_TOOLS_
class MLWebKit : public QObject
{
	Q_OBJECT
#else
class MLWebKit
{
#endif
private:
	MLWebKit();
	MLWebKit(MLWebKit& mlwebkit);
	MLWebKit& operator=(const MLWebKit& mlwebkit);

	bool initialize(void);

public:
	~MLWebKit();
	static MLWebKit& instance();
	void destroy();

	void load();
	void load(QUrl url);
	void show();
	void hide();

	bool reset(void); // reset to provided command line value, basically, does what initialize() does

#if defined (_PLAYER_) || defined (_PROPERTYCHANGER_) || defined (_DEBUG_TOOLS_)
	void attach_object(QObject& pObject);
#endif

#ifdef _DEBUG_TOOLS_
	Q_INVOKABLE void collect_garbage();
	Q_INVOKABLE void clear_caches();

public slots:
	void attach_objects(void);
#endif

private:
	QGraphicsView		view;
	QGraphicsScene		scene;
	QGraphicsWebView	webview;
	QWebPage		page;

#ifdef QT_OPENGL_LIB
	QGLWidget*		pWidget; // viewport
//	QGLWidget		widget; // viewport
#endif

#ifdef _DEBUG_TOOLS_
	QObjectList		list; // list of QObject*	
#endif

	QWebSettings*		pSettings;

#ifdef _SSLERROR_
	SSLError		sslerror;
#endif
};
#endif
