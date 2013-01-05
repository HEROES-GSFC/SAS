// *****************************************************************************
//
//     Copyright (c) 2008, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#include "gevplayer.h"
#include "filteringdlg.h"
#include "threaddisplay.h"
#include "utilities.h" 

#include <math.h>
#include <assert.h>



#define FILTERINGDLG_VERSION ( "1.0.0.0" )

#define TAG_VERSION ( "filteringdlgversion" )
#define TAG_FILTERRGBENABLED ( "filterrgbenabled" )
#define TAG_REDGAIN ( "redgain" )
#define TAG_GREENGAIN ( "greengain" )
#define TAG_BLUEGAIN ( "bluegain" )
#define TAG_REDOFFSET ( "redoffset" )
#define TAG_GREENOFFSET ( "greenoffset" )
#define TAG_BLUEOFFSET ( "blueoffset" )
#define TAG_BAYERFILTERING ( "bayerfiltering" )

#define VAL_TRUE ( "true" )
#define VAL_FALSE ( "false" )
#define VAL_BAYERSIMPLE ( "simple" )
#define VAL_BAYER3X3 ( "3x3" )

#define GAIN_TICK_COUNT ( 1000 )
#define GAIN_MAX ( 16 )

#define GRID_MIN_ROW_HEIGHT ( 24 )
#define SPIN_MIN_WIDTH ( 80 )
#define SPIN_MIN_HEIGHT ( 14 )


const double cNormalizer = log( static_cast<double>( GAIN_TICK_COUNT ) );


// =============================================================================
FilteringDlg::FilteringDlg( QWidget* aParent )
    : QDialog( aParent )
    , mThreadDisplay( NULL )
    , mLoading( true )
    , mFilterRGBEnabled( false )
    , mBayerFilter( PvBayerFilterSimple )
    , mSkipSyncSpinsCount( 0 )
    , mSkipSyncSlidersCount( 0 )
    , mSettingSlider( false )
{
	CreateLayout();
	InitControls();
}


// =============================================================================
FilteringDlg::~FilteringDlg()
{
}

void FilteringDlg::CreateLayout()
{

	QGroupBox *lRGBFilteringBox = new QGroupBox("RGB Filtering");

	QFont lBoldFont;
	lBoldFont.setBold( true );

	QGridLayout *lRGBLayout = new QGridLayout ;

	// First grid row
	int lRow = 0; int lCol = 0;
	QHBoxLayout *lEnabledLayout = new QHBoxLayout;
	mRGBFilterCheckBox = new QCheckBox("Enabled");
	QObject::connect( mRGBFilterCheckBox, SIGNAL( clicked() ), this, SLOT( OnBnClickedEnabledcheck() ) );
	mGainsLabel = new QLabel("Gains");
	mGainsLabel->setFont(lBoldFont);
	mOffsetsLabel = new QLabel("Offsets");
	mOffsetsLabel->setFont(lBoldFont);
	lRGBLayout->addWidget( mRGBFilterCheckBox, lRow, lCol++ );
	lCol++; // Skip slider
	lRGBLayout->addWidget( mGainsLabel, lRow, lCol++ );
	lCol++; // Skip slider
	lRGBLayout->addWidget( mOffsetsLabel, lRow, lCol++ );

	// RED grid row
	lRow++; lCol = 0;
	lRGBLayout->setRowMinimumHeight( lRow, GRID_MIN_ROW_HEIGHT );
	mRedLabel = new QLabel("Red");
	mRedLabel->setFont(lBoldFont);
	mRGSlider = new QSlider(Qt::Horizontal);
	mRGSpin = new QDoubleSpinBox;
	mRGSpin->setMinimumHeight( SPIN_MIN_HEIGHT );
	mRGSpin->setMinimumWidth( SPIN_MIN_WIDTH );
	mROSlider = new QSlider(Qt::Horizontal);
	mROSpin = new QSpinBox;
	mROSpin->setMinimumHeight( SPIN_MIN_HEIGHT );
	mROSpin->setMinimumWidth( SPIN_MIN_WIDTH );
	lRGBLayout->addWidget( mRedLabel, lRow, lCol++ );
	lRGBLayout->addWidget( mRGSlider, lRow, lCol++ );
	lRGBLayout->addWidget( mRGSpin, lRow, lCol++ );
	lRGBLayout->addWidget( mROSlider, lRow, lCol++ );
	lRGBLayout->addWidget( mROSpin, lRow, lCol++ );
	connect(mRGSlider, SIGNAL(valueChanged(int)), this, SLOT(SetRedSpin(int)));
	connect(mRGSpin, SIGNAL(valueChanged(double)), this, SLOT(SetRedSlider(double)));
	connect(mROSlider, SIGNAL(valueChanged(int)), this, SLOT(SetRedOffsetSpin(int)));
	connect(mROSpin, SIGNAL(valueChanged(int)), this, SLOT(SetRedOffsetSlider(int)));

	// GREEN grid row    aSlider->setValue( lValue );

	lRow++; lCol = 0;
	lRGBLayout->setRowMinimumHeight( lRow, GRID_MIN_ROW_HEIGHT );
	mGreenLabel = new QLabel("Green");
	mGreenLabel->setFont(lBoldFont);
	mGGSlider = new QSlider(Qt::Horizontal);
	mGGSpin = new QDoubleSpinBox;
	mGGSpin->setMinimumHeight( SPIN_MIN_HEIGHT );
	mGGSpin->setMinimumWidth( SPIN_MIN_WIDTH );
	mGOSlider = new QSlider(Qt::Horizontal);
	mGOSpin = new QSpinBox;
	mGOSpin->setMinimumHeight( SPIN_MIN_HEIGHT );
	mGOSpin->setMinimumWidth( SPIN_MIN_WIDTH );
	lRGBLayout->addWidget( mGreenLabel, lRow, lCol++ );
	lRGBLayout->addWidget( mGGSlider, lRow, lCol++ );
	lRGBLayout->addWidget( mGGSpin, lRow, lCol++ );
	lRGBLayout->addWidget( mGOSlider, lRow, lCol++ );
	lRGBLayout->addWidget( mGOSpin, lRow, lCol++ );
	connect(mGGSlider, SIGNAL(valueChanged(int)), this, SLOT(SetGreenSpin(int)));
	connect(mGGSpin, SIGNAL(valueChanged(double)), this, SLOT(SetGreenSlider(double)));
	connect(mGOSlider, SIGNAL(valueChanged(int)), this, SLOT(SetGreenOffsetSpin(int)));
	connect(mGOSpin, SIGNAL(valueChanged(int)), this, SLOT(SetGreenOffsetSlider(int)));

	// BLUE grid row
	lRow++; lCol = 0;
	lRGBLayout->setRowMinimumHeight( lRow, GRID_MIN_ROW_HEIGHT );
	mBlueLabel = new QLabel("Blue");
	mBlueLabel->setFont(lBoldFont);
	mBGSlider = new QSlider(Qt::Horizontal);
	mBGSpin = new QDoubleSpinBox;
	mBGSpin->setMinimumHeight( SPIN_MIN_HEIGHT );
	mBGSpin->setMinimumWidth( SPIN_MIN_WIDTH );
	mBOSlider = new QSlider(Qt::Horizontal);
	mBOSpin = new QSpinBox;
	mBOSpin->setMinimumHeight( SPIN_MIN_HEIGHT );
	mBOSpin->setMinimumWidth( SPIN_MIN_WIDTH );
	lRGBLayout->addWidget( mBlueLabel, lRow, lCol++ );
	lRGBLayout->addWidget( mBGSlider, lRow, lCol++ );
	lRGBLayout->addWidget( mBGSpin, lRow, lCol++ );
	lRGBLayout->addWidget( mBOSlider, lRow, lCol++ );
	lRGBLayout->addWidget( mBOSpin, lRow, lCol++ );
	connect(mBGSlider, SIGNAL(valueChanged(int)), this, SLOT(SetBlueSpin(int)));
	connect(mBGSpin, SIGNAL(valueChanged(double)), this, SLOT(SetBlueSlider(double)));
	connect(mBOSlider, SIGNAL(valueChanged(int)), this, SLOT(SetBlueOffsetSpin(int)));
	connect(mBOSpin, SIGNAL(valueChanged(int)), this, SLOT(SetBlueOffsetSlider(int)));

	QHBoxLayout *lButtonLayout = new QHBoxLayout;
	mWBButton = new QPushButton( tr( "White Balance" ) );
	mResetButton = new QPushButton( tr( "Reset" ) );
	QObject::connect( mWBButton, SIGNAL( clicked() ), this, SLOT( OnBnClickedWbbutton() ) );
	QObject::connect( mResetButton, SIGNAL( clicked() ), this, SLOT( OnBnClickedResetbutton() ) );
	lButtonLayout->addWidget( mWBButton );
	lButtonLayout->addWidget( mResetButton );

	QVBoxLayout *lRGBFilteringLayout = new QVBoxLayout;
	lRGBFilteringLayout->addLayout(lRGBLayout);
	lRGBFilteringLayout->addLayout(lButtonLayout);

	lRGBFilteringBox->setLayout(lRGBFilteringLayout);

	QGroupBox *lBayerInterpolationBox = new QGroupBox("Bayer Interpolation");
	QVBoxLayout *lBayerInterpolationLayout = new QVBoxLayout;
	mBayerCombo = new QComboBox();
	mBayerCombo->addItem( "Simple" );
	mBayerCombo->addItem( "3X3 Interpolation" );

    QObject::connect( mBayerCombo, SIGNAL( currentIndexChanged( int ) ), this, SLOT( OnBayerComboChanged( int ) ) );

	lBayerInterpolationLayout->addWidget( mBayerCombo );
	lBayerInterpolationBox->setLayout(lBayerInterpolationLayout);

	QVBoxLayout *lLayout = new QVBoxLayout;
	lLayout->addWidget(lRGBFilteringBox);
	lLayout->addWidget(lBayerInterpolationBox);

    setLayout( lLayout );
    setFixedSize( 620, 275 );

    Qt::WindowFlags lFlags = windowFlags();
    lFlags |= Qt::CustomizeWindowHint;
    lFlags &= ~Qt::WindowContextHelpButtonHint;
    lFlags &= ~Qt::WindowSystemMenuHint;
    
    lFlags |= Qt::WindowStaysOnTopHint;
    setWindowFlags( lFlags );

    setWindowTitle( tr( "Image Filtering" ) );

}

void FilteringDlg::OnBayerComboChanged( int aIndex )
{
    FromDialog();
}

void FilteringDlg::SetRedOffsetSlider( int aValue )
{
    mROSlider->setValue( aValue );
    FromDialog();
}

void FilteringDlg::SetBlueOffsetSlider( int aValue )
{
    mBOSlider->setValue( aValue );
    FromDialog();
}

void FilteringDlg::SetGreenOffsetSlider( int aValue )
{
    mGOSlider->setValue( aValue );
    FromDialog();
}

void FilteringDlg::SetRedOffsetSpin( int aValue )
{
    mROSpin->setValue( aValue );
    FromDialog();
}

void FilteringDlg::SetBlueOffsetSpin( int aValue )
{
    mBOSpin->setValue( aValue );
    FromDialog();
}

void FilteringDlg::SetGreenOffsetSpin( int aValue )
{
    mGOSpin->setValue( aValue );
    FromDialog();
}

void FilteringDlg::SetRedSlider(double aValue)
{
    mSettingSlider = true;
	mRGSlider->setValue( ToGainSlider( mRGSlider, aValue ) );
    mSettingSlider = false;
    FromDialog();
}

void FilteringDlg::SetRedSpin(int aValue)
{
    if ( !mSettingSlider )
    {
	    mRGSpin->setValue( FromGainSlider( aValue ) );
        FromDialog();
    }
}

void FilteringDlg::SetGreenSlider(double aValue)
{
    mSettingSlider = true;
	mGGSlider->setValue( ToGainSlider( mGGSlider, aValue ) );
    mSettingSlider = false;
    FromDialog();
}

void FilteringDlg::SetGreenSpin(int aValue)
{
    if ( !mSettingSlider )
    {
	    mGGSpin->setValue( FromGainSlider( aValue ) );
        FromDialog();
    }
}

void FilteringDlg::SetBlueSlider(double aValue)
{
    mSettingSlider = true;
	mBGSlider->setValue( ToGainSlider( mBGSlider, aValue ) );
    mSettingSlider = false;
    FromDialog();
}

void FilteringDlg::SetBlueSpin(int aValue)
{
    if ( !mSettingSlider )
    {
	    mBGSpin->setValue( FromGainSlider( aValue ) );
        FromDialog();
    }
}

// =============================================================================
void FilteringDlg::InitControls()
{
    mRGSpin->setRange( 0.0, 16.0 );
    mRGSpin->setSingleStep( 0.1 );
    mGGSpin->setRange( 0.0, 16.0 );
    mGGSpin->setSingleStep( 0.1 );
    mBGSpin->setRange( 0.0, 16.0 );
    mBGSpin->setSingleStep( 0.1 );

    mRGSlider->setRange( 0, GAIN_TICK_COUNT );
    mGGSlider->setRange( 0, GAIN_TICK_COUNT );
    mBGSlider->setRange( 0, GAIN_TICK_COUNT );

    mROSpin->setRange( -255, 255 );
    mGOSpin->setRange( -255, 255 );
    mBOSpin->setRange( -255, 255 );

    mROSlider->setRange( -255, 255 );
    mGOSlider->setRange( -255, 255 );
    mBOSlider->setRange( -255, 255 );


    ToDialog();

    mLoading = false;
    mSkipSyncSpinsCount = 0;
    mSkipSyncSlidersCount = 0;

}

// =============================================================================
void FilteringDlg::EnableInterface()
{
	bool lEnabled = mRGBFilterCheckBox->isChecked();

    mRGSpin->setEnabled( lEnabled );
    mGGSpin->setEnabled( lEnabled );
    mBGSpin->setEnabled( lEnabled );
    mROSpin->setEnabled( lEnabled );
    mGOSpin->setEnabled( lEnabled );
    mBOSpin->setEnabled( lEnabled );

    mRGSlider->setEnabled( lEnabled );
    mGGSlider->setEnabled( lEnabled );
    mBGSlider->setEnabled( lEnabled );
    mROSlider->setEnabled( lEnabled );
    mGOSlider->setEnabled( lEnabled );
    mBOSlider->setEnabled( lEnabled );

    mWBButton->setEnabled( lEnabled );
    mResetButton->setEnabled( lEnabled );

    mOffsetsLabel->setEnabled( lEnabled );
    mGainsLabel->setEnabled( lEnabled );
    mRedLabel->setEnabled( lEnabled );
    mGreenLabel->setEnabled( lEnabled );
    mBlueLabel->setEnabled( lEnabled );
}


// =============================================================================
void FilteringDlg::OnBnClickedEnabledcheck()
{
    FromDialog();
    EnableInterface();
}


// =============================================================================
void FilteringDlg::ToDialog()
{
    mLoading = true;

    // Filter RGB enabled?
    mRGBFilterCheckBox->setChecked(mFilterRGBEnabled);

    // Gains
    double lGainR = mFilterRGB.GetGainR();
    mRGSpin->setValue( lGainR );
    mGGSpin->setValue( mFilterRGB.GetGainG() );
    mBGSpin->setValue( mFilterRGB.GetGainB() );

    // Offsets
    mROSpin->setValue( mFilterRGB.GetOffsetR() );
    mGOSpin->setValue( mFilterRGB.GetOffsetG() );
    mBOSpin->setValue( mFilterRGB.GetOffsetB() );

    // Bayer filtering
    switch ( mBayerFilter )
    {
        case PvBayerFilterSimple:
            mBayerCombo->setCurrentIndex( 0 );
            break;

        case PvBayerFilter3X3:
            mBayerCombo->setCurrentIndex( 1 );
            break;

        default:
            assert( 0 );
    }

    mLoading = false;

    // Make sure UI is setup properly!
    EnableInterface();
}


// =============================================================================
void FilteringDlg::FromDialog()
{
    if ( mLoading )
    {
        return;
    }

    // Filter RGB enabled?
    mFilterRGBEnabled = mRGBFilterCheckBox->isChecked();

    // Gain R
    mFilterRGB.SetGainR( mRGSpin->value() );


    // Gain G
    mFilterRGB.SetGainG( mGGSpin->value() );

    // Gain B
    mFilterRGB.SetGainB( mBGSpin->value() );

    // Offset R
    mFilterRGB.SetOffsetR( mROSpin->value() );

    // Offset G
    mFilterRGB.SetOffsetG( mGOSpin->value() );

    // Offset B
    mFilterRGB.SetOffsetB( mBOSpin->value() );

    // Bayer filtering
    int lSelected = mBayerCombo->currentIndex();
    switch ( lSelected )
    {
        case 0:
            mBayerFilter = PvBayerFilterSimple;
            break;

        case 1:
            mBayerFilter = PvBayerFilter3X3;
            break;

        default:
            assert( 0 );
    }
}

// =============================================================================
void FilteringDlg::OnBnClickedWbbutton()
{
    assert( mThreadDisplay != NULL );
    if ( mThreadDisplay == NULL )
    {
        return;
    }

    mThreadDisplay->WhiteBalance( &mFilterRGB );

    ToDialog();
}

// =============================================================================
void FilteringDlg::OnBnClickedResetbutton()
{
    mFilterRGB.Reset();
    ToDialog();
}


// =============================================================================
void FilteringDlg::Save( PvConfigurationWriter &aWriter )
{
    QString lStr;

    // Save a version string, just in case we need it in the future
    aWriter.Store( FILTERINGDLG_VERSION, TAG_VERSION );

    // RGB filter enabled
    aWriter.Store( mFilterRGBEnabled ? VAL_TRUE : VAL_FALSE, TAG_FILTERRGBENABLED );

    // Gains
    aWriter.Store( lStr.setNum(mFilterRGB.GetGainR()).toAscii().data(), TAG_REDGAIN );
    aWriter.Store( lStr.setNum(mFilterRGB.GetGainG()).toAscii().data(), TAG_GREENGAIN );
    aWriter.Store( lStr.setNum(mFilterRGB.GetGainB()).toAscii().data(), TAG_BLUEGAIN );

    // Offsets
    aWriter.Store( lStr.setNum(mFilterRGB.GetOffsetR()).toAscii().data(), TAG_REDOFFSET );
    aWriter.Store( lStr.setNum(mFilterRGB.GetOffsetG()).toAscii().data(), TAG_GREENOFFSET );
    aWriter.Store( lStr.setNum(mFilterRGB.GetOffsetB()).toAscii().data(), TAG_BLUEOFFSET );

    // Bayer filtering
    lStr = VAL_BAYERSIMPLE;
    switch ( mBayerFilter )
    {
        case PvBayerFilterSimple:
            lStr = VAL_BAYERSIMPLE;
            break;

        case PvBayerFilter3X3:
            lStr = VAL_BAYER3X3;
            break;

        default:
            assert( 0 );
    }
    aWriter.Store( lStr.toAscii().data(), TAG_BAYERFILTERING );
}


// =============================================================================
void FilteringDlg::Load( PvConfigurationReader &aReader )
{
    PvResult lResult;
    PvString lPvStr;
    double lValueD = 0.0;
    int lValueI = 0;

    // Always load from a blank setup!
    mFilterRGBEnabled = false;
    mFilterRGB.Reset();
    mBayerFilter = PvBayerFilterSimple;

    lResult = aReader.Restore( TAG_FILTERRGBENABLED, lPvStr );
    if ( lResult.IsOK() )
    {
        mFilterRGBEnabled = ( lPvStr == VAL_TRUE );
    }

    // Gains
    lResult = aReader.Restore( TAG_REDGAIN, lPvStr );
    if ( lResult.IsOK() )
    {
    	mFilterRGB.SetGainR( QString(lPvStr.GetAscii()).toDouble() );
    }
    lResult = aReader.Restore( TAG_GREENGAIN, lPvStr );
    if ( lResult.IsOK() )
    {
        mFilterRGB.SetGainG( QString(lPvStr.GetAscii()).toDouble() );
    }
    lResult = aReader.Restore( TAG_BLUEGAIN, lPvStr );
    if ( lResult.IsOK() )
    {
        mFilterRGB.SetGainB( QString(lPvStr.GetAscii()).toDouble() );
    }

    // Offsets
    lResult = aReader.Restore( TAG_REDOFFSET, lPvStr );
    if ( lResult.IsOK() )
    {
        mFilterRGB.SetOffsetR( QString(lPvStr.GetAscii()).toInt() );
    }
    lResult = aReader.Restore( TAG_GREENOFFSET, lPvStr );
    if ( lResult.IsOK() )
    {
        mFilterRGB.SetOffsetG( QString(lPvStr.GetAscii()).toInt() );
    }
    lResult = aReader.Restore( TAG_BLUEOFFSET, lPvStr );
    if ( lResult.IsOK() )
    {
        mFilterRGB.SetOffsetB( QString(lPvStr.GetAscii()).toInt() );
    }

    // Bayer filtering
    lResult = aReader.Restore( TAG_BAYERFILTERING, lPvStr );
    if ( lResult.IsOK() )
    {
        if ( lPvStr == VAL_BAYERSIMPLE )
        {
            mBayerFilter = PvBayerFilterSimple;
        }
        else if ( lPvStr == VAL_BAYER3X3 )
        {
            mBayerFilter = PvBayerFilter3X3;
        }
    }

    ToDialog();
}

// =============================================================================
void FilteringDlg::ApplyFilterRGB( PvBuffer *aBuffer )
{
    mFilterRGB.Apply( aBuffer );
}

// =============================================================================
double FilteringDlg::FromGainSlider( double aValue )
{
    double lPos = PTMIN( static_cast<double>( GAIN_TICK_COUNT - 1 ), aValue );
    double lLog = log( GAIN_TICK_COUNT - lPos );
    double lValue = GAIN_MAX - lLog / cNormalizer * GAIN_MAX;

    return lValue;
}

// =============================================================================
int FilteringDlg::ToGainSlider( QSlider *aSlider, double &aValue )
{
    double lV1 = ( static_cast<double>( GAIN_MAX ) - aValue ) / static_cast<double>( GAIN_MAX ) * cNormalizer;
    double lV2 = static_cast<double>( GAIN_TICK_COUNT ) - exp( lV1 );

    int lMin = aSlider->minimum();
    int lMax = aSlider->maximum();

    int lValue = PTMAX( lMin, PTMIN( lMax, static_cast<int>( lV2 + 0.5 ) ) );

    return lValue;
}

// =============================================================================
void FilteringDlg::ConfigureConverter( PvBufferConverter &aConverter )
{
    aConverter.SetBayerFilter( GetBayerFilter() );
    if ( IsFilterRGBEnabled() )
    {
        aConverter.SetRGBFilter( mFilterRGB );
    }
    else
    {
        aConverter.ResetRGBFilter();
    }
}


