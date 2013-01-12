// ImageSaveDlg.cpp : implementation file
//

#include "gevplayer.h"
#include "imagesavedlg.h"
#include "filteringdlg.h"

#include <assert.h>
#include <QtGui/QFileDialog>
#include <QtCore/QDir>

#ifdef WIN32
#include <shlobj.h>
#endif

#ifndef WIN32
#include <sys/time.h>
#endif


ImageSaveDlg::ImageSaveDlg( QWidget* aParent, FilteringDlg *aFilteringDlg )
    : QDialog( aParent )
    , mThreadDisplay( NULL )
    , mSaveThrottling( MAXRATE )
    , mOneOutOf( 1 )
    , mMaxRate( 100 )
    , mAverageThroughput( 1 )
    , mCapturedSince( 0 )
    , mSaveEnabled( false )
    , mFrames( 0 )
    , mFPS( 0 )
    , mMbps( 0 )
    , mTotalSize( 0 )
    , mFormat( FORMAT_BMP )
    , mFilteringDlg( aFilteringDlg )
    , mCount( 0 )
{
#ifdef WIN32
    char myPictures[MAX_PATH];
	SHGetSpecialFolderPath(NULL, myPictures,CSIDL_MYPICTURES,true);

    mSavePath = myPictures;
#endif //WIN32

#ifdef _LINUX_
    //TODO: Find API call to get the proper default folder
    mSavePath = "";
#endif //_LINUX_
    CreateLayout();

    mPrevious.start();
    mStartTime.start();

    mSavePath = QDir::homePath();
}


int ImageSaveDlg::exec()
{

	switch ( mFormat )
	{
	default:
		assert( 0 );
		break;

	case FORMAT_BMP:
		mFormatCombo->setCurrentIndex( 0 );
		break;

	case FORMAT_RAW:
		mFormatCombo->setCurrentIndex( 1 );
		break;
	}

	QVariant lOneOutOf(mOneOutOf);
	mOneOutOfEdit->setText( lOneOutOf.toString() );
	QVariant lMaxRate(mMaxRate);
	mMaxRateEdit->setText( lMaxRate.toString() );
	QVariant lAverageThroughput(mAverageThroughput);
	mAverageThroughputEdit->setText( lAverageThroughput.toString() );
	switch ( mSaveThrottling )
	{
	default:
		assert( 0 );
		break;

	case ONEOUTOF:
		mOneOutOfRadio->setChecked(true);
		break;
	case MAXRATE:
		mMaxRateRadio->setChecked(true);
		break;
	case AVERAGETHROUGHPUT:
		mAverageThroughputRadio->setChecked(true);
		break;
	case NOTHROTTLE:
		mNoThrottleRadio->setChecked(true);
		break;
	}

	mSavePathEdit->setText( mSavePath );

	mSaveEnabledCheck->setChecked( mSaveEnabled );
	EnableInterface();

	return QDialog::exec();
}

///
/// \brief Creates the form's layout
///

void
ImageSaveDlg::CreateLayout()
{

	mSaveEnabledCheck = new QCheckBox("Enable Image Saving");
    QObject::connect( mSaveEnabledCheck, SIGNAL( clicked() ), this, SLOT( OnRadioClicked() ) );

	QGroupBox *lLocationBox = CreateLocationBox();
	QGroupBox *lFormatBox = CreateFormatBox();
	QGroupBox *lThrottlingBox = CreateThrottlingBox();
	QHBoxLayout *lButtons = new QHBoxLayout;
    mOKButton = new QPushButton( tr( "OK" ) );
    mCancelButton = new QPushButton( tr( "Cancel" ) );
    lButtons->addStretch();
    lButtons->addWidget( mOKButton );
    lButtons->addWidget( mCancelButton );

    QObject::connect( mOKButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
    QObject::connect( mCancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );

    QVBoxLayout *lLayout = new QVBoxLayout;
    lLayout->addWidget(mSaveEnabledCheck);
    lLayout->addWidget(lLocationBox);
    lLayout->addWidget(lFormatBox);
    lLayout->addWidget(lThrottlingBox);
    lLayout->addStretch( 100 );
    lLayout->addLayout(lButtons);

    setLayout( lLayout );
    setFixedSize( 520, 400 );

    Qt::WindowFlags lFlags = windowFlags();
    lFlags |= Qt::CustomizeWindowHint;
    lFlags &= ~Qt::WindowContextHelpButtonHint;
    lFlags &= ~Qt::WindowSystemMenuHint;
    setWindowFlags( lFlags );

    setWindowTitle( tr( "Image Saving Options" ) );
}

QGroupBox *
ImageSaveDlg::CreateFormatBox()
{
	QGroupBox *lFormatBox = new QGroupBox( tr( "Format" ) );
	mFormatCombo = new QComboBox();
	mFormatCombo->addItem( "Device independent bitmap (BMP)" );
	mFormatCombo->addItem( "Raw binary data" );
	QHBoxLayout *lLayout = new QHBoxLayout;
	lLayout->addWidget(mFormatCombo);
	lFormatBox->setLayout(lLayout);
	return lFormatBox;
}

///
/// \brief Create location group box
///

QGroupBox *
ImageSaveDlg::CreateLocationBox()
{
	QGroupBox *lLocationBox = new QGroupBox( tr( "Location to save images" ) );

	mSavePathEdit = new QLineEdit();

	mLocationButton = new QPushButton( "..." );
	mLocationButton->setMaximumWidth( 40 );

	QHBoxLayout *lLayout = new QHBoxLayout;
	lLayout->addWidget(mSavePathEdit);
	lLayout->addWidget(mLocationButton);

	QObject::connect( mLocationButton, SIGNAL( clicked() ), this, SLOT( OnBnClickedLocationButton() ) );

	lLocationBox->setLayout( lLayout );
	return lLocationBox;
}


///
/// \brief Create throttling group box
///

QGroupBox *
ImageSaveDlg::CreateThrottlingBox()
{
	QGroupBox *lThrottlingBox = new QGroupBox( tr( "Throttling options" ) );

	mOneOutOfRadio = new QRadioButton("Save one image out of every");
	mOneOutOfEdit = new QLineEdit();
	mOneOutOfEdit->setMinimumWidth( 75 );
	mOneOutOfEdit->setMaximumWidth( 75 );
	mCapturedImagesLabel = new QLabel("captured images.");
	QHBoxLayout *lOneOutOfLayout = new QHBoxLayout;
	lOneOutOfLayout->addWidget(mOneOutOfRadio);
	lOneOutOfLayout->addWidget(mOneOutOfEdit);
	lOneOutOfLayout->addWidget(mCapturedImagesLabel);
	lOneOutOfLayout->addStretch( 100 );
	QObject::connect( mOneOutOfRadio, SIGNAL( clicked() ), this, SLOT( OnRadioClicked() ) );

	mMaxRateRadio = new QRadioButton("Save a maximum of one image out of every");
	mMaxRateEdit = new QLineEdit();
	mMaxRateEdit->setMinimumWidth( 75 );
	mMaxRateEdit->setMaximumWidth( 75 );
	mMsLabel = new QLabel("ms.");
	QHBoxLayout *lMaxRateLayout = new QHBoxLayout;
	lMaxRateLayout->addWidget(mMaxRateRadio);
	lMaxRateLayout->addWidget(mMaxRateEdit);
	lMaxRateLayout->addWidget(mMsLabel);
	lMaxRateLayout->addStretch( 100 );
	QObject::connect( mMaxRateRadio, SIGNAL( clicked() ), this, SLOT( OnRadioClicked() ) );

	mAverageThroughputRadio = new QRadioButton("Throttle throughput to HDD. Maintain");
	mAverageThroughputEdit = new QLineEdit();
	mAverageThroughputEdit->setMinimumWidth( 75 );
	mAverageThroughputEdit->setMaximumWidth( 75 );
	mAverageLabel = new QLabel("Mbits/s average.");
	QHBoxLayout *lAverageThroughputLayout = new QHBoxLayout;
	lAverageThroughputLayout->addWidget(mAverageThroughputRadio);
	lAverageThroughputLayout->addWidget(mAverageThroughputEdit);
	lAverageThroughputLayout->addWidget(mAverageLabel);
	lAverageThroughputLayout->addStretch( 100 );
	QObject::connect( mAverageThroughputRadio, SIGNAL( clicked() ), this, SLOT( OnRadioClicked() ) );

	mNoThrottleRadio = new QRadioButton("No throttling (Limited by CPU, HDD, etc. Images may be dropped).");
	QObject::connect( mNoThrottleRadio, SIGNAL( clicked() ), this, SLOT( OnRadioClicked() ) );

	QVBoxLayout *lLayout = new QVBoxLayout;
	lLayout->addLayout(lOneOutOfLayout);
	lLayout->addLayout(lMaxRateLayout);
	lLayout->addLayout(lAverageThroughputLayout);
	lLayout->addWidget(mNoThrottleRadio);

	lThrottlingBox->setLayout( lLayout );
	return lThrottlingBox;
}

ImageSaveDlg::~ImageSaveDlg()
{
}


// ImageSaveDlg message handlers

void ImageSaveDlg::accept()
{

    if ( mOneOutOfRadio->isChecked() )
    {
        mSaveThrottling = ONEOUTOF;
        mOneOutOf = mOneOutOfEdit->text().toInt();
    }
    else if ( mMaxRateRadio->isChecked() )
	{
		mSaveThrottling = MAXRATE;
		mMaxRate = mMaxRateEdit->text().toInt();
	}
    else if ( mAverageThroughputRadio->isChecked() )
	{
		mSaveThrottling = AVERAGETHROUGHPUT;
		mAverageThroughput = mAverageThroughputEdit->text().toInt();
	}
    else if ( mNoThrottleRadio->isChecked() )
	{
		mSaveThrottling = NOTHROTTLE;
	}
    else
    {
    	assert(false);
    }

    mSavePath = mSavePathEdit->text();

    if ( mSaveEnabledCheck->isChecked() )
    {
        SetSaveEnabled( true );
    }
    else
    {
        SetSaveEnabled( false );
    }

    switch ( mFormatCombo->currentIndex() )
    {
    default:
        assert( 0 );
        break;

    case 0:
        mFormat = FORMAT_BMP;
        break;

    case 1:
        mFormat = FORMAT_RAW;
        break;
    }

    QDialog::accept();
}

// =============================================================================
void ImageSaveDlg::reject()
{
    QDialog::reject();
}



void ImageSaveDlg::OnBnClickedLocationButton()
{
	 QString dir = QFileDialog::getExistingDirectory(this, tr("Save images to folder"),
													 mSavePathEdit->text(),
	                                                 QFileDialog::ShowDirsOnly
	                                                 | QFileDialog::DontResolveSymlinks);

	 mSavePathEdit->setText(dir);
}


PvUInt64 GetTickCountMs()
{
#ifdef WIN32
    return ::GetTickCount();
#else
    timeval ts;
    gettimeofday( &ts, 0 );

    PvUInt64 lTickCount = (PvInt64)(ts.tv_sec * 1000LL + ( ts.tv_usec / 1000LL ) );

    return lTickCount;
#endif
}

// private
bool ImageSaveDlg::SaveImage( PvBuffer* aBuffer )
{
    QString lExt;
    PvBufferFormatType lType;

    switch ( mFormat )
    {
    default:
        assert( 0 );

    case FORMAT_BMP:
        lExt = ".bmp";
        lType = PvBufferFormatBMP;
        break;

    case FORMAT_RAW:
        lExt = ".bin";
        lType = PvBufferFormatRaw;
        break;
    }

    QString lFileName =  mSavePath;
    QString lTimeStamp;
    lTimeStamp.setNum( aBuffer->GetTimestamp(), 16 );

    QString lCount;
    lCount.setNum( mCount++, 10 );

    QString lSystemTime;
    lSystemTime.setNum( GetTickCountMs(), 10 );

#ifdef WIN32
    lFileName.append("\\");
#else
    lFileName.append("/");
#endif

    lFileName.append( lCount );
    lFileName.append( "_" );
    lFileName.append( lTimeStamp );
    lFileName.append( "_" );
    lFileName.append( lSystemTime );
    lFileName.append( lExt );
  
    mFilteringDlg->ConfigureConverter( mBufferWriter.GetConverter() );
    mBufferWriter.Store( aBuffer, lFileName.toAscii().data(), lType );

    PvUInt32 lDelta =  mStartTime.elapsed();
    mFrames++;

    mFPS = (double) mFrames * 1000 / (double) lDelta;

    // 1048576 bits per Mbit
	PvUInt32 lBytesPerImage = aBuffer->GetAcquiredSize();
    mMbps = static_cast<double>( lBytesPerImage ) * 8 * mFPS / 1048576.0;
    mTotalSize += lBytesPerImage;

    return true;
}

bool ImageSaveDlg::GetSaveEnabled() const
{
    return mSaveEnabled;
}

void ImageSaveDlg::SetSaveEnabled( bool aEnabled )
{
    mSaveEnabled = aEnabled;
}

// called from GEVPlayerDlg
bool ImageSaveDlg::SaveThisImage()
{
    assert( mThreadDisplay != NULL );
    if ( mThreadDisplay == NULL )
    {
        return false;
    }

    bool lResult = false;

    PvBuffer* lBuffer = mThreadDisplay->RetrieveLatestBuffer();
    if ( lBuffer != NULL )
    {
        lResult = SaveImage( lBuffer );
    }
    mThreadDisplay->ReleaseLatestBuffer();


    return lResult;

}

// Called from ThreadDisplay
bool ImageSaveDlg::SaveIfNecessary( PvBuffer *aRawBuffer )
{
    if ( !mSaveEnabled )
    {
        return false;
    }

    bool lSaveThisOne = false;
    double lBitsPerImage;
    double lBitsPerMs;
    switch( mSaveThrottling )
    {
    case ONEOUTOF:
        // 1 image every mOneOf captured images
        mCapturedSince++;
        if ( mCapturedSince >= mOneOutOf )
        {
            lSaveThisOne = true;
            mCapturedSince = 0;
        }
        break;
    case MAXRATE:
        // maximum of one out of every mMaxRate ms
        if ( mPrevious.elapsed() >= static_cast<int>( mMaxRate ) )
        {
            lSaveThisOne = true;
            mPrevious.start();
        }
        break;
    case AVERAGETHROUGHPUT:
        // maintain mAverageThroughput Mbits/s average
		lBitsPerImage = aRawBuffer->GetAcquiredSize() * 8;
        lBitsPerMs = mAverageThroughput * 1048.576;
        if ( mPrevious.elapsed() >= lBitsPerImage / lBitsPerMs )
        {
            lSaveThisOne = true;
            mPrevious.start();
        }
        break;
    case NOTHROTTLE:
        lSaveThisOne = true;
        break;
    default:
        assert(FALSE);
        break;

    }

    if ( lSaveThisOne )
    {
        return SaveImage( aRawBuffer );
    }

    return false;
}


void ImageSaveDlg::Save( PvConfigurationWriter &aWriter )
{
    aWriter.Store( GetSaveEnabled() ? "1" : "0", "saveenabled" );

    QString lStr;

    aWriter.Store( lStr.setNum(mOneOutOf).toAscii().data(), "oneoutof" );
    aWriter.Store( lStr.setNum(mMaxRate).toAscii().data(), "maxrate" );
    aWriter.Store( lStr.setNum(mAverageThroughput).toAscii().data(), "averagethroughput" );
    aWriter.Store( lStr.setNum(mSaveThrottling).toAscii().data(), "savethrottleoption" );
    aWriter.Store( mSavePath.toAscii().data(), "savepath" );
    aWriter.Store( mFormat == 0 ? "bmp" : "raw", "saveformat" );

}

void ImageSaveDlg::Load( PvConfigurationReader &aReader )
{
    PvString lSaveEnabled;
    PvString lSaveEnabledTag("saveenabled");
    aReader.Restore(lSaveEnabledTag, lSaveEnabled);
    if (strcmp(lSaveEnabled.GetAscii(),"1") == 0 )
    {
        mSaveEnabled = true;
    }
    else
    {
        mSaveEnabled = false;
    }

    PvString lOneOutOf;
    aReader.Restore( "oneoutof", lOneOutOf );
    mOneOutOf = atoi( lOneOutOf.GetAscii() );

    PvString lMaxRate;
    aReader.Restore( "maxrate", lMaxRate );
    mMaxRate = atoi( lMaxRate.GetAscii() );

    PvString lAverageThroughput;
    aReader.Restore( "averagethroughput", lAverageThroughput );
    mAverageThroughput = atoi(lAverageThroughput.GetAscii());

    PvString lSaveThrottleOption;
    aReader.Restore( "savethrottleoption", lSaveThrottleOption );
    mSaveThrottling = (SaveThrottleOption)atoi( lSaveThrottleOption.GetAscii() );

    PvString lSavePath;
    aReader.Restore( "savepath", lSavePath );
    mSavePath = lSavePath.GetAscii();

    PvString lFormat;
    aReader.Restore( "saveformat", lFormat );
    QString lStr( lFormat.GetAscii() );
    QString lLower = lStr.toLower();
    if ( lLower == "bmp" )
    {
        mFormat = FORMAT_BMP;
    }
    else if ( lLower == "raw" )
    {
        mFormat = FORMAT_RAW;
    }
    else
    {
        // Default. Can be expected as the user can end up manually
        // editing the persistence file.
        mFormat = FORMAT_BMP;
    }
}

void ImageSaveDlg::OnRadioClicked()
{
   EnableInterface();
}

void ImageSaveDlg::EnableInterface()
{
	bool lEnabled = mSaveEnabledCheck->isChecked();
	mOneOutOfEdit->setEnabled( lEnabled && mOneOutOfRadio->isChecked() );
	mMaxRateEdit->setEnabled( lEnabled && mMaxRateRadio->isChecked() );
	mAverageThroughputEdit->setEnabled( lEnabled && mAverageThroughputRadio->isChecked());
	mSavePathEdit->setEnabled( lEnabled );
	mLocationButton->setEnabled( lEnabled );
	mOneOutOfRadio->setEnabled( lEnabled );
	mMaxRateRadio->setEnabled( lEnabled );
	mAverageThroughputRadio->setEnabled( lEnabled );
	mNoThrottleRadio->setEnabled( lEnabled );
	mCapturedImagesLabel->setEnabled( lEnabled );
	mMsLabel->setEnabled( lEnabled );
	mAverageLabel->setEnabled( lEnabled );
	mFormatCombo->setEnabled( lEnabled );
}

void ImageSaveDlg::ResetStats()
{
    mFPS = 0.0;
    mMbps = 0.0;
    mFrames = 0;
    mTotalSize = 0;
    mStartTime.start();
}
