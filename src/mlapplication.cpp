#include "mlapplication.h"

#include <QDebug>

MLApplication::MLApplication(int argc, char *argv[]) : QApplication(argc, argv)
{
	QCoreApplication::setOrganizationName("Metrological");
	QCoreApplication::setOrganizationDomain("metrological.com");
	QCoreApplication::setApplicationName("ML Browser"); 

	/* set application (wide) settings */
}

/*virual*/MLApplication::~MLApplication()
{
}
