#include "course_model.h"
#include <assert.h>
#include <QSize>
#include <QColor>
#include <QBrush>
#include <QDebug>
#include <QFile>


CourseModel::CourseModel() : QAbstractItemModel()
{
	itemFont = QFont("Helvetica [Cronyx]");
#ifdef Q_OS_WIN32
	markIcons.append(QIcon(":/x.png"));
	markIcons.append(QIcon(":/1.png"));
	markIcons.append(QIcon(":/2.png"));
	markIcons.append(QIcon(":/3.svg"));
	markIcons.append(QIcon(":/4.svg"));
	markIcons.append(QIcon(":/5.svg"));
	markIcons.append(QIcon(":/6.svg"));
	markIcons.append(QIcon(":/7.png"));
	markIcons.append(QIcon(":/8.png"));
	markIcons.append(QIcon(":/9.png"));
	markIcons.append(QIcon(":/10.png"));
	markIcons.append(QIcon(":/m.png"));
#else
	markIcons.append(QIcon(":/x.svg"));
	markIcons.append(QIcon(":/1.svg"));
	markIcons.append(QIcon(":/2.svg"));
	markIcons.append(QIcon(":/3.svg"));
	markIcons.append(QIcon(":/4.svg"));
	markIcons.append(QIcon(":/5.svg"));
	markIcons.append(QIcon(":/6.svg"));
	markIcons.append(QIcon(":/7.svg"));
	markIcons.append(QIcon(":/8.svg"));
	markIcons.append(QIcon(":/9.svg"));
	markIcons.append(QIcon(":/10.svg"));
	markIcons.append(QIcon(":/m.png"));
#endif

	isTeacher = false;
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
	qDebug() << "SET TEST PRG" << id;
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
	QDomNode  node = nodeById(curTaskId, root);
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
	QDomNode  node = nodeById(id, root);
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


int CourseModel::taskMark(QDomNode  node) const
{
	if (node.isNull()) {
		return 0;
	}

	QDomElement readyEl = node.firstChildElement("MARK");
	if (readyEl.isNull()) {
		return 0;
	}
	return readyEl.text().toInt();
}


void CourseModel::setMark(int id, int mark)
{
	QDomNode  node = nodeById(id, root);
	if (node.isNull()) {
		return;
	}

	QDomElement readyEl = node.firstChildElement("MARK");
	if (readyEl.isNull()) {
		QDomElement markEl = courseXml.createElement("MARK");
		node.appendChild(markEl);
		readyEl = node.firstChildElement("MARK");

	}

	QDomText text = courseXml.createTextNode(QString::number(mark));
	for (int i = 0; i < readyEl.childNodes().count(); i++) {
		if (readyEl.childNodes().at(i).isText()) {
			QDomNode elText = readyEl.childNodes().at(i);
			readyEl.replaceChild(text, elText);
			break;
		}
	}
}


bool CourseModel::isTask(int id) const
{
	QDomNode task = nodeById(id, root);
	if (task.toElement().attribute("root") == "true") {
		qDebug() << "Is Node!";
		return false;
	}
	qDebug() << "Is task!";
	return true;
}


void CourseModel::setTask(int id, bool flag)
{
	qDebug() << "setTask!" << flag;
	QDomNode task = nodeById(id, root);
	task.toElement().setAttribute("root", flag ? "true" : "false");
	isTask(id);
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


int CourseModel::addNewTask(int par_id, int sibl_id) //Добавляем раздел перед sibl
{
	QDomNode par = nodeById(par_id, root);
	QDomDocument baseNode;
	baseNode.setContent(QString::fromUtf8("<T root=\"true\" id=\"2\" name=\"Новый раздел\">\n<DESC>Нет Описания</DESC>\n<CS>Кумир</CS>\n<MARK/>\n</T>\n"));
	QDomElement newNode = baseNode.firstChildElement();
	QDomNode copy = newNode.cloneNode(true);
	QDomNode impCopy = courseXml.importNode(copy, true);

	int copyid = getMaxId();

	impCopy.toElement().setAttribute("id", copyid);
	par.toElement().insertBefore(impCopy, nodeById(sibl_id, par));

	cash.clear();
	buildCash();
	emit dataChanged(QModelIndex(), createIndex(rowCount() + 1, 1, copyid));
	return copyid;
}


void CourseModel::moveTask(int new_par_id, int node_id)
{
	cash.clear();
	buildCash();
	qDebug() << "new_par_id" << new_par_id;
	QDomNode task = nodeById(node_id, root);
	QDomNode newPar = nodeById(new_par_id, root);
	QDomNode clone = task.cloneNode(true);
	newPar.insertBefore(clone, newPar.firstChild());
	task.parentNode().removeChild(task);
	cash.clear();
	buildCash();

	emit dataChanged(QModelIndex(), createIndex(rowCount() + 1, 1, node_id));
}


void CourseModel::addDeepTask(int id)
{
	if (id == 0) {
		QDomDocument baseNode;
		baseNode.setContent(QString::fromUtf8("<T xml:id=\"2\" name=\"Новое задание\">\n<DESC>Нет Описания</DESC>\n<CS>Кумир</CS>\n <READY>false</READY>\n</T>\n"));
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
	QDomNode copy = task.cloneNode(false);
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
		QString id = list.at(i).toElement().attribute("id");

		if (id == "") {
			id = list.at(i).toElement().attribute("xml:id");    //Другой вариант написания.
		}

		qDebug() << "Cash" << id;
		cash.insert(id.toInt(), list.at(i));
	}
}


QList<KumTask> CourseModel::childTasks(QModelIndex parent)
{
	QList <KumTask> toRet;
	QDomNode el = nodeById(parent.internalId(), root);
	QDomNodeList childs = el.childNodes();
	for (int i = 0; i < childs.count(); i++) {
		if (childs.at(i).nodeName() == "T") {
			QString id = childs.at(i).toElement().attribute("id");

			if (id == "") {
				id = childs.at(i).toElement().attribute("xml:id");
			}
			toRet.append(KumTask(id.toInt(), childs.at(i).toElement().attribute("name", "")));
		}
	}
	return toRet;
}


int CourseModel::idByNode(QDomNode node) const
{
	bool ok = false;
	int id = node.toElement().attribute("id", "").toInt(&ok);
	if (!ok) {
		return -1;
	}
	return id;
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


int CourseModel::loadCourse(QString file)
{
	courseFileName = file;
	qDebug() << "Load Course";
	QFile f(courseFileName);
	if (!f.open(QIODevice::ReadWrite)) {

		return -1;
	}
	if (f.atEnd()) {
		return -1;
	}

	QString error;
	courseXml.setContent(f.readAll(), true, &error);
	f.close();
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


QIcon CourseModel::iconByMark(int mark, bool isFolder) const
{
	if (isFolder) {
		return QIcon(":/folder.png");
	}
	if ((mark > -1) && (mark < 12)) {
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

	if (role == Qt::DisplayRole)    {
		QString title = node.toElement().attribute("name", "");
		return QVariant(title);
	}

	if (role == Qt::SizeHintRole) {
		return QVariant(QSize(10, 20));
	}

	if (role == Qt::FontRole) {
		return QVariant();
	}

	if (role == Qt::TextAlignmentRole) {
		return QVariant(Qt::AlignLeft);
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
		// qDebug()<<"Draw Mark id"<<index.internalId();
		if (nodeM.toElement().attribute("root") == "true") {
			qDebug() << "Folder";
		}
		return iconByMark(taskMark(index.internalId()), nodeM.toElement().attribute("root") == "true");
		//NUZHNO IKONKI ISPOLNITELEY
	}

	if (role == Qt::BackgroundRole) {
		return QBrush(QColor(255, 255, 255));
	}

	qDebug() << "No" << role << "role";
	return QVariant();
}


QVariant CourseModel::headerData(
	int section,
	Qt::Orientation orientation,
	int role
) const {
	Q_UNUSED(section);
	Q_UNUSED(orientation);
	Q_UNUSED(role);

	return QVariant(courseFileName);
}


QModelIndex CourseModel::index(
	int row, int column,
	const QModelIndex &parent
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


QDomNode CourseModel::nodeByRowColumn(int row, int column, QDomNode *parent) const
{
	Q_UNUSED(column);

	if (!parent) {
		return root;
	}

	qDebug() << "nodeByRowColumn";
	return parent->childNodes().at(row);

	return root;
}


QDomNode CourseModel::nodeById(int id, QDomNode parent) const
{
	if (parent.toElement().attribute("id", "") == QString::number(id)) {
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
		if (childs.at(i).toElement().attribute("id", "") == QString::number(id)) {
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


QModelIndex CourseModel::createMyIndex(int row, int column, QModelIndex parent) const
{
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

	QString string_id = childs.at(row).toElement().attribute("id", "");
	if (string_id == "") {
		string_id = childs.at(row).toElement().attribute("xml:id", "");
	}

	bool ok = false;
	int new_id = string_id.toInt(&ok);
	if (!ok) {

		qDebug() << "Bad ID" << string_id;
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
	QDomNodeList ispsNodes = node.toElement().elementsByTagName("ISP");

	QStringList modules;
	for (int i = 0; i < ispsNodes.count(); i++) {
		QDomElement curEl = ispsNodes.at(i).toElement();
		if ((curEl.attribute("ispname") != "") && (curEl.parentNode() == node)) {
			modules << curEl.attribute("ispname");

			qDebug() << "M" << curEl.toElement().attribute("ispname");
		}
	}
	return modules;
}


void CourseModel::removeModule(int id, QString modName)
{
	QDomNode node = nodeById(id, root);
	QDomElement csEl = node.firstChildElement("ISP");
	qDebug() << "csEl isNull:" << csEl.isNull();

	while (!csEl.isNull()) {
		if (csEl.attribute("ispname") == modName) {
			csEl.parentNode().removeChild(csEl);
			return;
		}

		csEl = csEl.nextSiblingElement("ISP");
	}
}


void CourseModel::addModule(QModelIndex index, QString isp)
{

	QDomNode node = nodeById(index.internalId(), root);
	QDomText text = courseXml.createTextNode(isp);
	qDebug() << "Append ISP" << isp;

	QDomElement ispEl = courseXml.createElement("ISP");
	ispEl.setAttribute("ispname", isp);
	QDomElement csEl = node.firstChildElement("ISP");
	ispEl.appendChild(text);
	if (csEl.isNull()) {

		node.toElement().appendChild(ispEl);
	} else {
		node.insertAfter(ispEl, csEl);
	}
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
		ispEl.setAttribute("ispname", isp.at(i));
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


QStringList CourseModel::Fields(int index, QString isp)
{
	QDomNode node = nodeById(index, root);
	QStringList fields;

	QDomElement csEl = node.firstChildElement("ISP");
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


QDomElement CourseModel::ispNodeByName(QModelIndex task_index, QString ispName)
{
	QDomNode node = nodeById(task_index.internalId(), root);
	QDomElement csEl = node.firstChildElement("ISP");
	while (!csEl.isNull()) {
		if (csEl.attribute("ispname") == ispName) {
			return csEl;
		}
		csEl = csEl.nextSiblingElement("ISP");
	}
	assert (0);
	return csEl;
}


void CourseModel::removeEnv(QDomElement ispElement, QString fieldStr)
{
	QDomElement fieldEl = ispElement.firstChildElement("ENV");
	while (!fieldEl.isNull()) {
		if (fieldEl.text() == fieldStr) {
			ispElement.removeChild(fieldEl);
			return;
		}
		fieldEl = fieldEl.nextSiblingElement("ENV");
	}
}


void CourseModel::addEnv(QDomElement ispElement, QString fieldStr)
{
	QDomElement csEl = ispElement.firstChildElement("ENV");
	if (csEl.isNull()) {
		QDomText text = courseXml.createTextNode(fieldStr);

		QDomElement envEl = courseXml.createElement("ENV");
		ispElement.appendChild(envEl);
		envEl.appendChild(text);
	} else {
		QDomText text = courseXml.createTextNode(fieldStr);

		QDomElement envEl = courseXml.createElement("ENV");
		ispElement.insertAfter(envEl, csEl);
		envEl.appendChild(text);
	}
}

