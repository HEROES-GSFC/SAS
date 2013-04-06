/*
 * splashscreen.h
 *
 *  Created on: Oct 19, 2009
 *      Author: fgobeil
 */

#ifndef __SPLASHSCREEN_H__
#define __SPLASHSCREEN_H__

#include <QtGui/QDialog>
#include <QtGui/QLabel>
#include <QtGui/QPixmap>


class SplashScreen  : public QDialog
{
    Q_OBJECT

public:

    SplashScreen();
	virtual ~SplashScreen();

protected:

	void paintEvent( QPaintEvent * event );

private:

	void CreateLayout();

	QLabel *mBitmapLabel;
	QLabel *mGEVPlayerLabel;
	QLabel *mPureGEVLabel;
	QLabel *mCopyrightLabel;
	QLabel *mPleoraLabel;
	QPixmap *mBackground;
};


#endif /* __SPLASHSCREEN_H__ */
