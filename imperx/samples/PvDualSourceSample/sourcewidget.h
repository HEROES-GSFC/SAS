// *****************************************************************************
//
//     Copyright (c) 2012, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#ifndef __SOURCEWIDGET_H__
#define __SOURCEWIDGET_H__

#include <QtGui/QPushButton>
#include <QtGui/QComboBox>
#include <QtGui/QTextEdit>
#include <QtCore/QTimer>


class PvDisplayWnd;
class Source;


#define WM_UPDATEACQCONTROLS ( 0x4000 )
#define WM_UPDATEACQMODE ( 0x4001 )
#define WM_UPDATEACQMODES ( 0x4002 )


class SourceWidget : public QWidget
{
    Q_OBJECT

public:

    SourceWidget( const QString &aTitle, Source *aSource, QWidget *parent = 0, Qt::WFlags flags = 0 );
    virtual ~SourceWidget();

    QComboBox *GetModeComboBox() { return mModeComboBox; }
    PvDisplayWnd *GetDisplay() { return mDisplay; }

	void EnableInterface();

protected slots:

	void OnStart();
	void OnStop();
	void OnTimer();
	void OnCbnSelchangeMode( int aIndex );

protected:

    bool event( QEvent *aEvent );

    void UpdateAcquisitionMode();
    void UpdateAcquisitionModes();

    // UI
    void CreateLayout();
    QLayout *CreateDisplayLayout();
    QLayout *CreateControlsLayout();

private:

    PvDisplayWnd *mDisplay;

    QTextEdit *mStatusLine;
    QTimer *mTimer;

    QComboBox *mModeComboBox;
    QPushButton *mPlayButton;
    QPushButton *mStopButton;

    QString mTitle;

    Source *mSource;
};


#endif // __SOURCEWIDGET_H__


