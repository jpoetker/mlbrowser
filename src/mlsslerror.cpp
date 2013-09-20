#include "mlsslerror.h"

#include <QDebug>
#include <QSslError>

SSLError::SSLError()
{
}

SSLError::~SSLError()
{
}

SSLError::SSLError( QObject* object )
{
}

void SSLError::handleSslErrors( QNetworkReply* reply, const QList<QSslError>& errors )
{
	foreach ( QSslError error, errors )
		qDebug () << error.errorString();
}
