/*
 * bufferoptions.h
 *
 *  Created on: Dec 3, 2009
 *      Author: fgobeil
 */

#ifndef __BUFFEROPTIONSDLG_H__
#define __BUFFEROPTIONSDLG_H__


#include <QtGui/QDialog>
#include <QtGui/QPushButton>
#include <QtGui/QCheckBox>
#include <QtGui/QRadioButton>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>

#include <PvConfigurationReader.h>
#include <PvConfigurationWriter.h>


class PvPipeline;
class GEVPlayer;

class BufferOptionsDlg : public QDialog
{
    Q_OBJECT

public:

    BufferOptionsDlg( PvPipeline *aPipeline, GEVPlayer* aGEVPlayerDlg );
	virtual ~BufferOptionsDlg();

    // Persistence
    static void Save( PvConfigurationWriter &aWriter, PvPipeline *aPipeline );
    static void Load( PvConfigurationReader &aReader, PvPipeline *aPipeline );

protected:

	void CreateLayout();

	void accept();

protected slots:

private:

	QLineEdit *mBufferCountEdit;
	QLineEdit *mBufferSizeEdit;

	QPushButton *mOKButton;
	QPushButton *mCancelButton;

	GEVPlayer* mGEVPlayerDlg;
	PvPipeline *mPipeline;

	PvUInt32 mBufferSize;
	PvUInt32 mBufferCount;

	PvUInt32 mOriginalBufferSize;
	PvUInt32 mOriginalBufferCount;

};


#endif /* __BUFFEROPTIONSDLG_H__ */


