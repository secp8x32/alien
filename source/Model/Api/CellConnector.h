#pragma once

#include "Model/Api/ChangeDescriptions.h"

class MODEL_EXPORT CellConnector
	: public QObject
{
public:
	CellConnector(QObject* parent = nullptr) : QObject(parent) {}
	virtual ~CellConnector() = default;

	virtual void reconnect(DataDescription& data, set<uint64_t> const& changedCellIds) = 0;
};

