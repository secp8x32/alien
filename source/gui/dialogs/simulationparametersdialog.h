#ifndef SIMULATIONPARAMETERSDIALOG_H
#define SIMULATIONPARAMETERSDIALOG_H

#include <QDialog>
#include "model/simulationparameters.h"

namespace Ui {
class SimulationParametersDialog;
}

class SimulationParametersDialog : public QDialog
{
    Q_OBJECT

public:
    SimulationParametersDialog(SimulationParameters* parameters, QWidget *parent = 0);
    ~SimulationParametersDialog();

    void updateSimulationParameters ();

private slots:
    void setLocalSimulationParametersToWidgets ();
    void getLocalSimulationParametersFromWidgets ();

    void defaultButtonClicked ();
    void loadButtonClicked ();
    void saveButtonClicked ();

private:
	void setItem(QString key, int matchPos, int value);
	void setItem(QString key, int matchPos, qreal value);
	int getItemInt(QString key, int matchPos);
	qreal getItemReal(QString key, int matchPos);

	Ui::SimulationParametersDialog *ui;

    //simulation parameters (not the global ones)
	SimulationParameters& _simulationParameters;
    SimulationParameters _localSimulationParameters;
};

#endif // SIMULATIONPARAMETERSDIALOG_H