﻿#include <QAction>

#include "ActionHolder.h"

ActionHolder::ActionHolder(QObject* parent) : QObject(parent)
{
	actionNewSimulation = new QAction("New", this);
	actionNewSimulation->setEnabled(true);
	actionLoadSimulation = new QAction("Load", this);
	actionLoadSimulation->setEnabled(true);
	actionSaveSimulation = new QAction("Save", this);
	actionSaveSimulation->setEnabled(true);
	actionRunSimulation = new QAction("Run", this);
	actionRunSimulation->setEnabled(true);
	actionRunSimulation->setCheckable(true);
	QIcon iconRunSimulation;
	iconRunSimulation.addFile(":/Icons/play.png", QSize(), QIcon::Normal, QIcon::Off);
	actionRunSimulation->setIcon(iconRunSimulation);
	actionRunSimulation->setIconVisibleInMenu(false);
	actionRunStepForward = new QAction("Step forward", this);
	actionRunStepForward->setEnabled(true);
	QIcon iconStepForward;
	iconStepForward.addFile(":/Icons/step.png", QSize(), QIcon::Normal, QIcon::Off);
	actionRunStepForward->setIcon(iconStepForward);
	actionRunStepForward->setIconVisibleInMenu(false);
	actionRunStepBackward = new QAction("Step backward", this);
	actionRunStepBackward->setEnabled(false);
	QIcon iconStepBackward;
	iconStepBackward.addFile(":/Icons/step_back.png", QSize(), QIcon::Normal, QIcon::Off);
	actionRunStepBackward->setIcon(iconStepBackward);
	actionRunStepBackward->setIconVisibleInMenu(false);
	actionSnapshot = new QAction("Snapshot", this);
	actionSnapshot->setEnabled(true);
	QIcon iconSnapshot;
	iconSnapshot.addFile(":/Icons/snapshot.png", QSize(), QIcon::Normal, QIcon::Off);
	actionSnapshot->setIcon(iconSnapshot);
	actionSnapshot->setIconVisibleInMenu(false);
	actionRestore = new QAction("Restore", this);
	actionRestore->setEnabled(false);
	QIcon iconRestore;
	iconRestore.addFile(":/Icons/restore_active.png", QSize(), QIcon::Normal, QIcon::Off);
	actionRestore->setIcon(iconRestore);
	actionRestore->setIconVisibleInMenu(false);
	actionExit = new QAction("Exit", this);
	actionExit->setEnabled(true);

	actionComputationGrid = new QAction("Computation grid", this);
	actionComputationGrid->setEnabled(true);
	actionEditSimParameters = new QAction("Edit", this);
	actionEditSimParameters->setEnabled(true);
	actionLoadSimParameters = new QAction("Load", this);
	actionLoadSimParameters->setEnabled(true);
	actionSaveSimParameters = new QAction("Save", this);
	actionSaveSimParameters->setEnabled(true);

	actionEditSymbols = new QAction("Edit", this);
	actionEditSymbols->setEnabled(true);
	actionLoadSymbols = new QAction("Load", this);
	actionLoadSymbols->setEnabled(true);
	actionSaveSymbols = new QAction("Save", this);
	actionSaveSymbols->setEnabled(true);
	actionMergeWithSymbols = new QAction("Merge with", this);
	actionMergeWithSymbols->setEnabled(true);

	actionEditor = new QAction("Editor", this);
	actionEditor->setCheckable(true);
	actionEditor->setChecked(false);
	actionEditor->setEnabled(true);
	QIcon iconEditor;
	iconEditor.addFile(":/Icons/EditorView.png", QSize(), QIcon::Normal, QIcon::On);
	iconEditor.addFile(":/Icons/PixelView.png", QSize(), QIcon::Normal, QIcon::Off);
	actionEditor->setIcon(iconEditor);
	actionEditor->setIconVisibleInMenu(false);
	actionMonitor = new QAction("Monitor", this);
	actionMonitor->setEnabled(true);
	QIcon iconMonitor;
	iconMonitor.addFile(":/Icons/monitor.png", QSize(), QIcon::Normal, QIcon::Off);
	actionMonitor->setIcon(iconMonitor);
	actionMonitor->setIconVisibleInMenu(false);
	actionZoomIn = new QAction("Zoom in", this);
	actionZoomIn->setEnabled(true);
	QIcon iconZoomIn;
	iconZoomIn.addFile(":/Icons/zoom_in.png", QSize(), QIcon::Normal, QIcon::Off);
	actionZoomIn->setIcon(iconZoomIn);
	actionZoomIn->setIconVisibleInMenu(false);
	actionZoomOut = new QAction("Zoom out", this);
	actionZoomOut->setEnabled(true);
	QIcon iconZoomOut;
	iconZoomOut.addFile(":/Icons/zoom_out.png", QSize(), QIcon::Normal, QIcon::Off);
	actionZoomOut->setIcon(iconZoomOut);
	actionZoomOut->setIconVisibleInMenu(false);
	actionFullscreen = new QAction("Fullscreen", this);
	actionFullscreen->setEnabled(true);
	actionFullscreen->setCheckable(true);
	actionFullscreen->setChecked(true);
	actionShowCellInfo = new QAction("Cell info", this);
	QIcon iconCellInfo;
	iconCellInfo.addFile("://Icons/info_off.png", QSize(), QIcon::Normal, QIcon::Off);
	iconCellInfo.addFile("://Icons/info_on.png", QSize(), QIcon::Normal, QIcon::On);
	actionShowCellInfo->setIcon(iconCellInfo);
	actionShowCellInfo->setIconVisibleInMenu(false);
	actionShowCellInfo->setEnabled(false);
	actionShowCellInfo->setCheckable(true);
	actionShowCellInfo->setChecked(false);

	actionNewCell = new QAction("New cell", this);
	QIcon iconNewCell;
	iconNewCell.addFile("://Icons/add_cell.png", QSize(), QIcon::Normal, QIcon::Off);
	actionNewCell->setIcon(iconNewCell);
	actionNewCell->setIconVisibleInMenu(false);
	actionNewCell->setEnabled(true);
	actionNewParticle = new QAction("New particle", this);;
	QIcon iconNewParticle;
	iconNewParticle.addFile("://Icons/add_energy.png", QSize(), QIcon::Normal, QIcon::Off);
	actionNewParticle->setIcon(iconNewParticle);
	actionNewParticle->setIconVisibleInMenu(false);
	actionNewParticle->setEnabled(true);
	actionCopyEntity = new QAction("Copy entity", this);;
	actionCopyEntity->setEnabled(false);
	actionPasteEntity = new QAction("Paste entity", this);
	actionPasteEntity->setEnabled(false);
	actionDeleteEntity = new QAction("Delete entity", this);
	actionDeleteEntity->setEnabled(false);
	actionNewToken = new QAction("New token", this);
	QIcon iconNewToken;
	iconNewToken.addFile("://Icons/add_token.png", QSize(), QIcon::Normal, QIcon::Off);
	actionNewToken->setIcon(iconNewToken);
	actionNewToken->setIconVisibleInMenu(false);
	actionNewToken->setEnabled(false);
	actionCopyToken = new QAction("Copy token", this);
	actionCopyToken->setEnabled(false);
	actionPasteToken = new QAction("Paste token", this);
	actionPasteToken->setEnabled(false);
	actionDeleteToken = new QAction("Delete token", this);
	QIcon iconDelToken;
	iconDelToken.addFile("://Icons/del_token.png", QSize(), QIcon::Normal, QIcon::Off);
	actionDeleteToken->setIcon(iconDelToken);
	actionDeleteToken->setIconVisibleInMenu(false);
	actionDeleteToken->setEnabled(false);

	actionNewRectangle = new QAction("New rectangle", this);
	actionNewRectangle->setEnabled(true);
	actionNewHexagon = new QAction("New hexagon", this);
	actionNewHexagon->setEnabled(true);
	actionNewParticles = new QAction("New particles", this);
	actionNewParticles->setEnabled(true);
	actionLoadCol = new QAction("Load", this);
	actionLoadCol->setEnabled(true);
	actionSaveCol = new QAction("Save", this);
	actionSaveCol->setEnabled(false);
	actionCopyCol = new QAction("Copy", this);
	actionCopyCol->setShortcut(Qt::CTRL + Qt::Key_C);
	actionCopyCol->setEnabled(false);
	actionPasteCol = new QAction("Paste", this);
	actionPasteCol->setShortcut(Qt::CTRL + Qt::Key_V);
	actionPasteCol->setEnabled(false);
	actionDeleteSel = new QAction("Delete selection", this);
	QIcon iconDeleteSel;
	iconDeleteSel.addFile("://Icons/del_cell.png", QSize(), QIcon::Normal, QIcon::Off);
	actionDeleteSel->setIcon(iconDeleteSel);
	actionDeleteSel->setIconVisibleInMenu(false);
	actionDeleteSel->setEnabled(false);
	actionDeleteCol = new QAction("Delete extended selection", this);
	QIcon iconDeleteCol;
	iconDeleteCol.addFile("://Icons/add_cluster.png", QSize(), QIcon::Normal, QIcon::Off);
	actionDeleteCol->setIcon(iconDeleteCol);
	actionDeleteCol->setIconVisibleInMenu(false);
	actionDeleteCol->setEnabled(false);
	actionRandomMultiplier = new QAction("Random multiplier", this);
	actionRandomMultiplier->setEnabled(false);
	actionGridMultiplier = new QAction("Grid multiplier", this);
	actionGridMultiplier->setEnabled(false);

	actionAbout = new QAction("About artificial life environment (alien)", this);
	actionAbout->setEnabled(true);
	actionDocumentation = new QAction("Documentation", this);
	actionDocumentation->setEnabled(true);
	actionDocumentation->setCheckable(true);

	actionRestrictTPS = new QAction("restrict TPS", this);
	actionRestrictTPS->setCheckable(true);
	actionRestrictTPS->setChecked(false);
	actionRestrictTPS->setEnabled(true);
}

