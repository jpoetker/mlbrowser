#ifndef MLSSLERROR_H
#define MLSSLERROR_H

#include <QObject>

#include <QNetworkReply>
#include <QSslError>

class SSLError : public QObject
{
	Q_OBJECT

public:
	SSLError();
	SSLError(QObject* object);
	~SSLError();

public slots:
	void handleSslErrors(QNetworkReply* reply, const QList<QSslError>& errors);

private:
	QObject* object;
};
#endif
