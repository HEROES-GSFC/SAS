/*
 * displayoptionsdlg.cpp
 *
 *  Created on: Oct 15, 2009
 *      Author: fgobeil
 */

#include "gevplayer.h"
#include "displayoptionsdlg.h"
#include "threaddisplay.h"

#include <PvDisplayWnd.h>

#include <QtGui/QHBoxLayout>
#include <QtGui/QGroupBox>

#include <assert.h>
#include <limits>


#define DISPLAYSETUP_VERSION ( "1.0.0.0" )

#define TAG_VERSION ( "displaysetupversion" )
#define TAG_ACCELERATION ( "displayacceleration" )
#define TAG_VSYNC ( "displayvsync" )
#define TAG_ENABLED ( "displayenabled" )
#define TAG_FPS ( "displayframerate" )
#define TAG_KEEP_PARTIAL ( "keeppartialimages" )


DisplayOptionsDlg::DisplayOptionsDlg( QWidget* aParent )
    : QDialog( aParent )
{
    CreateLayout();
}


DisplayOptionsDlg::~DisplayOptionsDlg()
{
}


void DisplayOptionsDlg::CreateLayout()
{
    mGLDisabled = new QRadioButton( "Disabled" );
    mGLEnabled = new QRadioButton( "Enabled (uses OpenGL)" );
    mGLFull = new QRadioButton( "Full (uses OpenGL and video memory)" );

    QObject::connect( mGLDisabled, SIGNAL( clicked() ), this, SLOT( OnRendererSelected() ) );
    QObject::connect( mGLEnabled, SIGNAL( clicked() ), this, SLOT( OnRendererSelected() ) );
    QObject::connect( mGLFull, SIGNAL( clicked() ), this, SLOT( OnRendererSelected() ) );

    mRenderer = new QLabel( "" );
    mRendererVersion = new QLabel( "" );
    mVSyncCheck = new QCheckBox( "Vertical synchronization" );

    QVBoxLayout *lOpenGLLayout = new QVBoxLayout;
    lOpenGLLayout->addWidget( mGLDisabled );
    lOpenGLLayout->addWidget( mGLEnabled );
    lOpenGLLayout->addWidget( mGLFull );
    lOpenGLLayout->addSpacing( 5 );
    lOpenGLLayout->addWidget( mRenderer );
    lOpenGLLayout->addWidget( mRendererVersion );
    lOpenGLLayout->addSpacing( 5 );
    lOpenGLLayout->addWidget( mVSyncCheck );
    QGroupBox *lOpenGL = new QGroupBox( "Hardware acceleration" );
    lOpenGL->setLayout( lOpenGLLayout );

    mFPSDisabled = new QRadioButton( "Display disabled" );
    mFPS10 = new QRadioButton( "10 frames per second" );
    mFPS30 = new QRadioButton( "30 frames per second" );
    mFPS60 = new QRadioButton( "60 frames per second" );

    QVBoxLayout *lFPSLayout = new QVBoxLayout;
    lFPSLayout->addWidget( mFPSDisabled );
    lFPSLayout->addWidget( mFPS10 );
    lFPSLayout->addWidget( mFPS30 );
    lFPSLayout->addWidget( mFPS60 );
    QGroupBox *lFPS = new QGroupBox( "Maximum display frame rate" );
    lFPS->setLayout( lFPSLayout );


    QVBoxLayout *lPartialImagesLayout = new QVBoxLayout;
    QGroupBox *lPartialImagesGroup = new QGroupBox( "Partial images behaviour" );
    mKeepPartialImages = new QCheckBox( "Keep partial images" );
    lPartialImagesLayout->addWidget( mKeepPartialImages );
    lPartialImagesGroup->setLayout( lPartialImagesLayout );



    QHBoxLayout *lButtons = new QHBoxLayout;
    mOKButton = new QPushButton( tr( "OK" ) );
    mCancelButton = new QPushButton( tr( "Cancel" ) );
    lButtons->addStretch();
    lButtons->addWidget( mOKButton );
    lButtons->addWidget( mCancelButton );

    QObject::connect( mOKButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
    QObject::connect( mCancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );

    QVBoxLayout *lMain = new QVBoxLayout;
    lMain->addWidget( lOpenGL );
    lMain->addWidget( lFPS );
    lMain->addWidget( lPartialImagesGroup );
    lMain->addStretch();
    lMain->addLayout( lButtons );

    setLayout( lMain );
    setFixedSize( 400, 440 );

    Qt::WindowFlags lFlags = windowFlags();
    lFlags |= Qt::CustomizeWindowHint;
    lFlags &= ~Qt::WindowContextHelpButtonHint;
    lFlags &= ~Qt::WindowSystemMenuHint;

    lFlags |= Qt::WindowStaysOnTopHint;
    setWindowFlags( lFlags );

    setWindowTitle( tr( "Display Options" ) );

    // Set the defaults appropriately
    mFPS30->setChecked( true );

    PvRendererMode lSupported = PvDisplayWnd::GetHighestRendererMode();

    // Hardware acceleration
    mGLDisabled->setEnabled( lSupported >= PvRendererModeDefault );
    mGLEnabled->setEnabled( lSupported >= PvRendererModeAccelerated );
    mGLFull->setEnabled( lSupported >= PvRendererModeHardwareAccelerated );
    mRenderer->setText( QString( "Renderer: " ) + PvDisplayWnd::GetRenderer().GetAscii() );
    mRendererVersion->setText( QString( "Version: " ) + PvDisplayWnd::GetRendererVersion().GetAscii() );

    mGLDisabled->setChecked( true );
    mVSyncCheck->setChecked( false ); // VSync not supported for Non-OpenGL
    mVSyncCheck->setDisabled( true );
}


void DisplayOptionsDlg::OnRendererSelected()
{
    if ( mGLEnabled->isChecked() || mGLFull->isChecked() )
    {
        mVSyncCheck->setDisabled( false );
    }
    else
    {
        assert( mGLDisabled->isChecked() );
        mVSyncCheck->setChecked( false ); // VSync not supported for Non-OpenGL
        mVSyncCheck->setDisabled( true );
    }
}


void DisplayOptionsDlg::Init( ThreadDisplay *aThreadDisplay )
{
    // Frames per seconds
    switch ( aThreadDisplay->GetMaxFPS() )
    {
    case 10:
        mFPS10->setChecked( true );
        break;

    default:
        assert( 0 );
        // Fallthrough

    case 30:
        mFPS30->setChecked( true );
        break;

    case 60:
        mFPS60->setChecked( true );
        break;

    case -1:
        mFPSDisabled->setChecked( true );
        break;
    }

    PvRendererMode lSupported = PvDisplayWnd::GetHighestRendererMode();

    // Hardware acceleration
    mGLDisabled->setEnabled( lSupported >= PvRendererModeDefault );
    mGLEnabled->setEnabled( lSupported >= PvRendererModeAccelerated );
    mGLFull->setEnabled( lSupported >= PvRendererModeHardwareAccelerated );
    mRenderer->setText( QString( "Renderer: " ) + PvDisplayWnd::GetRenderer().GetAscii() );
    mRendererVersion->setText( QString( "Version: " ) + PvDisplayWnd::GetRendererVersion().GetAscii() );
    switch ( aThreadDisplay->GetRendererMode() )
    {
    default:
        assert( 0 );
        // Fallthrough

    case PvRendererModeDefault:
        mGLDisabled->setChecked( true );
        mVSyncCheck->setChecked( false ); // VSync not supported for Non-OpenGL
        mVSyncCheck->setDisabled( true );
        break;

    case PvRendererModeAccelerated:
        mGLEnabled->setChecked( true );
        mVSyncCheck->setDisabled( false );
        mVSyncCheck->setChecked( aThreadDisplay->GetVSyncEnabled() );
        break;

    case PvRendererModeHardwareAccelerated:
        mGLFull->setChecked( true );
        mVSyncCheck->setDisabled( false );
        mVSyncCheck->setChecked( aThreadDisplay->GetVSyncEnabled() );
        break;
    }

    mKeepPartialImages->setChecked( aThreadDisplay->GetKeepPartialImages() );
}


void DisplayOptionsDlg::Apply( ThreadDisplay *aThreadDisplay )
{
    // Frames per seconds
    if ( mFPSDisabled->isChecked() )
    {
        aThreadDisplay->SetMaxFPS( -1 ); // Disabled
    }
    else if ( mFPS10->isChecked() )
    {
        aThreadDisplay->SetMaxFPS( 10 );
    }
    else if ( mFPS60->isChecked() )
    {
        aThreadDisplay->SetMaxFPS( 60 );
    }
    else
    {
        assert( mFPS30->isChecked() );
        aThreadDisplay->SetMaxFPS( 30 );
    }

    // Hardware acceleration
    if ( mGLEnabled->isChecked() )
    {
        aThreadDisplay->SetRendererMode( PvRendererModeAccelerated );
    }
    else if ( mGLFull->isChecked() )
    {
        aThreadDisplay->SetRendererMode( PvRendererModeHardwareAccelerated );
    }
    else
    {
        assert( mGLDisabled->isChecked() );
        aThreadDisplay->SetRendererMode( PvRendererModeDefault );
    }

    // VSync
    aThreadDisplay->SetVSyncEnabled( mVSyncCheck->checkState() == Qt::Checked );

    aThreadDisplay->SetKeepPartialImages( mKeepPartialImages->checkState() == Qt::Checked );
}


///
/// \brief Saves display configuration to file
///

void DisplayOptionsDlg::Save( PvConfigurationWriter& aWriter )
{
    QString lStr;

    aWriter.Store(DISPLAYSETUP_VERSION, TAG_VERSION );

    // Frames per seconds
    QString lFPS;
    if ( mFPSDisabled->isChecked() )
    {
        lFPS = "-1"; // Disabled
    }
    else if ( mFPS10->isChecked() )
    {
        lFPS = "10";
    }
    else if ( mFPS60->isChecked() )
    {
        lFPS = "60";
    }
    else
    {
        assert( mFPS30->isChecked() );
        lFPS = "30";
    }
    aWriter.Store( lFPS.toAscii().data(), TAG_FPS );


    // Hardware acceleration
    PvRendererMode lRenderMode;
    if ( mGLEnabled->isChecked() )
    {
        lRenderMode = PvRendererModeAccelerated;
    }
    else if ( mGLFull->isChecked() )
    {
        lRenderMode = PvRendererModeHardwareAccelerated;
    }
    else
    {
        assert( mGLDisabled->isChecked() );
        lRenderMode = PvRendererModeDefault;
    }

    aWriter.Store( lStr.setNum( lRenderMode ).toAscii().data(), TAG_ACCELERATION );
    aWriter.Store( ( mVSyncCheck->checkState() == Qt::Checked ) ? "1" : "0", TAG_VSYNC );
    aWriter.Store( ( mKeepPartialImages->checkState() == Qt::Checked ) ? "1" : "0", TAG_KEEP_PARTIAL );
}


///
/// \brief Retrieves display configuration from file
///

void DisplayOptionsDlg::Load( PvConfigurationReader& aReader )
{
    PvString lStr;

    bool lVSyncEnabled = false;
    if ( aReader.Restore( TAG_VSYNC, lStr ).IsOK() )
    {
        if ( strcmp( lStr.GetAscii(), "1" ) == 0 )
        {
            lVSyncEnabled = true;
        }
    }

    if ( aReader.Restore( TAG_KEEP_PARTIAL, lStr ).IsOK() )
    {
        if ( strcmp( lStr.GetAscii(), "1" ) == 0 )
        {
            mKeepPartialImages->setChecked( true );
        }
        else
        {
            mKeepPartialImages->setChecked( false );
        }
    }

    if ( aReader.Restore( TAG_ACCELERATION, lStr ).IsOK() )
    {
        int lAcceleration = atoi( lStr.GetAscii() );
        switch ( lAcceleration )
        {
        default:
            assert( 0 );
            // Fallthrough

        case PvRendererModeDefault:
            mGLDisabled->setChecked( true );
            mVSyncCheck->setChecked( false ); // VSync not supported for Non-OpenGL
            mVSyncCheck->setDisabled( true );
            break;

        case PvRendererModeAccelerated:
            mGLEnabled->setChecked( true );
            mVSyncCheck->setDisabled( false );
            mVSyncCheck->setChecked( lVSyncEnabled );
            break;

        case PvRendererModeHardwareAccelerated:
            mGLFull->setChecked( true );
            mVSyncCheck->setDisabled( false );
            mVSyncCheck->setChecked( lVSyncEnabled );
            break;
        }
    }



    if ( aReader.Restore( TAG_FPS, lStr ).IsOK() )
    {
        int lFPS = atoi( lStr.GetAscii() );
        switch ( lFPS )
        {
        case 10:
            mFPS10->setChecked( true );
            break;

        default:
            assert( 0 );
            // Fallthrough

        case 30:
            mFPS30->setChecked( true );
            break;

        case 60:
            mFPS60->setChecked( true );
            break;

        case -1:
            mFPSDisabled->setChecked( true );
            break;
        }
    }
}


