#pragma once

#include "threaddisplay.h"


#include <QtGui/QDialog>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QLineEdit>
#include <QtGui/QGroupBox>
#include <QtGui/QComboBox>
#include <QtGui/QCheckBox>
#include <QtCore/QTime>

#include <PvConfigurationReader.h>
#include <PvConfigurationWriter.h>
#include <PvBufferWriter.h>


typedef enum
{
    ONEOUTOF=0,
    MAXRATE=1,
    AVERAGETHROUGHPUT=2,
    NOTHROTTLE=3

} SaveThrottleOption;

typedef enum
{
    FORMAT_BMP = 0,
    FORMAT_RAW = 1

} FormatOption;


class ImageSaveDlg  : public QDialog
{
    Q_OBJECT

public:

	ImageSaveDlg( QWidget* aParent, FilteringDlg *aFilteringDlg );
	virtual ~ImageSaveDlg();


    bool GetSaveEnabled() const;
    void SetSaveEnabled( bool aEnabled );
    bool SaveThisImage();
    bool SaveIfNecessary( PvBuffer *aRawBuffer );
    void Save( PvConfigurationWriter& aWriter );
    void Load( PvConfigurationReader& lReader );
    void SetThreadDisplay( ThreadDisplay* aDisplay ) { mThreadDisplay = aDisplay; }
    void EnableInterface();

    void ResetStats();
    double GetFPS() { return mFPS; }
    double GetMbps() { return mMbps; }
    PvInt32 GetFrames() { return mFrames; }
    PvInt64 GetTotalSize() { return mTotalSize / 1048576; }
    int exec();

protected:

	QGroupBox *CreateFormatBox();
	QGroupBox *CreateLocationBox();
	QGroupBox *CreateThrottlingBox();
	void CreateLayout();

protected slots:

	void accept();
	void reject();
	void OnRadioClicked();
	void OnBnClickedLocationButton();

private:

    bool SaveImage( PvBuffer *aBuffer );

    QLineEdit *mOneOutOfEdit;
    QLineEdit *mMaxRateEdit;
    QLineEdit *mAverageThroughputEdit;
    QLineEdit *mSavePathEdit;
    QCheckBox *mSaveEnabledCheck;

    QPushButton *mLocationButton;
    QPushButton *mOKButton;
    QPushButton *mCancelButton;
    QRadioButton *mOneOutOfRadio;
    QRadioButton *mMaxRateRadio;
    QRadioButton *mAverageThroughputRadio;
    QRadioButton *mNoThrottleRadio;
    QLabel *mCapturedImagesLabel;
    QLabel *mMsLabel;
    QLabel *mAverageLabel;
    QComboBox *mFormatCombo;

    bool mSaveEnabled;
    double mFPS;
    double mMbps;
    PvUInt32 mFrames;
    PvUInt32 mCount;
    PvInt64 mTotalSize;

    SaveThrottleOption mSaveThrottling;

    PvUInt32 mOneOutOf;
    PvUInt32 mMaxRate;
    PvUInt32 mAverageThroughput;

    QTime mStartTime;
    QTime mPrevious;

    PvUInt32 mCapturedSince;

    FormatOption mFormat;

    ThreadDisplay* mThreadDisplay;
    FilteringDlg *mFilteringDlg;

    PvBufferWriter mBufferWriter;

    QString mSavePath;
};
