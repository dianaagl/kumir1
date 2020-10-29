#include "course_model.h"

#include <QSize>
#include <QColor>
#include <QBrush>
#include <QDebug>
#include <QFile>

#define MARK_BLOCK 12

CourseModel::CourseModel() : QAbstractItemModel()
{
	isTeacher = false;
	itemFont = QFont("Helvetica [Cronyx]");
#ifdef Q_OS_WIN32
	markIcons.append(QIcon(":/out_stand.png"));
	markIcons.append(QIcon(":/1.png"));
	markIcons.append(QIcon(":/2.png"));
	markIcons.append(QIcon(":/3.png"));
	markIcons.append(QIcon(":/4.png"));
	markIcons.append(QIcon(":/5.png"));
	markIcons.append(QIcon(":/6.png"));
	markIcons.append(QIcon(":/7.png"));
	markIcons.append(QIcon(":/8.png"));
	markIcons.append(QIcon(":/9.png"));
	markIcons.append(QIcon(":/10.png"));
	markIcons.append(QIcon(":/m.png"));

	markIcons.append(QIcon(":/folder_close.png"));
	markIcons.append(QIcon(":/folder_1.png"));
	markIcons.append(QIcon(":/folder_2.png"));
	markIcons.append(QIcon(":/folder_3.png"));
	markIcons.append(QIcon(":/folder_4.png"));
	markIcons.append(QIcon(":/folder_5.png"));
	markIcons.append(QIcon(":/folder_6.png"));
	markIcons.append(QIcon(":/folder_7.png"));
	markIcons.append(QIcon(":/folder_8.png"));
	markIcons.append(QIcon(":/folder_9.png"));
	markIcons.append(QIcon(":/folder_10.png"));
	markIcons.append(QIcon(":/folder_open.png"));
#else
	markIcons.append(QIcon(":/out_stand.svg"));
	markIcons.append(QIcon(":/1.png"));
	markIcons.append(QIcon(":/2.png"));
	markIcons.append(QIcon(":/3.png"));
	markIcons.append(QIcon(":/4.png"));
	markIcons.append(QIcon(":/5.png"));
	markIcons.append(QIcon(":/6.png"));
	markIcons.append(QIcon(":/7.png"));
	markIcons.append(QIcon(":/8.png"));
	markIcons.append(QIcon(":/9.png"));
	markIcons.append(QIcon(":/10.png"));
	markIcons.append(QIcon(":/m.png"));

	markIcons.append(QIcon(":/folder_close.svg"));
	markIcons.append(QIcon(":/folder_1.png  "));
	markIcons.append(QIcon(":/folder_2.svg"));
	markIcons.append(QIcon(":/folder_3.svg"));
	markIcons.append(QIcon(":/folder_4.svg"));
	markIcons.append(QIcon(":/folder_5.svg"));
	markIcons.append(QIcon(":/folder_6.svg"));
	markIcons.append(QIcon(":/folder_7.svg"));
	markIcons.append(QIcon(":/folder_8.svg"));
	markIcons.append(QIcon(":/folder_9.svg"));
	markIcons.append(QIcon(":/folder_10.svg"));
	markIcons.append(QIcon(":/folder_open.svg"));
#endif
}

int CourseModel::loadCourse(QString file)
{
	courseFileName = file;
	qDebug() << "Load Course";
	QFile f(courseFileName);
	if (!f.open(QIODevice::ReadOnly)) {
		qDebug() << "cant open" << courseFileName;
		return -1;
	};
	if (f.atEnd()) {
		return -1;
	}
	QString error;
	courseXml.setContent(f.readAll(), true, &error);
	qDebug() << "File parce:" << error;
	int count = 0;
	taskCount = count;
	root = courseXml.documentElement();
	insertRow(0);
	insertColumn(0);
	setData(createIndex(0, 0), QVariant());
	buildCash();

	return count;
}


int CourseModel::rowCount(const QModelIndex &parent) const
{
	if (!parent.isValid()) {
		return 1;
	}

	QDomNode node = nodeById(parent.internalId(), root);
	return subTasks(node);
}


QIcon CourseModel::iconByMark(int mark, bool isFolder)const
{
	if (isFolder) {
		mark = mark + MARK_BLOCK;
	}
	if ((0 <= mark) && (mark < 2 * MARK_BLOCK)) {
		return markIcons[mark];
	}

	return QIcon(":/x.png");
}


QVariant CourseModel::data(const QModelIndex &index, int role) const
{
	if (!index.isValid()) {
		return QVariant();
	}

	QDomNode node = nodeById(index.internalId(), root);

	if (role == Qt::DisplayRole) {
		QString title = node.toElement().attribute("name", "");
		return QVariant(title);
	}

	if (role == Qt::SizeHintRole) {
		QDomNode nodeM = nodeById(index.internalId(), root);
		if (nodeM.toElement().attribute("root") == "true") {
			return QVariant(QSize(30, 30));
		}

		return QVariant(QSize(30, 30));
	}

	if (role == Qt::FontRole) {
		return QVariant();
	}

	if (role == Qt::TextAlignmentRole) {
		return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
	}

	if (role == Qt::ForegroundRole) {
		if (isTeacher) {
			QVariant(QBrush(QColor(0, 0, 0)));
		}
		if (!taskAvailable(node)) {
			return QVariant(QBrush(QColor(150, 150, 150)));
		}
		return QVariant(QBrush(QColor(0, 0, 0)));
	}

	if (role == Qt::CheckStateRole) {
		return QVariant();
	}

	if (role == Qt::DecorationRole) {
		QDomNode nodeM = nodeById(index.internalId(), root);
		if (nodeM.toElement().attribute("root") == "true") {
			qDebug() << "Folder";
		}
		return iconByMark(taskMark(index.internalId()), nodeM.toElement().attribute("root") == "true");
	}

	if (role == Qt::BackgroundRole) {
		return QBrush(QColor(255, 255, 255));
	}

	qDebug() << "No " << role << " role";
	return QVariant();
}


QVariant CourseModel::headerData(
	int section, Qt::Orientation orientation, int role
) const {
	Q_UNUSED(section);
	Q_UNUSED(orientation);
	Q_UNUSED(role);
	return QVariant(courseFileName);
}


QModelIndex CourseModel::index(
	int row, int column, const QModelIndex &parent
) const {
	if (!hasIndex(row, column, parent)) {
		return QModelIndex();
	}
	return createMyIndex(row, column, parent);
}


int CourseModel::domRow(QDomNode &child) const //TODO Check
{
	QDomNode par = child.parentNode();
	QDomNodeList list = par.childNodes();
	for (int i = 0; i < list.count(); i++) {
		if (child == list.at(i)) {
			return i;
		}
	}
	return 0;
}


QModelIndex CourseModel::parent(const QModelIndex &child) const
{

	if (!child.isValid()) {
		return QModelIndex();
	}
	if (child.internalId() == 0) {
		return QModelIndex();
	}
	QDomNode child_n = nodeById(child.internalId(), root);
	QDomNode par = child_n.parentNode();
	if (par.toElement().attribute("id").toInt() == 0) {
        return createIndex(0, 0);
	}
	return createIndex(domRow(par), 0, idByNode(par));
}


int CourseModel::columnCount(const QModelIndex &parent) const
{
	Q_UNUSED(parent);
	return 1;
}


QDomNode CourseModel::nodeByRowColumn(
	int row, int column, QDomNode *parent
) const {
	Q_UNUSED(column);
	if (!parent) {
		return root;
	}
	qDebug() << "nodeByRowColumn";
	return parent->childNodes().at(row);
}


QDomNode CourseModel::nodeById(int id, QDomNode parent) const
{
	QString sid = QString::number(id);
	if (parent.toElement().attribute("id", "") == sid) {
		return parent;
	}

	if (!parent.hasChildNodes()) {
		return QDomNode();
	}

	QDomNode val = cash.value(id);
	if (!val.isNull()) {
		return val;
	}

	QDomNodeList childs = parent.childNodes();
	for (int i = 0; i < childs.size(); i++) {
		if (childs.at(i).toElement().attribute("id", "") == sid) {
			return childs.at(i);
		}
	}

	for (int i = 0; i < childs.size(); i++) {
		if (childs.at(i).hasChildNodes()) {
			QDomNode toret = nodeById(id, childs.at(i));
			if (!toret.isNull()) {
				return toret;
			}
		}
	}

	return QDomNode();
}


Qt::ItemFlags CourseModel::flags(const QModelIndex &index) const
{
	if (!index.isValid()) {
		return 0;
	}
	if (isTeacher) {
		return  Qt::ItemIsEnabled | Qt::ItemIsSelectable | Qt::ItemIsDragEnabled;
	}

	if (!taskAvailable(index.internalId())) {
		return Qt::NoItemFlags;
	}
	return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}


QModelIndex CourseModel::createMyIndex(
	int row, int column, QModelIndex parent
) const {
	if (!parent.isValid()) {
        return createIndex(0, 0);
	}
	int id = parent.internalId();
	if (id < 0) {
		return QModelIndex();
	}
	QDomNode par = nodeById(id, root);
	if (par.isNull()) {
		return QModelIndex();
	}
	QDomNodeList childs = par.childNodes();
	if (childs.count() <= row) {
		return QModelIndex();
	}
	bool ok = false;
	int new_id = childs.at(row).toElement().attribute("id", "").toInt(&ok);
	if (!ok) {
		qDebug() << "Bad ID";
		return createIndex(-10, -10, -10);
	}
	return createIndex(row, column, new_id);
}

QString CourseModel::getTaskText(QModelIndex index)
{
	if (!index.isValid()) {
		return "INDEX NOT VALID";
	}
	QDomNode node = nodeById(index.internalId(), root);
	QDomElement titleEl = node.firstChildElement("DESC");
	if (titleEl.isNull()) {
		return "";
	}

	return titleEl.text();
}

QString CourseModel::getTaskCheck(QModelIndex index)
{
	if (!index.isValid()) {
		return "INDEX NOT VALID";
	}
	QDomNode node = nodeById(index.internalId(), root);
	QDomElement titleEl = node.firstChildElement("CHECK");
	if (titleEl.isNull()) {
		return "";
	}

	return titleEl.text();
}

QString CourseModel::csName(int index)
{
	QDomNode node = nodeById(index, root);
	QDomElement csEl = node.firstChildElement("CS");
	if (csEl.isNull()) {
		return "NO CS";
	}

	return csEl.text();
}

QString CourseModel::progFile(int index)
{
	QDomNode node = nodeById(index, root);
	QDomElement csEl = node.firstChildElement("PROGRAM");
	if (csEl.isNull()) {
		return "";
	}
	return csEl.text();
}

QStringList CourseModel::Modules(int index)
{
	QDomNode node = nodeById(index, root);
	QDomElement csEl = node.firstChildElement("ISP");

	QStringList modules;
	while (!csEl.isNull()) {
		modules << csEl.attribute("ispname");
		csEl = csEl.nextSiblingElement("ISP");
	}
	return modules;
}

void CourseModel::setIsps(QModelIndex index, QStringList isp)
{
	QDomNode node = nodeById(index.internalId(), root);
	QDomElement csEl = node.firstChildElement("ISP");
	while (!csEl.isNull()) {
		node.removeChild(csEl);
		csEl = node.firstChildElement("ISP");
	}

	for (int i = 0; i < isp.count(); i++) {
		if (isp.at(i) == "") {
			continue;
		}
		QDomText text = courseXml.createTextNode(isp.at(i));
		qDebug() << "Append ISP" << isp.at(i);
		QDomElement ispEl = courseXml.createElement("ISP");
		ispEl.setAttribute("xml:ispname", isp.at(i));
		node.toElement().appendChild(ispEl);
	}
}

void CourseModel::setIspEnvs(QModelIndex index, QString isp, QStringList Envs)
{
	QDomNode node = nodeById(index.internalId(), root);
	QDomElement csEl = node.firstChildElement("ISP");
	while (!csEl.isNull()) {
		if (csEl.attribute("ispname") == isp) {
			QDomNodeList childList = csEl.childNodes();
			for (int j = 0; j < childList.count(); j++) {
				if (childList.at(j).nodeName() == "ENV") {
					csEl.removeChild(childList.at(j));
				}
			}

			for (int i = 0; i < Envs.count(); i++) {
				QDomText text = courseXml.createTextNode(Envs.at(i));
				qDebug() << "Append Env" << Envs.at(i);
				QDomElement envEl = courseXml.createElement("ENV");
				csEl.toElement().appendChild(envEl);
				envEl.appendChild(text);
			}
		}
		csEl = csEl.nextSiblingElement("ISP");
	}
}

void CourseModel::setUserText(QModelIndex index, const QString &text)
{
	QDomNode el = nodeById(index.internalId(), root);

	QDomElement userTextEl = el.firstChildElement("USER_PRG");
	if (userTextEl.isNull()) { //USER PRG пока нет - создаем
		userTextEl = courseXml.createElement("USER_PRG");
		el.appendChild(userTextEl);
	}
	QDomAttr userPrg = courseXml.createAttribute("prg");
	userPrg.setValue(text);
	userTextEl.setAttributeNode(userPrg);
	qDebug() << "SET USER PRG" << index.internalId() << " test " << text;
}

void CourseModel::setUserText(int id, const QString &text)
{
	QDomNode el = nodeById(id, root);

	QDomElement userTextEl = el.firstChildElement("USER_PRG");
	if (userTextEl.isNull()) { //USER PRG пока нет - создаем
		userTextEl = courseXml.createElement("USER_PRG");
		el.appendChild(userTextEl);
	}
	QDomAttr userPrg = courseXml.createAttribute("prg");
	userPrg.setValue(text);
	userTextEl.setAttributeNode(userPrg);
	qDebug() << "SET USER PRG" << id << " " << text;
}

void CourseModel::setUserTestedText(int id, const QString &text)
{
	QDomNode el = nodeById(id, root);
	QDomElement userTextEl = el.firstChildElement("TESTED_PRG");
	if (userTextEl.isNull()) { //USER PRG пока нет - создаем
		userTextEl = courseXml.createElement("TESTED_PRG");
		el.appendChild(userTextEl);
	}
	QDomAttr userPrg = courseXml.createAttribute("prg");
	userPrg.setValue(text);
	userTextEl.setAttributeNode(userPrg);
	qDebug() << "SET TESTED PRG" << id;
}

QString CourseModel::getUserText(int curTaskId)
{
	QDomNode  node = nodeById(curTaskId, root);
	QDomElement userTextEl = node.firstChildElement("USER_PRG");
	if (userTextEl.isNull()) {
		qDebug() << "Null user Prg" << curTaskId;
		return "";
	}
	QString userPrg = userTextEl.attribute("prg");
	return userPrg;
}

QString CourseModel::getUserTestedText(int curTaskId)
{
	QDomNode  node = nodeById(curTaskId, root);
	QDomElement userTextEl = node.firstChildElement("TESTED_PRG");
	if (userTextEl.isNull()) {
		qDebug() << "Null user  tested Prg" << curTaskId;
		return "";
	}
	QString userPrg = userTextEl.attribute("prg");
	return userPrg;
}


void CourseModel::setTag(int curTaskId, QString data, QString tag)
{
	QDomNode node = nodeById(curTaskId, root);
	if (node.isNull()) {
		qDebug() << "Set NODE NO NODE";
		return;
	}

	QDomElement readyEl = node.firstChildElement(tag);
	if (readyEl.isNull()) {
		qDebug() << "Create NEW" << tag;
		QDomElement markEl = courseXml.createElement(tag);
		node.appendChild(markEl);
		readyEl = node.firstChildElement(tag);
	}

	QDomText text = courseXml.createTextNode(data);
	for (int i = 0; i < readyEl.childNodes().count(); i++) {
		qDebug() << "SWAP" << tag;
		if (readyEl.childNodes().at(i).isText()) {
			QDomNode elText = readyEl.childNodes().at(i);
			readyEl.replaceChild(text, elText);
			break;
		}
	}

	if (readyEl.childNodes().count() == 0) {
		readyEl.appendChild(text);
	}
}


QModelIndex CourseModel::getIndexById(int id)
{
	QDomNode node = nodeById(id, root);
	if (node.isNull()) {
		return index(0, 0, QModelIndex());
	}
	int col = node.columnNumber();
	int row = node.lineNumber();
	if (id != 0) {
		return index(row, col, getIndexById(idByNode(node.parentNode())));
	} else {
		return index(row, col, QModelIndex());
	}
}


QStringList CourseModel::Fields(int index, QString isp)
{
	QDomNode node = nodeById(index, root);
	QDomElement csEl = node.firstChildElement("ISP");
	QStringList fields;

	while (!csEl.isNull()) {
		if (csEl.attribute("ispname") == isp) {
			QDomElement fieldEl = csEl.firstChildElement("ENV");
			while (!fieldEl.isNull()) {
				fields.append(fieldEl.text());
				fieldEl = fieldEl.nextSiblingElement("ENV");
			}
			return fields;
		}
		csEl = csEl.nextSiblingElement("ISP");
	}
	return fields;
}


int CourseModel::taskMark(QDomNode node) const
{
	if (node.isNull()) {
		return 0;
	}

	QDomElement readyEl = node.firstChildElement("MARK");

	if (readyEl.isNull()) {
		qDebug() << "Node is null;";
		return 0;
	}
	qDebug() << "MRK:" << readyEl.text();
	return readyEl.text().toInt();
}


void CourseModel::setParMark(QDomElement pnode)
{
	QDomNodeList childs = pnode.elementsByTagName("T");
	int min_m = 11;
	bool hasNull = false;
	for (int i = 0; i < childs.count(); i++) {
		int tmark = taskMark(childs.at(i));
		if (tmark == 0) {
			hasNull = true;
		}
		if (min_m > taskMark(childs.at(i)) && tmark > 0) {
			min_m = taskMark(childs.at(i));
		}
	}
	if (min_m < 11 and hasNull) {
		min_m = 11;
	}
	if (min_m > 0) {
		setMark(idByNode(pnode), min_m);
	}
}


void CourseModel::setMark(int id, int mark)
{
	QDomNode  node = nodeById(id, root);
	if (node.isNull()) {
		return;
	}

	QDomElement readyEl = node.firstChildElement("MARK");
	QDomText text = courseXml.createTextNode(QString::number(mark));
	if (readyEl.isNull()) {
		QDomElement markEl = courseXml.createElement("MARK");
		node.appendChild(markEl);
		readyEl = node.firstChildElement("MARK");
		readyEl.appendChild(text);
		qDebug() << "Is null";
		setParMark(node.parentNode().toElement());
		return;
	}

	for (int i = 0; i < readyEl.childNodes().count(); i++) {
		if (readyEl.childNodes().at(i).isText()) {
			QDomNode elText = readyEl.childNodes().at(i);
			readyEl.replaceChild(text, elText);
			break;
		}
	}

	if (readyEl.childNodes().count() == 0) {
		readyEl.appendChild(text);
	}

	qDebug() << "Task " << id << " mark" << taskMark(node) << " set" << mark;
	setParMark(node.parentNode().toElement());
}


QString CourseModel::Script(int index, QString isp)
{
	QDomNode node = nodeById(index, root);
	QDomElement csEl = node.firstChildElement("ISP");

	while (!csEl.isNull()) {
		if (csEl.attribute("ispname", "") == isp) {
			QDomElement fieldEl = csEl.firstChildElement("CONTROL");
			qDebug() << "Script name from XML:" << fieldEl.text();
			return fieldEl.text();
		}
		csEl = csEl.nextSiblingElement("ISP");
	}
	return "";
}


bool CourseModel::isTask(int id) const
{
	QDomNode task = nodeById(id, root);
	return (task.toElement().attribute("root") != "true");
}


int CourseModel::getMaxId()
{
	int max = 0;
	QDomNodeList list = courseXml.elementsByTagName("T");
	for (int i = 0; i < list.count(); i++) {

		int depId = list.at(i).toElement().attribute("id", "").toInt();
		if (depId > max) {
			max = depId;
		}
	}

	return max + 10;
}


int CourseModel::setChildsId(QDomNode par, int first_id)
{
	QDomNodeList Childs = par.childNodes();
	int cur_off = 0;
	for (int i = 0; i < Childs.count(); i++) {
		cur_off++;
		if (Childs.at(i).nodeName() == "T") {
			Childs.at(i).toElement().setAttribute("id", first_id + cur_off);
			QDomNodeList dep_list = Childs.at(i).toElement().elementsByTagName("DEPEND");
			qDebug() << "Dep count" << dep_list.count();
			Childs.at(i).toElement().removeChild(Childs.at(i).firstChildElement("DEPEND"));
			for (int j = 0; j < dep_list.count(); j++) {

				qDebug() << "dep remove";
			}

			cur_off = cur_off + setChildsId(Childs.at(i), cur_off + 1);
		}
	}
	return cur_off;
}


void CourseModel::addSiblingTask(int id)
{
	QDomNode task = nodeById(id, root);
	QDomNode copy = task.cloneNode();
	int copyid = getMaxId();
	copy.toElement().setAttribute("id", copyid);
	setChildsId(copy, copyid + 1);

	task.parentNode().toElement().insertAfter(copy, task);

	setMark(copyid, 0);
	cash.clear();
}


void CourseModel::addDeepTask(int id)
{
	if (id == 0) {
		QDomDocument baseNode;
		baseNode.setContent(QString::fromUtf8("<T xml:id=\"2\" xml:name=\"Новое задание\">\n<DESC>Нет Описания</DESC>\n<CS>Кумир</CS>\n <ISP xml:ispname=\"Robot\">\n</ISP>\n<READY>false</READY>\n</T>\n"));
		QDomElement newNode = baseNode.firstChildElement();
		QDomNode copy = newNode.cloneNode(true);
		QDomNode impCopy = courseXml.importNode(copy, true);
		int copyid = getMaxId();

		impCopy.toElement().setAttribute("xml:id", copyid);


		root.toElement().insertAfter(impCopy, root.lastChild());
		setMark(copyid, 0);

		cash.clear();
		buildCash();
		emit dataChanged(QModelIndex(), createIndex(rowCount() + 1, 1, copyid));
		return;
	}

	QDomNode task = nodeById(id, root);
	QDomNode copy = task.cloneNode(true);
	QDomNodeList taskChilds = task.childNodes();
	int copyid = getMaxId();

	copy.toElement().setAttribute("id", copyid);

	for (int i = 0; i < taskChilds.count(); i++) {
		QDomNode child = task.childNodes().item(i);
		QDomNode chCopy = child.cloneNode();
		if (chCopy.nodeName() != "T") {
			copy.toElement().appendChild(chCopy);
		}
	}
	task.toElement().insertBefore(copy, task.firstChild());
	setMark(copyid, 0);
	cash.clear();
	buildCash();
}


void CourseModel::removeNode(int id)
{
	QDomNode task = nodeById(id, root);
	task.parentNode().removeChild(task);
	cash.clear();
	buildCash();
}


bool CourseModel::taskAvailable(QDomNode task) const
{
	if (task.isNull()) {
		return false;
	}
	QDomElement depEl = task.firstChildElement("DEPEND");
	while (!depEl.isNull()) {
		QDomElement idEl = depEl.firstChildElement("ID");
		QDomElement markEl = depEl.firstChildElement("MIN");
		if (idEl.isNull() || markEl.isNull()) {
			depEl = depEl.nextSiblingElement("DEPEND");
			continue;
		}
		int depId = idEl.text().toInt();
		QDomNode depNode = nodeById(depId, root); //Узел от которого зависим

		int needMark = markEl.text().toInt();
		if (taskMark(depNode) < needMark) {
			return false;
		}
		depEl = depEl.nextSiblingElement("DEPEND");
	}
	return true;
}


QModelIndex CourseModel::moveUp(QModelIndex &index)
{
	if (!hasUpSib(index)) {
		return index;
	}
	QDomNode el = nodeById(index.internalId(), root);
	QDomNode per = el.previousSiblingElement("T");
	el.parentNode().toElement().insertBefore(el, per);
	cash.clear();
	buildCash();
	return createMyIndex(index.row() - 1, index.column(), index.parent());
}


QModelIndex CourseModel::moveDown(QModelIndex &index)
{
	if (!hasDownSib(index)) {
		return index;
	}
	QDomNode el = nodeById(index.internalId(), root);
	QDomNode per = el.nextSiblingElement("T");
	el.parentNode().toElement().insertAfter(el, per);
	cash.clear();
	buildCash();
	return createMyIndex(index.row() + 1, index.column(), index.parent());
}


void CourseModel::buildCash()
{
	QDomNodeList list = courseXml.elementsByTagName("T");
	for (int i = 0; i < list.count(); i++) {
		cash.insert(list.at(i).toElement().attribute("id").toInt(), list.at(i));
	}
}


int CourseModel::idByNode(QDomNode node) const
{
	bool ok = false;
	int id = node.toElement().attribute("id", "").toInt(&ok);
	return ok ? id : -1;
}


int CourseModel::subTasks(QDomNode parent) const
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

