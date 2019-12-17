#ifndef COURSE_CHANGES_H
#define COURSE_CHANGES_H

#include <QMap>

class courseChanges
{
public:
	courseChanges() {};
	void cleanChanges()
	{
		marksChanged.clear();
	}
	void setMark(int testId, int newMark)
	{
		marksChanged.insert(testId, newMark);
	}
	QMap<int, int> marksChanged;
};

#endif // COURSE_CHANGES_H

