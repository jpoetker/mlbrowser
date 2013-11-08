#ifndef MLAPPLICATION_H
#define MLAPPLICATION_H

#include <QApplication>

class MLApplication : public QApplication
{
private:
	MLApplication();
        MLApplication(const MLApplication& mlapplication);
        MLApplication& operator=(const MLApplication& mlapplication);

public:
	MLApplication(int argc, char *argv[]);
	virtual ~MLApplication();
};
#endif
