#include "basestructer.h"


QDataStream& operator<<(QDataStream& QDS, const AccessRode& C)
{
	QDS << C.ID << C.baliseID << C.routeID << C.route << C.routeType \
		<< C.annBeginName << C.annBeginCodeOrderMax << C.annunciatorEndName \
		<< C.balise << C.turnout << C.speed << C.pathWay << C.disasterPreLen << C.remark;
	return QDS;
}

QDataStream& operator>>(QDataStream& QDS, AccessRode& C)
{
	QDS >> C.ID >> C.baliseID >> C.routeID >> C.route >> C.routeType \
		>> C.annBeginName >> C.annBeginCodeOrderMax >> C.annunciatorEndName \
		>> C.balise >> C.turnout >> C.speed >> C.pathWay >> C.disasterPreLen >> C.remark;
	return QDS;
}

QDataStream& operator<<(QDataStream& QDS, const AccessRode_MapType& C)
{
	QDS << C.balise_ID <<C.balise_Diatance<< C.speed << C.speed_Length << C.pathWay_Length \
		<< C.pathWay_Fre << C.pathWay_SignalType << C.pathWay_Name << C.annBeginName << C.annEndName;
	return QDS;
}

QDataStream& operator>>(QDataStream& QDS, AccessRode_MapType& C)
{
	QDS >> C.balise_ID >>C.balise_Diatance>> C.speed >> C.speed_Length >> C.pathWay_Length \
		>> C.pathWay_Fre >> C.pathWay_SignalType >> C.pathWay_Name >> C.annBeginName >> C.annEndName;
	return QDS;
}

QDataStream& operator<<(QDataStream& QDS, const AccessRode_Sheet& C)
{
	QDS << C.sheet_name << C.sheet;
	return QDS;
}

QDataStream& operator<<(QDataStream& QDS, AccessRode_Sheet& C)
{
	QDS >> C.sheet_name >> C.sheet;
	return QDS;
}

QDataStream& operator<<(QDataStream& QDS, const Station& C)
{
	QDS << C.ID << C.stationName << C.regionID << C.subareaID << C.stationID;
	return QDS;
}

QDataStream& operator>>(QDataStream& QDS, Station& C)
{
	QDS >> C.ID >> C.stationName >> C.regionID >> C.subareaID >> C.stationID;
	return QDS;
}

QDataStream& operator<<(QDataStream& QDS, const Grade& C)
{
	QDS << C.ID << C.grade << C.len << C.endKm << C.remark;
	return QDS;
}

QDataStream& operator>>(QDataStream& QDS, Grade& C)
{
	QDS >> C.ID >> C.grade >> C.len >> C.endKm >> C.remark;
	return QDS;
}

QDataStream& operator<<(QDataStream& QDS, const NeutralSection& C)
{
	QDS << C.ID << C.beganKm << C.endKm << C.len << C.remark;
	return QDS;
}

QDataStream& operator>>(QDataStream& QDS, NeutralSection& C)
{
	QDS >> C.ID >> C.beganKm >> C.endKm >> C.len >> C.remark;
	return QDS;
}

QDataStream& operator<<(QDataStream& QDS, const PathWayData& C)
{
	QDS << C.ID << C.stationName << C.signalSpotName << C.signalSpotKm\
		<< C.signalSpotType << C.insulationJointType << C.pathWayName\
		<< C.pathWayFre << C.pathWayLen << C.pathWayCof << C.remark;
	return QDS;
}

QDataStream& operator>>(QDataStream& QDS, PathWayData& C)
{
	QDS >> C.ID >> C.stationName >> C.signalSpotName >> C.signalSpotKm\
		>> C.signalSpotType >> C.insulationJointType >> C.pathWayName\
		>> C.pathWayFre >> C.pathWayLen >> C.pathWayCof >> C.remark;
	return QDS;
}

QDataStream& operator<<(QDataStream& QDS, const PathWayData_Less& C)
{
	QDS << C.signalSpotType << C.pathWayFre << C.pathWayLen;
	return QDS;
}

QDataStream& operator>>(QDataStream& QDS, PathWayData_Less& C)
{
	QDS >> C.signalSpotType >> C.pathWayFre >> C.pathWayLen;
	return QDS;
}

QDataStream& operator<<(QDataStream& QDS, const PathWaySpeed& C)
{
	QDS << C.ID << C.speed << C.len << C.endKm << C.remark;
	return QDS;
}

QDataStream& operator>>(QDataStream& QDS, PathWaySpeed& C)
{
	QDS >> C.ID >> C.speed >> C.len >> C.endKm >> C.remark;
	return QDS;
}

QDataStream& operator<<(QDataStream& QDS, const BaliseLocation& C)
{
	QDS << C.ID << C.baliseName << C.baliseID << \
		C.baliseKm << C.baliseType << C.baliseUse << C.remark_1 << C.remark_2;
	return QDS;
}

QDataStream& operator>>(QDataStream& QDS, BaliseLocation& C)
{
	QDS >> C.ID >> C.baliseName >> C.baliseID >> \
		C.baliseKm >> C.baliseType >> C.baliseUse >> C.remark_1 >> C.remark_2;
	return QDS;
}

QDataStream& operator<<(QDataStream& QDS, const BrokenLink& C)
{
	QDS << C.lineName << C.lineType << C.brokenLinkType << C.brokenLinkPosSta\
		<< C.brokenLinkPosEnd << C.brokenLinkLongLen << C.brokenLinkShortLen;
	return QDS;
}

QDataStream& operator>>(QDataStream& QDS, BrokenLink& C)
{
	QDS >> C.lineName >> C.lineType >> C.brokenLinkType >> C.brokenLinkPosSta\
		>> C.brokenLinkPosEnd >> C.brokenLinkLongLen >> C.brokenLinkShortLen;
	return QDS;
}

QDataStream& operator<<(QDataStream& QDS, const KmInfo& C)
{
	QDS << C.ID << C.beforeLineType << C.beforeKm << \
		C.afterLineType << C.afterType << C.remark;
	return QDS;
}

QDataStream& operator>>(QDataStream& QDS, KmInfo& C)
{
	QDS >> C.ID >> C.beforeLineType >> C.beforeKm >> \
		C.afterLineType >> C.afterType >> C.remark;
	return QDS;
}

QDataStream& operator<<(QDataStream& QDS, const StationSide& C)
{
	QDS << C.ID << C.stationName << C.trackName << C.baliseName << C.stationSide;
	return QDS;
}

QDataStream& operator>>(QDataStream& QDS, StationSide& C)
{
	QDS >> C.ID >> C.stationName >> C.trackName >> C.baliseName >> C.stationSide;
	return QDS;
}