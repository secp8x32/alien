#include "symboltable.h"

void SymbolTable::addEntry(QString const& key, QString const& value)
{
	_symbolTable[key] = value;
}

void SymbolTable::delEntry(QString const& key)
{
	_symbolTable.remove(key);
}

QString SymbolTable::applyTableToCode(QString const& input) const
{
	if (_symbolTable.contains(input)) {
		return _symbolTable[input];
	}
	return input;
}

void SymbolTable::clearTable()
{
	_symbolTable.clear();
}

QMap< QString, QString > const& SymbolTable::getTable() const
{
	return _symbolTable;
}

void SymbolTable::setTable(const QMap< QString, QString >& table)
{
	_symbolTable = table;
}

void SymbolTable::serializePrimitives(QDataStream& stream) const
{
	stream << _symbolTable;
}

void SymbolTable::deserializePrimitives(QDataStream& stream)
{
	stream >> _symbolTable;
}
