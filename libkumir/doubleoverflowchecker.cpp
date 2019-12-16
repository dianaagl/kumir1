//
// C++ Implementation: doubleoverflowchecker
//
// Description:
//
//
// Author: Виктор Яковлев <V.Yacovlev@gmail.com>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "doubleoverflowchecker.h"
#include <cmath>

#ifndef Q_CC_MSVC

double DoubleOverflowChecker::power2in1023 = 
	89884656743115795386465259539451236680898848947115328636715040578866337902750481566354238661203768010560056939935696678829394884407208311246423715319737062188883946712432742638151109800623047059726541476042502884419075341171231440736956555270413618581675255342293149119973622969239858152417678164812112068608.;
#else

#include <xmath.h>
double DoubleOverflowChecker::power2in1023 = INF;

#endif
bool DoubleOverflowChecker::checkSumm(const double first, const double second)
{
	double u = first / 2.;
	double w = second / 2.;
	return (-power2in1023<=u+w) && (u+w<power2in1023);
}

bool DoubleOverflowChecker::checkDiff(const double first, const double second)
{
	double u = first / 2.;
	double w = (-second) / 2.;
	return (-power2in1023<=u+w) && (u+w<power2in1023);
}

bool DoubleOverflowChecker::checkProd(const double first, const double second)
{
	if ( first>0 && second>0 ) {
		double u = log_2(first);
		double w = log_2(second);
		return u+w<1024;
	}
	else if ( first<0 && second<0 ) {
		double u = log_2(-first);
		double w = log_2(-second);
		return u+w<1024;
	}
	else if ( first>0 && second<0 ) {
		double u = log_2(first);
		double w = log_2(-second);
		return u+w<1024;
	}
	else if ( first<0 && second>0 ) {
		double u = log_2(-first);
		double w = log_2(second);
		return u+w<1024;
	}
	else
		return true;
}

bool DoubleOverflowChecker::checkPower(const double first, const double second)
{
	if ( first==0 || second<=0 )
		return true;
	else if ( first>0 ) {
		double u = log_2(first);
		return u*second < 1024;
	}
	else {
		double u = log_2(-first);
		return u*second < 1024;
	}
}


bool DoubleOverflowChecker::checkFromString(const QString & value)
{
	bool expForm = value.toLower().contains("e");
	bool isNegative = value.startsWith("-");
	QString tmp = value;
	if ( isNegative )
		tmp.remove(0,1);
	if ( value=="inf" )
		return false;
	if ( expForm ) {
		int e_pos = value.toLower().indexOf("e");
		QString mantissa = value.left(e_pos);
		QString exponent = value.mid(e_pos+1);
		int exp_value = exponent.toInt();
//		int power_mantissa = 0;
		int dot_pos = mantissa.indexOf(".");
		if (dot_pos==-1)
			dot_pos = mantissa.length();
		int new_dot_pos = dot_pos + exp_value;
		mantissa.remove(".");
		if ( new_dot_pos < 0 ) {
			while ( new_dot_pos < 0 ) {
				mantissa = "0" + mantissa;
				new_dot_pos ++;
			}
			tmp = "." + mantissa;
		}
		else if ( new_dot_pos > mantissa.length() ) {
			while ( new_dot_pos > mantissa.length() ) {
				mantissa += "0";
// 				new_dot_pos --;
			}
			tmp = mantissa;
		}
		else {
			tmp = mantissa;
			tmp.insert(new_dot_pos,'.');
		}
	}
//	bool ok = true;
//	double val = tmp.toDouble(&ok);
	int dot_pos = tmp.indexOf(".");
	if ( dot_pos==-1 )
		dot_pos = tmp.length();
	if ( dot_pos < 308 )
		return true;
	else if ( dot_pos>308 )
		return false;
	else {
		tmp = tmp.left(308);
		return tmp < QString("89884656743115795386465259539451236680898848947115328636715040578866337902750481566354238661203768010560056939935696678829394884407208311246423715319737062188883946712432742638151109800623047059726541476042502884419075341171231440736956555270413618581675255342293149119973622969239858152417678164812112068608");
	}
	
	return false;
}

double DoubleOverflowChecker::log_2(const double x)
{
	// log_a(b) = log_c(b) / log_c(a)
	return log(x) / log(2.0);
}
