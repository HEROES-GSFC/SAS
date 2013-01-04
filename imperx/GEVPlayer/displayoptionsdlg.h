/*
 * displayoptionsdlg.h
 *
 *  Created on: Oct 15, 2009
 *      Author: fgobeil
 */

#ifndef __DISPLAYOPTIONSDLG_H__
#define __DISPLAYOPTIONSDLG_H__


#include <QtGui/QDialog>
#include <QtGui/QPushButton>
#include <QtGui/QCheckBox>
#include <QtGui/QRadioButton>
#include <QtGui/QLabel>
#include <PvConfigurationReader.h>
#include <PvConfigurationWriter.h>

class ThreadDisplay;


class DisplayOptionsDlg : public QDialog
{
    Q_OBJECT

public:

    DisplayOptionsDlg( QWidget* aParent );
	virtual ~DisplayOptionsDlg();

	void Init( ThreadDisplay *aThreadDisplay );
	void Apply( ThreadDisplay *aThreadDisplay );

    void Save( PvConfigurationWriter& aWriter );
    void Load( PvConfigurationReader& aReader );

protected:

	void CreateLayout();

protected slots:

	void OnRendererSelected();

private:

	QRadioButton *mGLDisabled;
	QRadioButton *mGLEnabled;
	QRadioButton *mGLFull;
	QLabel *mRenderer;
	QLabel *mRendererVersion;
	QCheckBox *mVSyncCheck;
	QRadioButton *mFPSDisabled;
	QRadioButton *mFPS10;
	QRadioButton *mFPS30;
	QRadioButton *mFPS60;

	QPushButton *mOKButton;
	QPushButton *mCancelButton;
    QCheckBox *mKeepPartialImages;

};


#endif /* __DISPLAYOPTIONSDLG_H__ */
