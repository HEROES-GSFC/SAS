// *****************************************************************************
//
//     Copyright (c) 2008, Pleora Technologies Inc., All rights reserved.
//
// *****************************************************************************

#pragma once

#include <QtGui/QDialog>
#include <QtGui/QLabel>
#include <QtGui/QPushButton>
#include <QtGui/QGroupBox>
#include <QtGui/QComboBox>
#include <QtGui/QCheckBox>
#include <QtGui/QDoubleSpinBox>


#include <PvBufferConverter.h>
#include <PvFilterRGB.h>
#include <PvConfigurationWriter.h>
#include <PvConfigurationReader.h>

class ThreadDisplay;


class FilteringDlg : public QDialog
{
    Q_OBJECT

public:

    FilteringDlg( QWidget* aParent );
	virtual ~FilteringDlg();

    // Persistence
    void Save( PvConfigurationWriter &aWriter );
    void Load( PvConfigurationReader &aReader );

    void SetThreadDisplay( ThreadDisplay *aThreadDisplay ) { mThreadDisplay = aThreadDisplay; }

    PvBayerFilterType GetBayerFilter() const { return mBayerFilter; }
    bool IsFilterRGBEnabled() const { return mFilterRGBEnabled; }

    void ApplyFilterRGB( PvBuffer *aBuffer );
    void ConfigureConverter( PvBufferConverter &aConverter );


protected:

	void CreateLayout();
	void InitControls();

protected slots:


    void OnBnClickedEnabledcheck();
    void OnBnClickedWbbutton();
    void OnBnClickedResetbutton();
    void SetRedSlider(double aValue);
    void SetRedSpin(int aValue);
    void SetGreenSlider(double aValue);
    void SetGreenSpin(int aValue);
    void SetBlueSlider(double aValue);
    void SetBlueSpin(int aValue);

    void SetRedOffsetSlider(int aValue);
    void SetRedOffsetSpin(int aValue);
    void SetGreenOffsetSlider(int aValue);
    void SetGreenOffsetSpin(int aValue);
    void SetBlueOffsetSlider(int aValue);
    void SetBlueOffsetSpin(int aValue);
    void OnBayerComboChanged( int aIndex ); 

private:

    ThreadDisplay *mThreadDisplay;

    void EnableInterface();
    void ToDialog();
    void FromDialog();

    double FromGainSlider( double aValue );
    int ToGainSlider( QSlider *aSlider, double &aValue );

	QCheckBox *mRGBFilterCheckBox;

    QDoubleSpinBox *mRGSpin;
    QDoubleSpinBox *mGGSpin;
    QDoubleSpinBox *mBGSpin;
    QSpinBox *mROSpin;
    QSpinBox *mGOSpin;
    QSpinBox *mBOSpin;
    QSlider *mRGSlider;
    QSlider *mGGSlider;
    QSlider *mBGSlider;
    QSlider *mROSlider;
    QSlider *mGOSlider;
    QSlider *mBOSlider;
    QPushButton *mWBButton;
    QPushButton *mResetButton;
    QComboBox *mBayerCombo;
    QLabel *mGainsLabel;
    QLabel *mOffsetsLabel;
    QLabel *mRedLabel;
    QLabel *mGreenLabel;
    QLabel *mBlueLabel;
    bool mSettingSlider;

    PvFilterRGB mFilterRGB;
    bool mFilterRGBEnabled;
    PvBayerFilterType mBayerFilter;

    bool mLoading;
    int mSkipSyncSpinsCount;
    int mSkipSyncSlidersCount;
};
