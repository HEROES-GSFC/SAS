// *****************************************************************************
//
//     Copyright (c) 2009, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#pragma once

#include <QtGui/QDialog>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QRadioButton>
#include <QtGui/QLineEdit>
#include <QtGui/QGroupBox>

#include <PvConfigurationReader.h>
#include <PvConfigurationWriter.h>


class SetupDlg : public QDialog
{
    Q_OBJECT

public:

    SetupDlg( QWidget* aParent );
	virtual ~SetupDlg();

    int exec();

    // Persistence
    void Save( PvConfigurationWriter &aWriter );
    void Load( PvConfigurationReader &aReader );
    bool IsTheSame( PvConfigurationReader &aReader );

    enum Role
    {
        RoleInvalid = -1,
        RoleCtrlData = 0,
        RoleCtrl,
        RoleData,
        RoleMonitor
    };

    enum Destination
    {
        DestinationInvalid = -1,
        DestinationUnicastAuto = 0,
        DestinationUnicastOther = 1,
        DestinationMulticast = 2,
        DestinationUnicastSpecific = 3
    };

    void SetEnabled( bool aEnabled ) { mEnabled = aEnabled; }

    // Get accessors, to be used AFTER dialog is closed
    Role GetRole() const { return mSetup.mRole; }
    unsigned short GetDefaultChannel() const { return mSetup.mDefaultChannel; }
    Destination GetDestination() const { return mSetup.mDestination; }
    unsigned short GetUnicastSpecificPort() const { return mSetup.mUnicastSpecificPort; }
    const QString &GetUnicastIP() const { return mSetup.mUnicastIP; }
    unsigned short GetUnicastPort() const { return mSetup.mUnicastPort; }
    const QString &GetMulticastIP() const { return mSetup.mMulticastIP; }
    unsigned short GetMulticastPort() const { return mSetup.mMulticastPort; }

    // Set accessors, to be used BEFORE dialog is opened
    void SetRole( Role aRole ) { mSetup.mRole = aRole; }
    void SetDefaultChannel( unsigned short aChannel ) { mSetup.mDefaultChannel = aChannel; }
    void SetDestination( Destination aDestination ) { mSetup.mDestination = aDestination; }
    void SetUnicastIP( const QString &aIP ) { mSetup.mUnicastIP = aIP; }
    void SetUnicastPort( unsigned short aPort ) { mSetup.mUnicastPort = aPort; }
    void SetMulticastIP( const QString &aMulticastIP ) { mSetup.mMulticastIP = aMulticastIP; }
    void SetMulticastPort( unsigned short aMulticastPort ) { mSetup.mMulticastPort = aMulticastPort; }

protected slots:

    void accept();
    void reject();
    void OnRadioClicked();

protected:

    struct Setup
    {
        Setup()
        {
            Reset();
        }

        void Reset()
        {
            mRole = RoleCtrlData;
            mDefaultChannel = 0;
            mDestination = DestinationUnicastAuto;
            mUnicastSpecificPort = 0;
            mUnicastIP = "0.0.0.0";
            mUnicastPort = 0;
            mMulticastIP = "239.192.1.1";
            mMulticastPort = 1042;
        }

        Role mRole;
        unsigned short mDefaultChannel;
        Destination mDestination;
        unsigned short mUnicastSpecificPort;
        QString mUnicastIP;
        unsigned short mUnicastPort;
        QString mMulticastIP;
        unsigned short mMulticastPort;

    };

    void CreateLayout();
    QGroupBox *CreateRole();
    QGroupBox *CreateDefaultChannel();
    QGroupBox *CreateDestination();

    void LoadToUI();

    static void Save( PvConfigurationWriter &aWriter, Setup &aSetup );
    static void Load( PvConfigurationReader &aReader, Setup &aSetup );

    static void StrToRole( const QString &aStr, Role &aRole );
    static void StrToDestination( const QString &aStr, Destination &aDestination );

    void EnableInterface();

    void IPStrToCtrl( const QString &aIPStr, QLineEdit *aCtrl );

    QRadioButton *mCtrlDataRadio;
    QRadioButton *mCtrlRadio;
    QRadioButton *mDataRadio;
    QRadioButton *mMonitorRadio;
    QLabel *mDefaultChannelLabel;
    QLineEdit *mDefaultChannelEdit;
    QRadioButton *mUnicastSpecificRadio;
    QRadioButton *mUnicastAutoRadio;
    QRadioButton *mUnicastOtherRadio;
    QLineEdit *mUnicastSpecificPortEdit;
    QLineEdit *mUnicastIPCtrl;
    QLineEdit *mUnicastPortEdit;
    QRadioButton *mMulticastRadio;
    QLineEdit *mMulticastIPCtrl;
    QLabel *mUnicastSpecificPortLabel;
    QLabel *mUnicastIPLabel;
    QLabel *mUnicastPortLabel;
    QLineEdit *mMulticastPortEdit;
    QLabel *mMulticastIPLabel;
    QLabel *mMulticastPortLabel;
    QPushButton *mOKButton;
    QPushButton *mCancelButton;

private:

    Setup mSetup;
    bool mEnabled;
};

