#ifndef MLWEBKIT_H
#define MLWEBKIT_H

#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsWebView>
#include <QWebPage>
#include <QWebFrame>

#ifdef QT_OPENGL_LIB
#include <QGLWidget>
#endif

#ifdef _DEBUG_TOOLS_
#include <QWebInspector>
#include <QGraphicsProxyWidget>
#include <QWebSettings>
#endif

#ifdef _SSLERROR_
#include "mlsslerror.h"
#endif

class MLWebKit
{
public:
	MLWebKit();
	~MLWebKit();

	void load(QUrl url);
	void show();
	void hide();

#if defined (_PLAYER_) || defined (_PROPERTYCHANGER_)
	void attach_object(QObject* pObject, QString _name_);
#endif

#ifdef _DEBUG_TOOLS_
	static MLWebKit* instance();
	void inspector();

	void collect_garbage();
	void clear_caches();
#endif

private:
	QGraphicsView*		pView;
	QGraphicsScene*		pScene;
	QGraphicsWebView*	pWebview;
	QWebPage*		pPage;
	QWebFrame*		pFrame;

	QObject*		pObject;

#ifdef QT_OPENGL_LIB
	QGLWidget*		pWidget; // viewport
#endif

#ifdef _DEBUG_TOOLS_
	static MLWebKit*	pWebKit;
	QWebInspector*		pInspector;
	QGraphicsProxyWidget*	pProxyWidget;
	QWebSettings*		pSettings;
#endif

#ifdef _SSLERROR_
	SSLError*		pSSLerror;
#endif
};
#endif
