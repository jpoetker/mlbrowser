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
	~MLWebKit();

public:
	static MLWebKit& instance();
	void load(QUrl url);
	void show();
	void hide();

#if defined (_PLAYER_) || defined (_PROPERTYCHANGER_) || defined (_DEBUG_TOOLS_)
	void attach_object(QObject* pObject, const QString _name_);
#endif

#ifdef _DEBUG_TOOLS_
	Q_INVOKABLE void toggle_inspector();
	Q_INVOKABLE void collect_garbage();
	Q_INVOKABLE void clear_caches();
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

	QWebSettings*		pSettings;

#ifdef _SSLERROR_
	SSLError		sslerror;
#endif
};
#endif
