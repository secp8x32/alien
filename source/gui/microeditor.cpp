#include <QTabWidget>
#include <QToolButton>
#include <QLabel>
#include <QTextEdit>
#include <QEvent>

#include "model/simulationparameters.h"
#include "model/simulationcontext.h"
#include "model/energyparticlemap.h"
#include "model/alienfacade.h"
#include "model/entities/cell.h"
#include "model/entities/cellcluster.h"
#include "model/entities/energyparticle.h"
#include "gui/guisettings.h"
#include "gui/guisettings.h"
#include "microeditor/tokentab.h"
#include "microeditor/celledit.h"
#include "microeditor/clusteredit.h"
#include "microeditor/energyedit.h"
#include "microeditor/hexedit.h"
#include "microeditor/metadataedit.h"
#include "microeditor/symboledit.h"
#include "microeditor/cellcomputeredit.h"
#include "global/servicelocator.h"

#include "microeditor.h"

const int tabPosX1 = 410;
const int tabPosX2 = 810;

MicroEditor::MicroEditor(SimulationContext* context, QObject *parent)
	: QObject(parent)
	, _context(context)
    , _focusCell(0)
    , _focusEnergyParticle(0)
    , _currentClusterTab(0)
    , _currentTokenTab(0)
    , _pasteTokenPossible(false)
    , _savedTokenEnergy(0)
{
//    ui->setupUi(this);

}

MicroEditor::~MicroEditor()
{
//    delete ui;
}

void MicroEditor::init (MicroEditorWidgets widgets)
{
	_widgets = widgets;

    //save tab widgets
    _tabCluster = _widgets.tabClusterWidget->widget(0);
    _tabCell = _widgets.tabClusterWidget->widget(1);
    _tabParticle = _widgets.tabClusterWidget->widget(2);
    _tabSelection = _widgets.tabClusterWidget->widget(3);
    _tabMeta = _widgets.tabClusterWidget->widget(4);
    _tabComputer = _widgets.tabComputerWidget->widget(0);
    _tabSymbolTable = _widgets.tabComputerWidget->widget(1);

    //hide widgets
	_widgets.tabClusterWidget->setVisible(false);
	_widgets.tabComputerWidget->setVisible(false);
	_widgets.tabTokenWidget->setVisible(false);

    //set colors
	_widgets.requestCellButton->setStyleSheet(BUTTON_STYLESHEET);
	_widgets.requestEnergyParticleButton->setStyleSheet(BUTTON_STYLESHEET);
	_widgets.delEntityButton->setStyleSheet(BUTTON_STYLESHEET);
	_widgets.delClusterButton->setStyleSheet(BUTTON_STYLESHEET);
	_widgets.addTokenButton->setStyleSheet(BUTTON_STYLESHEET);
	_widgets.delTokenButton->setStyleSheet(BUTTON_STYLESHEET);
	_widgets.buttonShowInfo->setStyleSheet(BUTTON_STYLESHEET);

    //set tooltip
	_widgets.requestCellButton->setToolTip("add cell");
	_widgets.requestEnergyParticleButton->setToolTip("add energy particle");
	_widgets.delEntityButton->setToolTip("delete cell/energy particle");
	_widgets.delClusterButton->setToolTip("delete cell cluster");
	_widgets.addTokenButton->setToolTip("add token");
	_widgets.delTokenButton->setToolTip("del token");
	_widgets.buttonShowInfo->setToolTip("show informations");

    //install event filter for parent widget
	_widgets.tabSymbolsWidget->parent()->installEventFilter(this);

    //establish connections
    connect(_widgets.cellEditor, SIGNAL(cellDataChanged(CellTO)), this, SLOT(changesFromCellEditor(CellTO)));
    connect(_widgets.clusterEditor, SIGNAL(clusterDataChanged(CellTO)), this, SLOT(changesFromClusterEditor(CellTO)));
    connect(_widgets.energyEditor, SIGNAL(energyParticleDataChanged(QVector3D,QVector3D,qreal)), this, SLOT(changesFromEnergyParticleEditor(QVector3D,QVector3D,qreal)));
    connect(_widgets.requestCellButton, SIGNAL(clicked()), this, SIGNAL(requestNewCell()));
    connect(_widgets.requestEnergyParticleButton, SIGNAL(clicked()), this, SIGNAL(requestNewEnergyParticle()));
    connect(_widgets.delEntityButton, SIGNAL(clicked()), this, SLOT(delSelectionClicked()));
    connect(_widgets.delClusterButton, SIGNAL(clicked()), this, SLOT(delExtendedSelectionClicked()));
	connect(_widgets.buttonShowInfo, SIGNAL(clicked()), this, SLOT(buttonShowInfoClicked()));
	connect(_widgets.cellComputerEdit, SIGNAL(changesFromComputerMemoryEditor(QByteArray)), this, SLOT(changesFromComputerMemoryEditor(QByteArray)));
    connect(_widgets.cellComputerEdit, SIGNAL(compileButtonClicked(QString)), this, SLOT(compileButtonClicked(QString)));
    connect(_widgets.addTokenButton, SIGNAL(clicked()), this, SLOT(addTokenClicked()));
    connect(_widgets.delTokenButton, SIGNAL(clicked()), this, SLOT(delTokenClicked()));
    connect(_widgets.tabTokenWidget, SIGNAL(currentChanged(int)), this, SLOT(tokenTabChanged(int)));
    connect(_widgets.metadataEditor, SIGNAL(metadataChanged(QString,QString,quint8,QString)), this, SLOT(changesFromMetadataEditor(QString,QString,quint8,QString)));
    connect(_widgets.symbolEdit, SIGNAL(symbolTableChanged()), this, SLOT(changesFromSymbolTableEditor()));

	_widgets.symbolEdit->loadSymbols(_context->getSymbolTable());
}


void MicroEditor::update ()
{
	_widgets.symbolEdit->loadSymbols(_context->getSymbolTable());
	changesFromSymbolTableEditor(); //nach reset
}

void MicroEditor::setVisible (bool visible)
{
	_widgets.requestCellButton->setVisible(visible);
	_widgets.requestEnergyParticleButton->setVisible(visible);
	_widgets.delEntityButton->setVisible(visible);
	_widgets.delClusterButton->setVisible(visible);
	_widgets.addTokenButton->setVisible(visible);
	_widgets.delTokenButton->setVisible(visible);
	_widgets.buttonShowInfo->setVisible(visible);
}

bool MicroEditor::isVisible ()
{
    return _widgets.requestCellButton->isVisible();
}

bool MicroEditor::eventFilter (QObject * watched, QEvent * event)
{
/*    if( (watched == _tabSymbolsWidget->parent()) && (event->type() == QEvent::Resize) ) {
        setTabSymbolsWidgetVisibility();
    }*/
    return QObject::eventFilter(watched, event);
}

Cell* MicroEditor::getFocusedCell ()
{
    return _focusCell;
}

void MicroEditor::computerCompilationReturn (bool error, int line)
{
	_widgets.cellComputerEdit->setCompilationState(error, line);
}


void MicroEditor::defocused (bool requestDataUpdate)
{
    disconnect(_widgets.tabClusterWidget, SIGNAL(currentChanged(int)), 0, 0);

    if( requestDataUpdate )
        requestUpdate();

    //close tabs
    if( _focusCell )
        _currentClusterTab = _widgets.tabClusterWidget->currentIndex();
    if(_widgets.tabClusterWidget->count() > 0 ) {
        while(_widgets.tabClusterWidget->count() > 0 )
			_widgets.tabClusterWidget->removeTab(0);
		_widgets.tabClusterWidget->setVisible(false);
    }
    if(_widgets.tabComputerWidget->count() > 0 ) {
        while(_widgets.tabComputerWidget->count() > 0)
			_widgets.tabComputerWidget->removeTab(0);
		_widgets.tabComputerWidget->setVisible(false);
    }
    if(_widgets.tabTokenWidget->count() > 0 ) {
        disconnect(_widgets.tabTokenWidget, SIGNAL(currentChanged(int)), 0, 0);  //token widgets will be generated dynamically
        while(_widgets.tabTokenWidget->count() > 0 ) {
            delete _widgets.tabTokenWidget->widget(0);
			_widgets.tabTokenWidget->removeTab(0);
        }
        connect(_widgets.tabTokenWidget, SIGNAL(currentChanged(int)), this, SLOT(tokenTabChanged(int)));
		_widgets.tabTokenWidget->setVisible(false);
    }
    setTabSymbolsWidgetVisibility();
    _currentTokenTab = 0;
    emit numTokenUpdate(0, 0, _pasteTokenPossible);

    //deactivate buttons
    if(_widgets.addTokenButton->isEnabled() )
		_widgets.addTokenButton->setEnabled(false);
    if(_widgets.delTokenButton->isEnabled() )
		_widgets.delTokenButton->setEnabled(false);
    if(_widgets.delEntityButton->isEnabled() )
		_widgets.delEntityButton->setEnabled(false);
    if(_widgets.delClusterButton->isEnabled() )
		_widgets.delClusterButton->setEnabled(false);

    _focusCell = 0;
    _focusEnergyParticle = 0;
}

void MicroEditor::cellFocused (Cell* cell, bool requestDataUpdate)
{
    if( (!isVisible()) || (!_context) || (!cell) )
        return;

    defocused(requestDataUpdate);

    _focusCell = cell;

    //update data for cluster editor
    _context->lock();
    AlienFacade* facade = ServiceLocator::getInstance().getService<AlienFacade>();
    _focusCellReduced = facade->buildFeaturedCellTO(cell);
    QList< quint64 > ids = cell->getCluster()->getCellIds();
	_context->unlock();
	CellMetadata cellMeta = getCellMetadata(cell);
	CellClusterMetadata clusterMeta = getCellClusterMetadata(cell);
	_widgets.clusterEditor->updateCluster(_focusCellReduced);
	_widgets.cellEditor->updateCell(_focusCellReduced);
	_widgets.metadataEditor->updateMetadata(clusterMeta.name, cellMeta.name, cellMeta.color, cellMeta.description);

    //update data for cell function: computer
    if( _focusCellReduced.cellFunctionType == Enums::CellFunction::COMPUTER ) {

        //activate tab for computer widgets
//        _tabTokenWidget->move(10, tabPosY2);
		_widgets.tabComputerWidget->setVisible(true);
		_widgets.tabComputerWidget->insertTab(0, _tabComputer, "cell computer");
		_widgets.tabComputerWidget->insertTab(1, _tabSymbolTable, "symbol table");

        //update computer widgets
		_widgets.cellComputerEdit->updateComputerMemory(_focusCellReduced.computerMemory);

        //load computer code from meta data if available
        if( !cellMeta.computerSourcecode.isEmpty() ) {
			_widgets.cellComputerEdit->updateComputerCode(cellMeta.computerSourcecode);
        }

        //otherwise use translated cell data
        else
			_widgets.cellComputerEdit->updateComputerCode(_focusCellReduced.computerCode);
    }
//    else
//        _tabTokenWidget->move(10, tabPosY1);

    //activate cell cluster tabs
	_widgets.tabClusterWidget->setVisible(true);
	_widgets.tabClusterWidget->insertTab(0, _tabCell, "cell");
	_widgets.tabClusterWidget->insertTab(1, _tabCluster, "cluster");
	_widgets.tabClusterWidget->insertTab(2, _tabMeta, "meta data");
	_widgets.tabClusterWidget->setCurrentIndex(_currentClusterTab);
    connect(_widgets.tabClusterWidget, SIGNAL(currentChanged(int)), this, SLOT(clusterTabChanged(int)));


    //generate tab for each token
    int numToken = _focusCellReduced.tokenEnergies.size();
    if( numToken > 0 ) {
		_widgets.tabTokenWidget->setVisible(true);
        disconnect(_widgets.tabTokenWidget, SIGNAL(currentChanged(int)), 0, 0);
        for(int i = 0; i < numToken; ++i) {
            TokenTab* tokenTab = new TokenTab(_widgets.tabTokenWidget);
            connect(tokenTab, SIGNAL(tokenMemoryChanged(QByteArray)), this, SLOT(changesFromTokenMemoryEditor(QByteArray)));
            connect(tokenTab, SIGNAL(tokenPropChanged(qreal)), this, SLOT(changesFromTokenEditor(qreal)));
			_widgets.tabTokenWidget->addTab(tokenTab, QString("token %1").arg(i+1));
            tokenTab->update(_context->getSymbolTable(), _focusCellReduced.tokenEnergies[i], _focusCellReduced.tokenData[i]);
        }
        connect(_widgets.tabTokenWidget, SIGNAL(currentChanged(int)), this, SLOT(tokenTabChanged(int)));

    }
	SimulationParameters* parameters = _context->getSimulationParameters();
    emit numTokenUpdate(numToken, parameters->cellMaxToken, _pasteTokenPossible);

    //update Symbols Widget
    setTabSymbolsWidgetVisibility();

    //update buttons
	_widgets.delEntityButton->setEnabled(true);
	_widgets.delClusterButton->setEnabled(true);
    if( numToken < parameters->cellMaxToken)
		_widgets.addTokenButton->setEnabled(true);
    else
		_widgets.addTokenButton->setEnabled(false);
    if( numToken > 0 )
		_widgets.delTokenButton->setEnabled(true);
    else
		_widgets.delTokenButton->setEnabled(false);
}

CellMetadata MicroEditor::getCellMetadata(Cell* cell)
{
	_context->lock();
	CellMetadata meta = cell->getMetadata();
	_context->unlock();
	return meta;
}

CellClusterMetadata MicroEditor::getCellClusterMetadata(Cell* cell)
{
	_context->lock();
	CellClusterMetadata meta = cell->getCluster()->getMetadata();
	_context->unlock();
	return meta;
}

void MicroEditor::energyParticleFocused (EnergyParticle* e)
{
    if( (!isVisible()) || (!_context) || (!e) )
        return;

    defocused();

    //activate tab
	_widgets.tabClusterWidget->setVisible(true);
	_widgets.tabClusterWidget->insertTab(0, _tabParticle, "energy particle");

    //activate widgets
	_widgets.delEntityButton->setEnabled(true);
	_widgets.delClusterButton->setEnabled(true);

    _focusEnergyParticle = e;
    energyParticleUpdated_Slot(e);
}

void MicroEditor::energyParticleUpdated_Slot (EnergyParticle* e)
{
    if( !e )
        return;

    //update data for editor if particle is focused (we also use cluster editor)
    if( _focusEnergyParticle == e ) {
        _context->lock();
        auto pos = e->getPosition();
        auto vel = e->getVelocity();
        auto energyValue = e->getEnergy();
        _context->unlock();
		_widgets.energyEditor->updateEnergyParticle(pos, vel, energyValue);
    }
}


void MicroEditor::reclustered (QList< CellCluster* > clusters)
{
    if( !_context)
        return;
    if( _focusCell ) {

        //_focusCell contained in clusters?
        _context->lock();
        bool contained = false;
        foreach(CellCluster* cluster, clusters)
            if( cluster->getCellsRef().contains(_focusCell) )
                contained = true;
        _context->unlock();

        //proceed only if _focusCell is contained in clusters
        if( contained ) {

            //update data for cluster editor
            AlienFacade* facade = ServiceLocator::getInstance().getService<AlienFacade>();
            _context->lock();
            _focusCellReduced = facade->buildFeaturedCellTO(_focusCell);
            _context->unlock();
			CellMetadata cellMeta = getCellMetadata(_focusCell);
			CellClusterMetadata clusterMeta = getCellClusterMetadata(_focusCell);
			_widgets.clusterEditor->updateCluster(_focusCellReduced);
			_widgets.cellEditor->updateCell(_focusCellReduced);
			_widgets.metadataEditor->updateMetadata(clusterMeta.name, cellMeta.name, cellMeta.color, cellMeta.description);

            //update computer code editor
            if( _focusCellReduced.cellFunctionType == Enums::CellFunction::COMPUTER ) {
                //_computerCodeEditor->update(_focusCellReduced.computerCode);
            }
        }
    }
}

void MicroEditor::universeUpdated (SimulationContext* context, bool force)
{
	_context = context;
    defocused(false);
}

void MicroEditor::requestUpdate ()
{
    //save cell data
    if( _focusCell ) {

         //save edited code from code editor
        if( _focusCellReduced.cellFunctionType == Enums::CellFunction::COMPUTER ) {
            QString code = _widgets.cellComputerEdit->getComputerCode();
			CellMetadata meta = getCellMetadata(_focusCell);
			meta.computerSourcecode = code;
			setCellMetadata(_focusCell, meta);
        }

        //save edited code from cluster editor
        if( _currentClusterTab == 0 )
			_widgets.cellEditor->requestUpdate();
        if( _currentClusterTab == 1 )
			_widgets.clusterEditor->requestUpdate();
        if( _currentClusterTab == 2 )
			_widgets.metadataEditor->requestUpdate();

        //save token data => see
        if(_widgets.tabTokenWidget->count() > 0 ) {
            TokenTab* tab = static_cast<TokenTab*>(_widgets.tabTokenWidget->currentWidget());
            tab->requestUpdate();
        }
    }

    //save energy particle data
    if( _focusEnergyParticle ) {
		_widgets.energyEditor->requestUpdate();
    }

}

void MicroEditor::setCellMetadata(Cell* cell, CellMetadata meta)
{
	_context->lock();
	cell->setMetadata(meta);
	_context->unlock();
}

void MicroEditor::setCellClusterMetadata(Cell * cell, CellClusterMetadata meta)
{
	_context->lock();
	cell->getCluster()->setMetadata(meta);
	_context->unlock();
}

void MicroEditor::entitiesSelected (int numCells, int numEnergyParticles)
{
    if( (numCells > 0) || (numEnergyParticles > 0) ){
		_widgets.delEntityButton->setEnabled(true);
		_widgets.delClusterButton->setEnabled(true);
    }
    else {
		_widgets.delEntityButton->setEnabled(false);
		_widgets.delClusterButton->setEnabled(false);
    }

    //active tab if not active
    if(_widgets.tabClusterWidget->currentWidget() != _tabSelection) {
		_widgets.tabClusterWidget->setVisible(true);
        while(_widgets.tabClusterWidget->count() > 0 )
			_widgets.tabClusterWidget->removeTab(0);
		_widgets.tabClusterWidget->insertTab(0, _tabSelection, "selection");
    }

    //update selection tab
    //define auxilliary strings
    QString parStart = "<p style=\" margin-top:0px; margin-bottom:0px; margin-left:0px; margin-right:0px; -qt-block-indent:0; text-indent:0px;\">";
    QString parEnd = "</p>";
    QString colorTextStart = "<span style=\"color:"+CELL_EDIT_TEXT_COLOR1.name()+"\">";
    QString colorDataStart = "<span style=\"color:"+CELL_EDIT_DATA_COLOR1.name()+"\">";
    QString colorEnd = "</span>";
    QString text;
    text = parStart+colorTextStart+ "cells: &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"+colorEnd
           + colorDataStart + QString("%1").arg(numCells)+colorEnd+parEnd;
    text += parStart+colorTextStart+ "energy particles: &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"+colorEnd
           + colorDataStart + QString("%1").arg(numEnergyParticles)+colorEnd+parEnd;
	_widgets.selectionEditor->setText(text);
}

void MicroEditor::addTokenClicked ()
{
    //create token (new token is the last token on the stack)
    int newTokenTab = _currentTokenTab+1;
	SimulationParameters* parameters = _context->getSimulationParameters();
    _focusCellReduced.tokenEnergies.insert(newTokenTab, parameters->tokenCreationEnergy);
    QByteArray data(parameters->cellFunctionComputerTokenMemorySize, 0);
    data[0] = _focusCellReduced.cellTokenAccessNum; //set access number for new token
    _focusCellReduced.tokenData.insert(newTokenTab, data);

    //emit signal to notify other instances => update _focusCell from _focusCellReduced
    invokeUpdateCell(false);

    cellFocused(_focusCell);
	_widgets.tabTokenWidget->setCurrentIndex(newTokenTab);

    //activate Symbols Widget
    setTabSymbolsWidgetVisibility();

/*
    //create token (new token is the last token on the stack)
    _focusCellReduced.tokenEnergies << NEW_TOKEN_ENERGY;
    QVector< quint8 > data(TOKEN_MEMSIZE, 0);
    data[0] = _focusCellReduced.cellTokenAccessNum; //set access number for new token
    _focusCellReduced.tokenData << data;

    //add token tab
    int numToken = _focusCellReduced.tokenEnergies.size();
    TokenTab* tokenTab = new TokenTab(_tabTokenWidget);
    _tabTokenWidget->setVisible(true);
    connect(tokenTab, SIGNAL(tokenMemoryChanged(QString)), this, SLOT(changesFromTokenMemoryEditor(QString)));
    connect(tokenTab, SIGNAL(tokenPropChanged(qreal)), this, SLOT(changesFromTokenEditor(qreal)));
    disconnect(_tabTokenWidget, SIGNAL(currentChanged(int)), 0, 0);
    _tabTokenWidget->addTab(tokenTab, QString("token %1").arg(numToken));
    connect(_tabTokenWidget, SIGNAL(currentChanged(int)), this, SLOT(tokenTabChanged(int)));
    tokenTab->update(_focusCellReduced.tokenEnergies[numToken-1], _focusCellReduced.tokenData[numToken-1]);
    _tabTokenWidget->setCurrentIndex(numToken-1);

    //update widgets
    ui->delTokenButton->setEnabled(true);
    if( numToken == CELL_TOKENSTACKSIZE )
        ui->addTokenButton->setEnabled(false);
    emit numTokenUpdate(numToken, CELL_TOKENSTACKSIZE, _pasteTokenPossible);

    //emit signal to notify other instances
    invokeUpdateCell(false);*/
}

void MicroEditor::delTokenClicked ()
{
    //remove token
    _focusCellReduced.tokenEnergies.removeAt(_widgets.tabTokenWidget->currentIndex());
    _focusCellReduced.tokenData.removeAt(_widgets.tabTokenWidget->currentIndex());

    //emit signal to notify other instances => update _focusCell from _focusCellReduced
    invokeUpdateCell(false);

    int newTokenTab = _currentTokenTab;
    int numToken = _focusCellReduced.tokenEnergies.size();
    if( (newTokenTab > 0) && (newTokenTab == numToken) )
        newTokenTab--;
    cellFocused(_focusCell, false);
	_widgets.tabTokenWidget->setCurrentIndex(newTokenTab);

    //update sybols widget
    setTabSymbolsWidgetVisibility();


/*
    //remove token
    _focusCellReduced.tokenEnergies.removeAt(_tabTokenWidget->currentIndex());
    _focusCellReduced.tokenData.removeAt(_tabTokenWidget->currentIndex());
    int numToken = _focusCellReduced.tokenEnergies.size();

    //remove token widget
    disconnect(_tabTokenWidget, SIGNAL(currentChanged(int)), 0, 0);
    delete _tabTokenWidget->currentWidget();
    connect(_tabTokenWidget, SIGNAL(currentChanged(int)), this, SLOT(tokenTabChanged(int)));

    //decrement token number on next tabs
    for(int i = _tabTokenWidget->currentIndex(); i < numToken; ++i)
        _tabTokenWidget->setTabText(i, QString("token %1").arg(i+1));

    //update button and tab widget
    ui->addTokenButton->setEnabled(true);
    if( numToken == 0 ) {
        ui->delTokenButton->setEnabled(false);
        _tabTokenWidget->setVisible(false);
    }
    emit numTokenUpdate(numToken, CELL_TOKENSTACKSIZE, _pasteTokenPossible);

    //emit signal to notify other instances
    invokeUpdateCell(false);*/
}

void MicroEditor::copyTokenClicked ()
{
    requestUpdate();
    _savedTokenEnergy = _focusCellReduced.tokenEnergies[_currentTokenTab];
    _savedTokenData = _focusCellReduced.tokenData[_currentTokenTab];
    _pasteTokenPossible = true;
    int numToken = _focusCellReduced.tokenEnergies.size();
	SimulationParameters* parameters = _context->getSimulationParameters();
	emit numTokenUpdate(numToken, parameters->cellMaxToken, _pasteTokenPossible);
}

void MicroEditor::pasteTokenClicked ()
{
    //create token (new token is the next to current token on the stack)
    int newTokenTab = _currentTokenTab+1;
    _focusCellReduced.tokenEnergies.insert(newTokenTab, _savedTokenEnergy);
    _savedTokenData[0] = _focusCellReduced.cellTokenAccessNum; //set access number for new token
    _focusCellReduced.tokenData.insert(newTokenTab, _savedTokenData);

    //emit signal to notify other instances => update _focusCell from _focusCellReduced
    invokeUpdateCell(false);

    cellFocused(_focusCell);
	_widgets.tabTokenWidget->setCurrentIndex(newTokenTab);

    //update Symbols Widget
    setTabSymbolsWidgetVisibility();
}

void MicroEditor::delSelectionClicked ()
{
    if( !_context)
        return;

    //defocus
    defocused(false);

    //request deletion
    emit delSelection();
}

void MicroEditor::delExtendedSelectionClicked ()
{
    if( !_context)
        return;

    //defocus
    defocused(false);

    //request deletion
    emit delExtendedSelection();
}

void MicroEditor::buttonShowInfoClicked()
{
	if (_widgets.buttonShowInfo->isChecked()) {
		_widgets.buttonShowInfo->setIcon(QIcon(resourceInfoOn));
	}
	else {
		_widgets.buttonShowInfo->setIcon(QIcon(resourceInfoOff));
	}
	emit toggleInformation(_widgets.buttonShowInfo->isChecked());
}

void MicroEditor::changesFromCellEditor (CellTO newCellProperties)
{
    //copy cell properties editable by cluster editor
    _focusCellReduced.copyCellProperties(newCellProperties);

    //close tabs
    if(_widgets.tabComputerWidget->count() > 0 ) {
        while(_widgets.tabComputerWidget->count() > 0)
			_widgets.tabComputerWidget->removeTab(0);
		_widgets.tabComputerWidget->setVisible(false);
    }

    //update data for cell function: computer
    if( _focusCellReduced.cellFunctionType == Enums::CellFunction::COMPUTER ) {

        //activate tab for computer widgets
//        _tabTokenWidget->move(10, tabPosY2);
		_widgets.tabComputerWidget->setVisible(true);
		_widgets.tabComputerWidget->insertTab(0, _tabComputer, "cell computer");
		_widgets.tabComputerWidget->insertTab(1, _tabSymbolTable, "symbol table");
		_widgets.cellComputerEdit->updateComputerMemory(_focusCellReduced.computerMemory);

        //load computer code from meta data if available
		CellMetadata meta = getCellMetadata(_focusCell);
        if( !meta.computerSourcecode.isEmpty() ) {
			_widgets.cellComputerEdit->updateComputerCode(meta.computerSourcecode);
        }

        //otherwise use translated cell data
        else
			_widgets.cellComputerEdit->updateComputerCode(_focusCellReduced.computerCode);
    }
//    else
//        _tabTokenWidget->move(10, tabPosY1);

    //update Symbols Widget
    setTabSymbolsWidgetVisibility();

    //emit signal to notify other instances
    invokeUpdateCell(false);

}

void MicroEditor::changesFromClusterEditor (CellTO newClusterProperties)
{
    //copy cell properties editable by cluster editor
    _focusCellReduced.copyClusterProperties(newClusterProperties);

    //emit signal to notify other instances
    invokeUpdateCell(true);
}

void MicroEditor::changesFromEnergyParticleEditor (QVector3D pos, QVector3D vel, qreal energyValue)
{
    if( (!_context) || (!_focusEnergyParticle) )
        return;

    //update energy particle (we do this without informing the simulator...)
    _context->lock();
	_context->getEnergyParticleMap()->setParticle(_focusEnergyParticle->getPosition(), 0);
    _focusEnergyParticle->setPosition(pos);
    _focusEnergyParticle->setVelocity(vel);
    _focusEnergyParticle->setEnergy(energyValue);
	_context->getEnergyParticleMap()->setParticle(_focusEnergyParticle->getPosition(), _focusEnergyParticle);
    _context->unlock();

    //emit signal to notify other instances
    emit energyParticleUpdated(_focusEnergyParticle);
}

void MicroEditor::changesFromTokenEditor (qreal energy)
{
    _focusCellReduced.tokenEnergies[_currentTokenTab] = energy;

    //emit signal to notify other instances
    invokeUpdateCell(false);
}

void MicroEditor::changesFromComputerMemoryEditor(QByteArray const& data)
{
    //copy cell memory
    _focusCellReduced.computerMemory = data;

    //emit signal to notify other instances
    invokeUpdateCell(false);
}

void MicroEditor::changesFromTokenMemoryEditor(QByteArray data)
{
    //copy token memory
    _focusCellReduced.tokenData[_currentTokenTab] = data;

    //emit signal to notify other instances
    invokeUpdateCell(false);
}

void MicroEditor::changesFromMetadataEditor(QString clusterName, QString cellName, quint8 cellColor, QString cellDescription)
{
	{
		CellMetadata meta = getCellMetadata(_focusCell);
		meta.name = cellName;
		meta.color = cellColor;
		meta.description = cellDescription;
		setCellMetadata(_focusCell, meta);
	}
	{
		CellClusterMetadata meta = getCellClusterMetadata(_focusCell);
		meta.name = clusterName;
		setCellClusterMetadata(_focusCell, meta);
	}

    //emit signal to notify macro editor
    emit metadataUpdated();
}

void MicroEditor::changesFromSymbolTableEditor ()
{
    QWidget* widget = _widgets.tabTokenWidget->currentWidget();
    TokenTab* tokenTab= qobject_cast<TokenTab*>(widget);
    if( tokenTab ) {
		tokenTab->update(_context->getSymbolTable(), _focusCellReduced.tokenEnergies[_currentTokenTab], _focusCellReduced.tokenData[_currentTokenTab]);
    }
//    _focusCellReduced.tokenData[_currentTokenTab] = data;
}

void MicroEditor::clusterTabChanged (int index)
{
    requestUpdate();
    _currentClusterTab = index;
}

void MicroEditor::tokenTabChanged (int index)
{
    if( _currentTokenTab >= 0 ) {
        TokenTab* tab = static_cast<TokenTab*>(_widgets.tabTokenWidget->widget(_currentTokenTab));
        if( tab )
            tab->requestUpdate();
    }
    _currentTokenTab = index;
}

void MicroEditor::compileButtonClicked (QString code)
{
    if( (!_context) || (!_focusCell) )
        return;

    //transfer code to cell meta data
	CellMetadata meta = _focusCell->getMetadata();
	meta.computerSourcecode = code;
	setCellMetadata(_focusCell, meta);

    //update cell data
    _focusCellReduced.computerCode = code;

    //NOTE: widgets are updated via reclustered(...)

    //emit signal to notify other instances
	_widgets.cellComputerEdit->expectCellCompilerAnswer();
    invokeUpdateCell(false);
}

void MicroEditor::invokeUpdateCell (bool clusterDataChanged)
{
    QList< Cell* > cells;
    QList< CellTO > newCellsData;
    cells << _focusCell;
    newCellsData << _focusCellReduced;
    emit updateCell(cells, newCellsData, clusterDataChanged);
}

void MicroEditor::setTabSymbolsWidgetVisibility ()
{
    if(_widgets.tabTokenWidget->isVisible() ) {
		_widgets.tabSymbolsWidget->setGeometry(tabPosX2, _widgets.tabClusterWidget->y(), _widgets.tabSymbolsWidget->width(), _widgets.tabSymbolsWidget->height());
		_widgets.tabSymbolsWidget->setVisible(true);
    }
    else if(_widgets.tabComputerWidget->isVisible() ) {
		_widgets.tabSymbolsWidget->setGeometry(tabPosX1, _widgets.tabClusterWidget->y(), _widgets.tabSymbolsWidget->width(), _widgets.tabSymbolsWidget->height());
		_widgets.tabSymbolsWidget->setVisible(true);
    }
    else
		_widgets.tabSymbolsWidget->setVisible(false);
}
