// *****************************************************************************
//
//     Copyright (c) 2009, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#include "setupdlg.h"

#include <QtGui/QGridLayout>
#include <QtGui/QVBoxLayout>
#include <QtGui/QMessageBox>

#include <assert.h>
#include <cstdio>


#define LAYOUT_IDENT ( 20 )

#define SETUPDLG_VERSION ( "1.0.0.0" )

#define TAG_VERSION ( "setupversion" )
#define TAG_ROLE ( "setuprole" )
#define TAG_DEFAULTCHANNEL ( "setupdefaultchannel" )
#define TAG_DESTINATION ( "setupdestination" )
#define TAG_UNICASTSPECIFICPORT ( "setupunicastspecificport" )
#define TAG_UNICASTIP ( "setupunicastip" )
#define TAG_UNICASTPORT ( "setupunicastport" )
#define TAG_MULTICASTIP ( "setupmulticastip" )
#define TAG_MULTICASTPORT ( "setupmulticastport" )

#define VAL_ROLECTRLDATA ( "ctrldata" )
#define VAL_ROLECTRL ( "ctrl" )
#define VAL_ROLEDATA ( "data" )
#define VAL_ROLEMONITOR ( "monitor" )
#define VAL_DESTINATIONUNICASTAUTO ( "unicastauto" )
#define VAL_DESTINATIONUNICASTSPECIFIC ( "unicastspecific" )
#define VAL_DESTINATIONUNICASTOTHER ( "unicastother" )
#define VAL_DESTINATIONMULTICAST ( "multicast" )


///
/// \brief Constructor
///

SetupDlg::SetupDlg( QWidget* aParent )
    : QDialog( aParent )
    , mEnabled( true )
{
    CreateLayout();
}


///
/// \brief Destructor
///

SetupDlg::~SetupDlg()
{
}


///
/// \brief Creates the form's layout
///

void
SetupDlg::CreateLayout()
{
    setWindowTitle( tr( "Setup" ) );
    mOKButton = new QPushButton( tr( "OK" ) );
    mCancelButton = new QPushButton( tr( "Cancel" ) );

    QObject::connect( mOKButton, SIGNAL( clicked() ), this, SLOT( accept() ) );
    QObject::connect( mCancelButton, SIGNAL( clicked() ), this, SLOT( reject() ) );

    QGroupBox *lRole = CreateRole();
    QGroupBox *lDefaultChannel = CreateDefaultChannel();
    QGroupBox *lDestination = CreateDestination();

    QVBoxLayout *lLeft = new QVBoxLayout;
    lLeft->addWidget( lRole );
    lLeft->addWidget( lDefaultChannel );
    lLeft->addStretch();

    QHBoxLayout *lGroups = new QHBoxLayout;
    lGroups->addLayout( lLeft );
    lGroups->addWidget( lDestination );

    QHBoxLayout *lButtons = new QHBoxLayout;
    lButtons->addStretch();
    lButtons->addWidget( mOKButton );
    lButtons->addWidget( mCancelButton );

    QVBoxLayout *lLayout = new QVBoxLayout;
    lLayout->addLayout( lGroups );
    lLayout->addStretch();
    lLayout->addLayout( lButtons );

    setLayout( lLayout );
    setFixedSize( 530, 420 );
    
    Qt::WindowFlags lFlags = windowFlags();
    lFlags &= ~Qt::WindowContextHelpButtonHint;
    lFlags &= ~Qt::WindowSystemMenuHint;
    setWindowFlags( lFlags );
}


///
/// \brief Create role group box
///

QGroupBox *
SetupDlg::CreateRole()
{
    QGroupBox *lBox = new QGroupBox( tr( "GEVPlayer Role" ) );

    mCtrlDataRadio = new QRadioButton( tr( "Controller and data receiver" ) );
    mCtrlRadio = new QRadioButton( tr( "Controller" ) );
    mDataRadio = new QRadioButton( tr( "Data receiver" ) );
    mMonitorRadio = new QRadioButton( tr( "Monitor" ) );

    QObject::connect( mCtrlDataRadio, SIGNAL( clicked() ), this, SLOT( OnRadioClicked() ) );
    QObject::connect( mCtrlRadio, SIGNAL( clicked() ), this, SLOT( OnRadioClicked() ) );
    QObject::connect( mDataRadio, SIGNAL( clicked() ), this, SLOT( OnRadioClicked() ) );
    QObject::connect( mMonitorRadio, SIGNAL( clicked() ), this, SLOT( OnRadioClicked() ) );

    QVBoxLayout *lLayout = new QVBoxLayout;
    lLayout->addWidget( mCtrlDataRadio );
    lLayout->addWidget( mCtrlRadio );
    lLayout->addWidget( mDataRadio );
    lLayout->addWidget( mMonitorRadio );
    lLayout->addStretch();

    lBox->setLayout( lLayout );

    return lBox;
}



///
/// \brief Create default channel group
///

QGroupBox *
SetupDlg::CreateDefaultChannel()
{
    QGroupBox *lBox = new QGroupBox( tr( "Default Channel" ) );

    mDefaultChannelLabel = new QLabel( tr( "Channel" ) );
    mDefaultChannelEdit = new QLineEdit( tr( "0" ) );

    QHBoxLayout *lDefaultChannelLayout = new QHBoxLayout;
    lDefaultChannelLayout->addWidget( mDefaultChannelLabel );
    lDefaultChannelLayout->addStretch();
    lDefaultChannelLayout->addWidget( mDefaultChannelEdit );

    QVBoxLayout *lVLayout = new QVBoxLayout;
    lVLayout->addLayout( lDefaultChannelLayout );
    lVLayout->addStretch();

    lBox->setLayout( lVLayout );

    return lBox;
}


///
/// \brief Create destination group box
///

QGroupBox *
SetupDlg::CreateDestination()
{
    QGroupBox *lBox = new QGroupBox( tr( "Stream Destination" ) );

    mUnicastAutoRadio = new QRadioButton( tr( "Unicast, automatic" ) );
    mUnicastSpecificRadio = new QRadioButton( tr( "Unicast, specific local port" ) );
    mUnicastOtherRadio = new QRadioButton( tr( "Unicast, other destination" ) );
    mUnicastSpecificPortLabel = new QLabel( tr( "Port" ) );
    mUnicastSpecificPortEdit = new QLineEdit( tr( "0" ) );
    mUnicastIPLabel = new QLabel( tr( "IP address" ) );
    mUnicastIPCtrl = new QLineEdit( tr( "0.0.0.0" ) );
    mUnicastPortLabel = new QLabel( tr( "Port" ) );
    mUnicastPortEdit = new QLineEdit( tr( "0" ) );
    mMulticastRadio = new QRadioButton( tr( "Multicast" ) );
    mMulticastIPLabel = new QLabel( tr( "IP address " ) );
    mMulticastIPCtrl = new QLineEdit( tr( "0.0.0.0" ) );
    mMulticastPortLabel = new QLabel( tr( "Port" ) );
    mMulticastPortEdit = new QLineEdit( tr( "0" ) );

    QObject::connect( mUnicastAutoRadio, SIGNAL( clicked() ), this, SLOT( OnRadioClicked() ) );
    QObject::connect( mUnicastSpecificRadio, SIGNAL( clicked() ), this, SLOT( OnRadioClicked() ) );
    QObject::connect( mUnicastOtherRadio, SIGNAL( clicked() ), this, SLOT( OnRadioClicked() ) );
    QObject::connect( mMulticastRadio, SIGNAL( clicked() ), this, SLOT( OnRadioClicked() ) );

    QHBoxLayout *lUnicastSpecificPort = new QHBoxLayout;
    lUnicastSpecificPort->addSpacing( LAYOUT_IDENT );
    lUnicastSpecificPort->addWidget( mUnicastSpecificPortLabel );
    lUnicastSpecificPort->addStretch();
    lUnicastSpecificPort->addWidget( mUnicastSpecificPortEdit );

	QHBoxLayout *lUnicastIP = new QHBoxLayout;
    lUnicastIP->addSpacing( LAYOUT_IDENT );
    lUnicastIP->addWidget( mUnicastIPLabel );
    lUnicastIP->addStretch();
    lUnicastIP->addWidget( mUnicastIPCtrl );

    QHBoxLayout *lUnicastPort = new QHBoxLayout;
    lUnicastPort->addSpacing( LAYOUT_IDENT );
    lUnicastPort->addWidget( mUnicastPortLabel );
    lUnicastPort->addStretch();
    lUnicastPort->addWidget( mUnicastPortEdit );

    QHBoxLayout *lMulticastIP = new QHBoxLayout;
    lMulticastIP->addSpacing( LAYOUT_IDENT );
    lMulticastIP->addWidget( mMulticastIPLabel );
    lMulticastIP->addStretch();
    lMulticastIP->addWidget( mMulticastIPCtrl );

    QHBoxLayout *lMulticastPort = new QHBoxLayout;
    lMulticastPort->addSpacing( LAYOUT_IDENT );
    lMulticastPort->addWidget( mMulticastPortLabel );
    lMulticastPort->addStretch();
    lMulticastPort->addWidget( mMulticastPortEdit );

    QVBoxLayout *lLayout = new QVBoxLayout;
    lLayout->addWidget( mUnicastAutoRadio );
    lLayout->addWidget( mUnicastSpecificRadio );
    lLayout->addLayout( lUnicastSpecificPort );
    lLayout->addWidget( mUnicastOtherRadio );
    lLayout->addLayout( lUnicastIP );
    lLayout->addLayout( lUnicastPort );
    lLayout->addWidget( mMulticastRadio );
    lLayout->addLayout( lMulticastIP );
    lLayout->addLayout( lMulticastPort );
    lLayout->addStretch();

    lBox->setLayout( lLayout );

    return lBox;
}


///
/// \brief Start thread, call base class
///

int SetupDlg::exec()
{
    LoadToUI();
    EnableInterface();

    return QDialog::exec();
}


///
/// \brief Loads data to the UI
///

void SetupDlg::LoadToUI()
{
    QString lStr;

    // Role
    switch ( mSetup.mRole )
    {
        case RoleCtrlData:
            mCtrlDataRadio->setChecked( true );
            break;

        case RoleCtrl:
            mCtrlRadio->setChecked( true );
            break;

        case RoleData:
            mDataRadio->setChecked( true );
            break;

        case RoleMonitor:
            mMonitorRadio->setChecked( true );
            break;

        default:
            assert( 0 );
    }

    // Default channel
    lStr.sprintf( "%i", mSetup.mDefaultChannel );
    mDefaultChannelEdit->setText( lStr );

    // Destination
    switch ( mSetup.mDestination )
    {
        case DestinationUnicastAuto:
            mUnicastAutoRadio->setChecked( true );
            break;

        case DestinationUnicastSpecific:
            mUnicastSpecificRadio->setChecked( true );
            break;

        case DestinationUnicastOther:
            mUnicastOtherRadio->setChecked( true );
            break;

        case DestinationMulticast:
            mMulticastRadio->setChecked( true );
            break;

        default:
            assert( 0 );
    }

    // Port
    lStr.sprintf( "%i", mSetup.mUnicastSpecificPort );
    mUnicastSpecificPortEdit->setText( lStr );

    // IP
    IPStrToCtrl( mSetup.mUnicastIP, mUnicastIPCtrl );

    // Port
    lStr.sprintf( "%i", mSetup.mUnicastPort );
    mUnicastPortEdit->setText( lStr );

    // Multicast IP
    IPStrToCtrl( mSetup.mMulticastIP, mMulticastIPCtrl );

    // Multicast port
    lStr.sprintf( "%i", mSetup.mMulticastPort );
    mMulticastPortEdit->setText( lStr );

    EnableInterface();
}


///
/// \brief Sets a string IP address into an edit control
///

void SetupDlg::IPStrToCtrl( const QString &aIPStr, QLineEdit *aCtrl )
{
    aCtrl->setText( aIPStr );
}


///
/// \brief Sets the enabled state of the form's UI
///

void SetupDlg::EnableInterface()
{
    if ( !mEnabled )
    {
        // Master disable, not touching the state of the UI
        mCtrlDataRadio->setEnabled( false );
        mCtrlRadio->setEnabled( false );
        mDataRadio->setEnabled( false );
        mMonitorRadio->setEnabled( false );
        mDefaultChannelLabel->setEnabled( false );
        mDefaultChannelEdit->setEnabled( false );
        mUnicastAutoRadio->setEnabled( false );
        mUnicastSpecificRadio->setEnabled( false );
        mUnicastOtherRadio->setEnabled( false );
        mMulticastRadio->setEnabled( false );
        mUnicastSpecificPortEdit->setEnabled( false );
        mUnicastSpecificPortLabel->setEnabled( false );
        mUnicastIPCtrl->setEnabled( false );
        mUnicastPortEdit->setEnabled( false );
        mUnicastIPLabel->setEnabled( false );
        mUnicastPortLabel->setEnabled( false );
        mMulticastIPCtrl->setEnabled( false );
        mMulticastPortEdit->setEnabled( false );
        mMulticastIPLabel->setEnabled( false );
        mMulticastPortLabel->setEnabled( false );

        return;
    }

    mCtrlDataRadio->setEnabled( true );
	mCtrlRadio->setEnabled( true );
	mDataRadio->setEnabled( true );
	mMonitorRadio->setEnabled( true );

    bool lCtrlData = mCtrlDataRadio->isChecked();
    bool lCtrl = mCtrlRadio->isChecked();
    bool lData = mDataRadio->isChecked();

    // Default channel
    mDefaultChannelLabel->setEnabled( lCtrlData || lData );
    mDefaultChannelEdit->setEnabled( lCtrlData || lData );

    // Stream radio buttons
    mUnicastAutoRadio->setEnabled( ( lCtrlData || lData ) && mEnabled );
    mUnicastSpecificRadio->setEnabled( ( lCtrlData || lData ) && mEnabled );
    mUnicastOtherRadio->setEnabled( lCtrl && mEnabled );
    mMulticastRadio->setEnabled( ( lCtrl || lData || lCtrlData ) && mEnabled );

    // Make sure we check the first enabled option (if needed)
    bool lValid = false;
    lValid |= mUnicastAutoRadio->isChecked() && mUnicastAutoRadio->isEnabled();
    lValid |= mUnicastSpecificRadio->isChecked() && mUnicastSpecificRadio->isEnabled();
    lValid |= mUnicastOtherRadio->isChecked() && mUnicastOtherRadio->isEnabled();
    lValid |= mMulticastRadio->isChecked() && mMulticastRadio->isEnabled();
    if ( !lValid )
    {
        if ( mUnicastAutoRadio->isEnabled() )
        {
            mUnicastAutoRadio->setChecked( true );
        }
        else if ( mUnicastSpecificRadio->isEnabled() )
        {
            mUnicastSpecificRadio->setChecked( true );
        }
        else if ( mUnicastOtherRadio->isEnabled() )
        {
            mUnicastOtherRadio->setChecked( true );
        }
        else if ( mMulticastRadio->isEnabled() )
        {
            mMulticastRadio->setChecked( true );
        }
    }

    // Unicast specific port
    mUnicastSpecificPortEdit->setEnabled( mUnicastSpecificRadio->isChecked() && mUnicastSpecificRadio->isEnabled() && mEnabled );
    mUnicastSpecificPortLabel->setEnabled( mUnicastSpecificPortEdit->isEnabled() );

	// Unicast other IP/port
    mUnicastIPCtrl->setEnabled( mUnicastOtherRadio->isChecked() && mUnicastOtherRadio->isEnabled() && mEnabled );
    mUnicastPortEdit->setEnabled( mUnicastIPCtrl->isEnabled() );
    mUnicastIPLabel->setEnabled( mUnicastIPCtrl->isEnabled() );
    mUnicastPortLabel->setEnabled( mUnicastIPCtrl->isEnabled() );

    // Multicast IP/port
    mMulticastIPCtrl->setEnabled( mMulticastRadio->isChecked() && mMulticastRadio->isEnabled() && mEnabled );
    mMulticastPortEdit->setEnabled( mMulticastIPCtrl->isEnabled() );
    mMulticastIPLabel->setEnabled( mMulticastIPCtrl->isEnabled() );
    mMulticastPortLabel->setEnabled( mMulticastIPCtrl->isEnabled() );
}


///
/// \brief Form accept handler: moves from UI to data
///

void SetupDlg::accept()
{
    if ( mEnabled )
    {
        QString lStr;
        int lValue = 0;
        int lCount = 0;

        mSetup.mRole = RoleInvalid;
        mSetup.mDestination = DestinationInvalid;

        if ( mCtrlDataRadio->isChecked() )
        {
            mSetup.mRole = RoleCtrlData;
        }
        else if ( mCtrlRadio->isChecked() )
        {
            mSetup.mRole = RoleCtrl;
        }
        else if ( mDataRadio->isChecked() )
        {
            mSetup.mRole = RoleData;
        }
        else if ( mMonitorRadio->isChecked() )
        {
            mSetup.mRole = RoleMonitor;
        }
        else
        {
            assert( 0 );
        }

        lStr = mDefaultChannelEdit->text();
        lCount = sscanf( lStr.toAscii(), "%d", &lValue );
        if ( lCount != 1 )
        {
            QMessageBox::warning( this, "GEVPlayer", "Error parsing default channel." );
        	return;
        }
        if ( ( lValue < 0 ) || ( lValue > 511 ) )
        {
        	QMessageBox::warning( this, "GEVPlayer", "Default channel out of range. It must be between 0 and 511 inclusively." );
        	return;
        }
        mSetup.mDefaultChannel = static_cast<unsigned short>( lValue );

        if ( mUnicastAutoRadio->isChecked() )
        {
            mSetup.mDestination = DestinationUnicastAuto;
        }
        else if ( mUnicastSpecificRadio->isChecked() )
        {
            mSetup.mDestination = DestinationUnicastSpecific;
        }
        else if ( mUnicastOtherRadio->isChecked() )
        {
            mSetup.mDestination = DestinationUnicastOther;
        }
        else if ( mMulticastRadio->isChecked() )
        {
            mSetup.mDestination = DestinationMulticast;
        }
        else
        {
            assert( 0 );
        }

        lStr = mUnicastSpecificPortEdit->text();
        lCount = sscanf( lStr.toAscii(), "%d", &lValue );
        if ( lCount != 1 )
        {
            QMessageBox::warning( this, "GEVPlayer", "Error unicast specific port." );
        	return;
        }
        mSetup.mUnicastSpecificPort = static_cast<unsigned short>( lValue );

		mSetup.mUnicastIP = mUnicastIPCtrl->text();

        lStr = mUnicastPortEdit->text();
        lCount = sscanf( lStr.toAscii(), "%d", &lValue );
        if ( lCount != 1 )
        {
            QMessageBox::warning( this, "GEVPlayer", "Error parsing unicast port." );
        	return;
        }
        mSetup.mUnicastPort = static_cast<unsigned short>( lValue );

        mSetup.mMulticastIP = mMulticastIPCtrl->text();

        lStr = mMulticastPortEdit->text();
        lCount = sscanf( lStr.toAscii(), "%d", &lValue );
        if ( lCount != 1 )
        {
            QMessageBox::warning( this, "GEVPlayer", "Error parsing multicast port." );
        	return;
        }
        mSetup.mMulticastPort = static_cast<unsigned short>( lValue );
    }

    QDialog::accept();
}


///
/// \brief Closes the form without saving the data.
///

void SetupDlg::reject()
{
    QDialog::reject();
}


///
/// \brief Saves the data to a persistence file
///

void SetupDlg::Save( PvConfigurationWriter &aWriter )
{
    Save( aWriter, mSetup );
}


///
/// \brief Saves the data to a persistence file
///

void SetupDlg::Save( PvConfigurationWriter &aWriter, Setup &aSetup )
{
    QString lStr;

    // Save a version string, just in case we need it in the future
    aWriter.Store( SETUPDLG_VERSION, TAG_VERSION );

    // Role mRole;
    lStr = VAL_ROLECTRLDATA;
    switch ( aSetup.mRole )
    {
        case RoleCtrlData:
            lStr = VAL_ROLECTRLDATA;
            break;

        case RoleCtrl:
            lStr = VAL_ROLECTRL;
            break;

        case RoleData:
            lStr = VAL_ROLEDATA;
            break;

        case RoleMonitor:
            lStr = VAL_ROLEMONITOR;
            break;

        default:
            assert( 0 );
    }
    aWriter.Store( lStr.toAscii().data(), TAG_ROLE );

    // Default channel
	lStr.sprintf( "%i" , aSetup.mDefaultChannel );
	aWriter.Store( lStr.toAscii().data(), TAG_DEFAULTCHANNEL );

    // Destination mDestination;
    lStr = VAL_DESTINATIONUNICASTAUTO;
    switch ( aSetup.mDestination )
    {
        case DestinationUnicastAuto:
            lStr = VAL_DESTINATIONUNICASTAUTO;
            break;

        case DestinationUnicastSpecific:
            lStr = VAL_DESTINATIONUNICASTSPECIFIC;
            break;

        case DestinationUnicastOther:
            lStr = VAL_DESTINATIONUNICASTOTHER;
            break;

        case DestinationMulticast:
            lStr = VAL_DESTINATIONMULTICAST;
            break;

        default:
            assert( 0 );
    }
    aWriter.Store( lStr.toAscii().data(), TAG_DESTINATION );

	// unsigned short mUnicastSpecificPort;
	lStr.sprintf( "%i" , aSetup.mUnicastSpecificPort );
	aWriter.Store( lStr.toAscii().data(), TAG_UNICASTSPECIFICPORT );

	// QString mUnicastIP;
	aWriter.Store( aSetup.mUnicastIP.toAscii().data(), TAG_UNICASTIP );

	// unsigned short mUnicastPort;
	lStr.sprintf( "%i" , aSetup.mUnicastPort );
	aWriter.Store( lStr.toAscii().data(), TAG_UNICASTPORT );

	// QString mMulticastIP;
	aWriter.Store( aSetup.mMulticastIP.toAscii().data(), TAG_MULTICASTIP );

	// unsigned short mMulticastPort;
	lStr.sprintf( "%i", aSetup.mMulticastPort );
	aWriter.Store( lStr.toAscii().data(), TAG_MULTICASTPORT );
}


///
/// \brief Loads data from a persistence file
///

void SetupDlg::Load( PvConfigurationReader &aReader )
{
    Load( aReader, mSetup );
}


///
/// \brief Loads data from a persistence file
///

void SetupDlg::Load( PvConfigurationReader &aReader, Setup &aSetup )
{
    PvResult lResult;
    PvString lPvStr;

    // Always load from a blank setup!
    aSetup.Reset();

    // Role mRole;
    PvString lRole(TAG_ROLE);
    lResult = aReader.Restore( lRole, lPvStr );
    if ( lResult.IsOK() )
    {
        StrToRole( lPvStr.GetAscii(), aSetup.mRole );
    }

    // PvUInt16 mDefaultCannel
    PvString lDefaultChannel( TAG_DEFAULTCHANNEL );
    lResult = aReader.Restore( lDefaultChannel, lPvStr );
    if ( lResult.IsOK() )
    {
    	int lValue = 0;
    	sscanf( lPvStr.GetAscii(), "%i", &lValue );
    	aSetup.mDefaultChannel = static_cast<unsigned short>( lValue );
    }

    // unsigned short mUnicastSpecificPort;
    PvString lUnicastSpecificPort( TAG_UNICASTSPECIFICPORT );
    lResult = aReader.Restore( lUnicastSpecificPort, lPvStr );
    if ( lResult.IsOK() )
    {
        int lPort;
        sscanf( lPvStr.GetAscii(), "%i", &lPort );
        aSetup.mUnicastSpecificPort = static_cast<unsigned short>( lPort );
    }

    // Destination mDestination;
    PvString lDestination(TAG_DESTINATION);
    lResult = aReader.Restore( lDestination, lPvStr );
    if ( lResult.IsOK() )
    {
        StrToDestination( lPvStr.GetAscii(), aSetup.mDestination );
    }

    // QString mUnicastIP;
    PvString lUnicastIP( TAG_UNICASTIP );
    lResult = aReader.Restore( lUnicastIP, lPvStr );
    if ( lResult.IsOK() )
    {
        aSetup.mUnicastIP = lPvStr.GetAscii();
    }

    // unsigned short mUnicastPort;
    PvString lUnicastPort( TAG_UNICASTPORT );
    lResult = aReader.Restore( lUnicastPort, lPvStr );
    if ( lResult.IsOK() )
    {
        int lPort;
        sscanf( lPvStr.GetAscii(), "%i", &lPort );
        aSetup.mUnicastPort = static_cast<unsigned short>( lPort );
    }

    // QString mMulticastIP;
    PvString lMulticastIP( TAG_MULTICASTIP );
    lResult = aReader.Restore( lMulticastIP, lPvStr );
    if ( lResult.IsOK() )
    {
        aSetup.mMulticastIP = lPvStr.GetAscii();
    }

    // unsigned short mMulticastPort;
    PvString lMulticastPort( TAG_MULTICASTPORT );
    lResult = aReader.Restore( lMulticastPort, lPvStr );
    if ( lResult.IsOK() )
    {
        int lPort;
        sscanf( lPvStr.GetAscii(), "%i", &lPort );
        aSetup.mMulticastPort = static_cast<unsigned short>( lPort );
    }

}


///
/// \brief Checks if the data is the same as what is in the persistence file
///

bool SetupDlg::IsTheSame( PvConfigurationReader &aReader )
{
    // Load a local setup
    Setup lSetup;
    Load( aReader, lSetup );

    // Start with the assumption that they are the same
    bool lSame = true;

    // Try to invalidate assumption with destination, role
    lSame &= lSetup.mDestination == mSetup.mDestination;
    lSame &= lSetup.mRole == mSetup.mRole;

    // Default channel
    lSame &= lSetup.mDefaultChannel == mSetup.mDefaultChannel;

    // Only if destination is unicast specific, compare port
    if ( lSetup.mDestination == DestinationUnicastSpecific )
    {
        lSame &= lSetup.mUnicastSpecificPort == mSetup.mUnicastSpecificPort;
    }

	// Only if destination is unicast other, compare destination
    if ( lSetup.mDestination == DestinationUnicastOther )
    {
        lSame &= lSetup.mUnicastIP == mSetup.mUnicastIP;
        lSame &= lSetup.mUnicastPort == mSetup.mUnicastPort;
    }

    // Only if destination is multicast, compare destination
    if ( lSetup.mDestination == DestinationMulticast )
    {
        lSame &= lSetup.mMulticastIP == mSetup.mMulticastIP;
        lSame &= lSetup.mMulticastPort == mSetup.mMulticastPort;
    }

    // Return conclusion!
    return lSame;
}


///
/// \brief Converts a string to a role enum
///

void SetupDlg::StrToRole( const QString &aStr, Role &aRole )
{
    if ( aStr == VAL_ROLECTRLDATA )
    {
        aRole = RoleCtrlData;
    }
    else if ( aStr == VAL_ROLEDATA )
    {
        aRole = RoleData;
    }
    else if ( aStr == VAL_ROLECTRL )
    {
        aRole = RoleCtrl;
    }
    else if ( aStr == VAL_ROLEMONITOR )
    {
        aRole = RoleMonitor;
    }
}


///
/// \brief Converts a string to destination
///

void SetupDlg::StrToDestination( const QString &aStr, Destination &aDestination )
{
    if ( aStr == VAL_DESTINATIONUNICASTAUTO )
    {
        aDestination = DestinationUnicastAuto;
    }
    else if ( aStr == VAL_DESTINATIONUNICASTSPECIFIC )
    {
        aDestination = DestinationUnicastSpecific;
    }
    else if ( aStr == VAL_DESTINATIONUNICASTOTHER )
    {
        aDestination = DestinationUnicastOther;
    }
    else if ( aStr == VAL_DESTINATIONMULTICAST )
    {
        aDestination = DestinationMulticast;
    }
}


///
/// \brief Handler for all radio buttons
///

void SetupDlg::OnRadioClicked()
{
    EnableInterface();
}


