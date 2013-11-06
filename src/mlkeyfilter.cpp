#include "mlkeyfilter.h"

#include <QDebug>
#include <QEvent>
#include <QKeyEvent>
#include <QApplication>

#include "mlwebkit.h"

bool KeyFilter::eventFilter(QObject* pObject, QEvent* pEvent)
{
	if ( pEvent->type() == QEvent::KeyPress )
	{
//TODO: understand why a stream of keycode = 0 with alternating repeat occurs 

		QKeyEvent* pKeyEvent = static_cast< QKeyEvent* >( pEvent );

		/*
		pKeyEvent->count
		pKeyEvent->isAutoRepeat
		pKeyEvent->key
		pKeyEvent->matches
		pKeyEvent->modifiers
		pKeyEvent->nativeModifiers
		pKeyEvent->nativeScanCode
		pKeyEvent->nativeVirtualKey
		pKeyEvent->text
		*/

		if ( pKeyEvent->key() != 0 ) // avoid a (default) stream of output
		{
			qDebug() << "key code (Qt::Key) : " << QString("%1").arg(pKeyEvent->key(), 0, 16) << " : unicode : " << pKeyEvent->text() << " : modifiers (or of Qt::KeyboardModifier's) : "<< pKeyEvent->modifiers() << " : keypress event (true/false) : " << true << " : key by autorepeat mechanism (true/false) : " << pKeyEvent->isAutoRepeat();
//TODO: check values

			switch ( pKeyEvent->key() )
			{
				case Qt::Key_Select : // 0x1010000
				{
					QKeyEvent keyevent ( QEvent::KeyPress, Qt::Key_Enter, pKeyEvent->modifiers(), "", pKeyEvent->isAutoRepeat(), 1 );
					qDebug () << "send event : " << QCoreApplication::sendEvent ( reinterpret_cast<QObject*> (QApplication::focusWidget()), &keyevent ); 
					return true;
				}

				case Qt::Key_Menu : // 0x1000055
        	        	{
					QKeyEvent keyevent ( QEvent::KeyPress, Qt::Key_Backspace, pKeyEvent->modifiers(), "", pKeyEvent->isAutoRepeat(), 1 );
					qDebug () << "send event : " << QCoreApplication::sendEvent ( reinterpret_cast<QObject*> (QApplication::focusWidget()), &keyevent ); 
					return true;
        		        } 

				case Qt::Key_HomePage : // 'Apps', 0x1000090
				{
					QKeyEvent keyevent ( QEvent::KeyPress, Qt::Key_F8, pKeyEvent->modifiers(), "", pKeyEvent->isAutoRepeat(), 1 );
					qDebug () << "send event : " << QCoreApplication::sendEvent ( reinterpret_cast<QObject*> (QApplication::focusWidget()), &keyevent ); 
					return true;
				}
				case Qt::Key_Favorites : // 'FAV1', 0x1000091
				{
					QKeyEvent keyevent ( QEvent::KeyPress, Qt::Key_F10, pKeyEvent->modifiers(), "", pKeyEvent->isAutoRepeat(), 1 );
					qDebug () << "send event : " << QCoreApplication::sendEvent ( reinterpret_cast<QObject*> (QApplication::focusWidget()), &keyevent ); 
					return true;
				}
#ifdef _DEBUG_TOOLS_
				case Qt::Key_VolumeMute : // 0x1000071
				{	
					QKeyEvent keyevent ( QEvent::KeyPress, Qt::Key_BracketRight, Qt::ControlModifier, "", pKeyEvent->isAutoRepeat(), 1 );
					qDebug () << "send event : " << QCoreApplication::sendEvent ( reinterpret_cast<QObject*> (QApplication::focusWidget()), &keyevent ); 
					return true;
				}

				case Qt::Key_F1 : //
				{
					// Call the garbage collector
					MLWebKit& webkit = MLWebKit::instance();
					webkit.collectGarbage();
					return true;
				}

				case Qt::Key_F2 : //
				{
					// Clear memory caches
					MLWebKit& webkit = MLWebKit::instance();
					webkit.clearCaches();
					return true;
				}
#endif
				case Qt::Key_F3 : //
				{
					QApplication* pApp = (QApplication *)QApplication::instance();
					Q_ASSERT(pApp != NULL);
					//pApp->quit();
					pApp->exit();
					return true;
				}

				default:;
			}
		}
	}	

	return QObject::eventFilter(pObject, pEvent);
}
