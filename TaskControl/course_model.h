#ifndef COURSE_MODEL_H
#define COURSE_MODEL_H

#include <QAbstractItemModel>
#include <QStringList>
#include <QFont>
#include <QDomDocument>
#include <QIcon>
#include <QHash>

class courseModel: public QAbstractItemModel
{
	Q_OBJECT

public:

	courseModel();

	int rowCount(const QModelIndex &parent = QModelIndex()) const;
	QVariant data(const QModelIndex &index, int role) const;
	QVariant headerData(
		int section, Qt::Orientation orientation, int role = Qt::DisplayRole
	) const;
	QModelIndex index(int row, int column, const QModelIndex &parent) const;
	QModelIndex parent(const QModelIndex &child) const;
	int columnCount(const QModelIndex &parent) const;
	Qt::ItemFlags flags(const QModelIndex &index) const;

public:
	int loadCourse(QString fileName);
	QString getTaskText(QModelIndex index);
	QString getTaskCheck(QModelIndex index);
	QString courceDescr()
	{
		QDomElement titleEl = root.firstChildElement("DESC");
		if (titleEl.isNull()) {
			return "";
		}

		return titleEl.text();
	}

	QModelIndexList indexes()
	{
		return persistentIndexList();
	}
	void setIspEnvs(QModelIndex index, QString isp, QStringList Envs);
	void setIsps(QModelIndex index, QStringList isp);

	void setUserText(QModelIndex index, const QString &text);
	void setUserText(int id, const QString &text);
	void setUserTestedText(int id, const QString &text);

	QString getUserText(int curTaskId);
	QString getUserTestedText(int curTaskId);

	QString getTitle(int curTaskId)
	{
		QDomNode  node = nodeById(curTaskId, root);
		return node.toElement().attribute("name", "");
	}

	QString name()
	{
		return root.toElement().attribute("name", "");
	}


	void setTitle(int curTaskId, QString title)
	{
		QDomNode el = nodeById(curTaskId, root);
		el.toElement().setAttribute("name", title);
	}

	void setTag(int curTaskId, QString data, QString tag);

	void setDesc(int curTaskId, QString desc)
	{
		setTag(curTaskId, desc, "DESC");
	}

	void setProgram(int curTaskId, QString prg)
	{
		setTag(curTaskId, prg, "PROGRAM");
	}

	QModelIndex getIndexById(int id);

	QString csName(int index);
	QString progFile(int index);
	QStringList Modules(int index);
	QString Script(int index, QString isp);
	QStringList Fields(int index, QString isp);

	int taskMark(int id) const
	{
		return taskMark(nodeById(id, root));
	}

	int taskMark(QDomNode node) const;
	void setParMark(QDomElement  pnode);
	void setMark(int id, int mark);

	QStringList getScripts(int id);
	bool isTask(int id) const
	{
		QDomNode task = nodeById(id, root);
		return (task.toElement().attribute("root") != "true");
	}

	int getMaxId();
	int setChildsId(QDomNode par, int first_id);

	void addSiblingTask(int id);
	void addDeepTask(int id);

	void removeNode(int id)
	{
		QDomNode task = nodeById(id, root);
		task.parentNode().removeChild(task);
		cash.clear();
		buildCash();
	}

	bool  taskAvailable(int id) const
	{
		return taskAvailable(nodeById(id, root));
	}

	bool taskAvailable(QDomNode task) const;

	QDomDocument *document()
	{
		return &courseXml;
	}

	void setTeacher(bool flag)
	{
		isTeacher = flag;
	}

	bool hasUpSib(QModelIndex &index)
	{
		QDomNode el = nodeById(index.internalId(), root);
		return !el.previousSiblingElement("T").isNull();
	}

	bool hasDownSib(QModelIndex &index)
	{
		QDomNode el = nodeById(index.internalId(), root);
		return !el.nextSiblingElement("T").isNull();
	}

	QModelIndex moveUp(QModelIndex &index);
	QModelIndex moveDown(QModelIndex &index);

	QString rootText()
	{
		return root.attribute("name");
	}

	void setRootText(QString text)
	{
		root.setAttribute("name", text);
	}

	void buildCash()
	{
		QDomNodeList list = courseXml.elementsByTagName("T");
		for (int i = 0; i < list.count(); i++) {
			cash.insert(list.at(i).toElement().attribute("id").toInt(), list.at(i));
		}
	}

private:
	QIcon iconByMark(int mark, bool isFolder) const;
	QDomNode nodeByRowColumn(int row, int column, QDomNode *parent) const;
	QDomNode nodeById(int id, QDomNode parent) const;
	QModelIndex createMyIndex(int row, int column, QModelIndex parent) const;

	int idByNode(QDomNode node) const
	{
		bool ok = false;
		int id = node.toElement().attribute("id", "").toInt(&ok);
		return ok ? id : -1;
	}

	int subTasks(QDomNode parent) const
	{
		QDomNodeList childs = parent.childNodes();
		int count = 0;
		for (int i = 0; i < childs.count(); i++) {
			if (childs.at(i).nodeName() == "T") {
				count++;
			}
		}
		return count;
	}

	int domRow(QDomNode &child) const;

	bool isTeacher;
	int taskCount;
	QString courseFileName;
	QString courseName;
	QString courseDesc;
	QFont itemFont;
	QDomDocument courseXml;
	QDomElement root;
	QList<QIcon> markIcons;
	QHash<int, QDomNode> cash;
};

#endif // COURSE_MODEL_H
