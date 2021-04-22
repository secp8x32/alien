#pragma once

#include <QObject>
#include <QTimer>

#include "Definitions.h"

class ZoomActionController : public QObject
{
    Q_OBJECT
public:
    ZoomActionController(QObject* parent = nullptr);

    void init(ActionHolder* actions, SimulationViewWidget* simulationViewWidget);

    Q_SLOT void onZoomInClicked();
    Q_SLOT void onZoomOutClicked();

    Q_SLOT void onContinuousZoomIn(QVector2D const& worldPos);
    Q_SLOT void onContinuousZoomOut(QVector2D const& worldPos);
    Q_SLOT void onEndContinuousZoom();

    Q_SIGNAL void updateActionsState();

private:
    Q_SLOT void onContinuousZoom();

    bool isSimulationRunning() const;
    void setItemView();
    void setPixelOrVectorView();

    ActionHolder* _actions = nullptr;
    SimulationViewWidget* _simulationViewWidget = nullptr;

    enum class ContinuousZoomMode
    {
        In,
        Out
    };
    boost::optional<ContinuousZoomMode> _continuousZoomMode;
    boost::optional<QVector2D> _continuousZoomWorldPos;
    QTimer _continuousZoomTimer;
};