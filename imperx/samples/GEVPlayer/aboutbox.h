#ifndef ABOUTBOX_H_
#define ABOUTBOX_H_


#include <QtGui/QDialog>
#include <QtGui/QPushButton>
#include <QtGui/QLabel>

class AboutBox  : public QDialog
{
    Q_OBJECT

public:

	AboutBox( QWidget *aParent );
	virtual ~AboutBox();

private:

	void CreateLayout();

	QPushButton* mOKButton;
	QLabel *mBitmapLabel;
	QLabel *mGEVPlayerLabel;
	QLabel *mPureGEVLabel;
	QLabel *mCopyrightLabel;
	QLabel *mPleoraLabel;
	QPixmap *mBackground;
};


#endif /* ABOUTBOX_H_ */
