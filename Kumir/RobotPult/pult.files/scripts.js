/* ---------------- ОБЩИЕ СВЕДЕНИЯ
 * 
 * В странице пульта доступен глобальный javascript-объект с именем
 * "pult", который предоставляет следующие методы:
 * 
 *  function evaluateCommand(text: string)
 *  function setWindowSize(w: int, h: int)
 *  function copyToClipboard(data: string)
 * 
 * evaluateCommand - ставит в очередь на выполнение очередную 
 *      КуМир-команду;
 * 
 * setWidowSize - устанавливает размер окна;
 * 
 * copyToClipboard - копирует заданный текст в буфер обмена.
 * 
 * 
 * Кроме того, страница (точнее - скрипт страницы) должна реализовывать
 * следующие функции:
 *  
 *  function onSetLink(link: boolean): void
 *  function onCommandFinished(command: string, status: string) : void
 * 
 * onSetLink - вызывается при изменении наличия связи с КуМиром;
 *      параметр link указывает наличие связи;
 * 
 * onCommandFinished - вызывается при завершении выполнения Роботом
 *      команды; command - текст вызванной команды, status - результат
 *      выполнения команды (если это вопрос), либо статус успешности
 *      ("ОК" или "Отказ").
 * ------------------------------------------------------------------ */


/* Флаг: есть ли связь с КуМиром */
var HAS_LINK = true;
/* Текст КуМир-программы, которая передается
 * по нажатию кнопки "Скопировать в буфер обмена"
 */
var KUMIR_TEXT = "";

/* Добавляет текст в окно логгера */
function addLoggerText(text) {
	var logger_iframe = document.getElementById("logger");
	var logger_document = logger_iframe.contentDocument;
	var logger_table = logger_document.getElementById("logger_table");
	logger_table.innerHTML += text;
	logger_iframe.contentWindow.scrollTo(0, logger_table.clientHeight);
}

/* Добавляет текст к Кумир-программе, если есть связь с Кумиром */
function addKumirText(text) {
    if (HAS_LINK)
        KUMIR_TEXT += text+"\n";
}

/* Обработчик события "Связь установлена/прервана" */
function onSetLink(link) {
    HAS_LINK = link;
    var indicator = document.getElementById("link_indicator");
    indicator.className = link? "has_link" : "no_link";
}

/* Вызов команды пульта при наличии связи */
function evaluateCommand(command) {
    if (HAS_LINK)
        pult.evaluateCommand(command);
    else
        addLoggerText("<tr><td colspan='2'>НЕТ СВЯЗИ</td></tr>");
}

/* Обработчик события "Выполнение команды завершено" */
function onCommandFinished(command, status) {
    addLoggerText("<tr><td>"+command+"</td><td>"+status+"</td></tr>\n");
}

/* Проверка на зажатость некоторых кнопок */
function isPressed(id) {
	var elem = document.getElementById(id+"_toggled");
	var visible = elem.className!="hidden";
	return visible;
}

/* Отжимает зажатые кнопки */
function unpressToggleButtons() {
	document.getElementById("btn_wall_painted").className = "visible";
	document.getElementById("btn_wall_painted_toggled").className = "hidden";
	document.getElementById("btn_free_clean_toggled").className = "hidden";
	document.getElementById("btn_free_clean").className = "visible";
}

/* Очищает окно логгера и кумир-программу */
function btnClearClick() {
	var logger_iframe = document.getElementById("logger");
	logger_iframe.src = logger_iframe.src;
	unpressToggleButtons();
    KUMIR_TEXT = "";
}

/* Копирует текст в буфер обмена */
function btnCopyClick() {
    pult.copyToClipboard(KUMIR_TEXT);
	alert("Текст программы скопирован в буфер обмена");
	unpressToggleButtons();
}

/* Скроллинг вверх */
function btnScrollUpClick() {
	var logger_iframe = document.getElementById("logger");
	var logger_document = logger_iframe.contentDocument;
	var logger_table = logger_document.getElementById("logger_table");
	var w = logger_iframe.contentWindow;
	var h = w.pageYOffset;
	var newH = h - 11;
	if (newH<0)
		newH = 0;
	w.scrollTo(0, newH);
	unpressToggleButtons();
}

/* Скроллинг вниз */
function btnScrollDownClick() {
	var logger_iframe = document.getElementById("logger");
	var logger_document = logger_iframe.contentDocument;
	var logger_table = logger_document.getElementById("logger_table");
	var w = logger_iframe.contentWindow;
	var h = w.pageYOffset;
	var newH = h + 11;
	if (newH>logger_table.clientHeight)
		newH = logger_table.clientHeight;
	w.scrollTo(0, newH);
	unpressToggleButtons();
}

/* Обработка нажатия кнопки "вверх" */
function btnUpClick() {
	if ( isPressed("btn_wall_painted") ) {
        evaluateCommand("сверху стена");
        addKumirText("вывод 'Сверху стена: ', сверху стена, нс");
	}
	else if ( isPressed("btn_free_clean") ) {
        evaluateCommand("сверху свободно");
        addKumirText("вывод 'Сверху свободно: ', сверху свободно, нс");
	}
	else {
		evaluateCommand("вверх");
        addKumirText("вверх");
	}
	unpressToggleButtons();
}

/* Обработка нажатия кнопки "вниз" */
function btnDownClick() {
	if ( isPressed("btn_wall_painted") ) {
		evaluateCommand("снизу стена");
        addKumirText("вывод 'Снизу стена: ', снизу стена, нс");
	}
	else if ( isPressed("btn_free_clean") ) {
		evaluateCommand("снизу свободно");
        addKumirText("вывод 'Снизу свободно: ', снизу свободно, нс");
	}
	else {
		evaluateCommand("вниз");
        addKumirText("вниз");
	}
	unpressToggleButtons();
}

/* Обработка нажатия кнопки "влево" */
function btnLeftClick() {
	if ( isPressed("btn_wall_painted") ) {
		evaluateCommand("слева стена");
        addKumirText("вывод 'Слева стена: ', слева стена, нс");
	}
	else if ( isPressed("btn_free_clean") ) {
		evaluateCommand("слева свободно");
        addKumirText("вывод 'Слева свободно: ', слева свободно, нс");
	}
	else {
		evaluateCommand("влево");
        addKumirText("влево");
	}
	unpressToggleButtons();
}

/* Обработка нажатия кнопки "вправо" */
function btnRightClick() {
	if ( isPressed("btn_wall_painted") ) {
		evaluateCommand("справа стена");
        addKumirText("вывод 'Справа стена: ', справа стена, нс");
	}
	else if ( isPressed("btn_free_clean") ) {
		evaluateCommand("справа свободно");
        addKumirText("вывод 'Справа свободно: ', справа свободно, нс");
	}
	else {
		evaluateCommand("вправо");
        addKumirText("вправо");
	}
	unpressToggleButtons();
}

/* Обработка нажатия центральной кнопки  */
function btnPaintClick() {
	if ( isPressed("btn_wall_painted") ) {
		evaluateCommand("клетка закрашена");
        addKumirText("вывод 'Клетка закрашена: ', клетка закрашена, нс");
	}
	else if ( isPressed("btn_free_clean") ) {
		evaluateCommand("клетка чистая");
        addKumirText("вывод 'Клетка чистая: ', клетка чистая, нс");
	}
	else {
		evaluateCommand("закрасить");
        addKumirText("закрасить");
	}
	unpressToggleButtons();
}

/* Обработка нажатия кнопки "радиация" */
function btnRadiationClick() {
	evaluateCommand("радиация");
    addKumirText("вывод 'радиация: ', радиация, нс");
	unpressToggleButtons();
}

/* Обработка нажатия кнопки "температура" */
function btnTempClick() {
	evaluateCommand("температура");
    addKumirText("вывод 'температура: ', температура, нс");
	unpressToggleButtons();
}

/* Обработка нажатия кнопки "стена/закрашено" */
function btnWallPaintedClick() {
	document.getElementById("btn_wall_painted").className = "hidden";
	document.getElementById("btn_wall_painted_toggled").className = "visible";
	document.getElementById("btn_free_clean_toggled").className = "hidden";
	document.getElementById("btn_free_clean").className = "visible";
}

/* Обработка отжатия кнопки "стена/закрашено" */
function btnWallPaintedUnClick() {
	document.getElementById("btn_wall_painted").className = "visible";
	document.getElementById("btn_wall_painted_toggled").className = "hidden";
}

/* Обработка нажатия кнопки "свободно/чисто" */
function btnFreeCleanClick() {
	document.getElementById("btn_wall_painted").className = "visible";
	document.getElementById("btn_wall_painted_toggled").className = "hidden";
	document.getElementById("btn_free_clean_toggled").className = "visible";
	document.getElementById("btn_free_clean").className = "hidden";
}

/* Обработка отжатия кнопки "свободно/чисто" */
function btnFreeCleanUnClick() {
	document.getElementById("btn_free_clean_toggled").className = "hidden";
	document.getElementById("btn_free_clean").className = "visible";
}

var timerId = null;
var timerRunning = false;
var hintDivId = "";

function showHintWorker() {
    document.getElementById(hintDivId).className = "visible";
    timerRunning = false;
    timerId = null;
}


function showHint(id) {
    if (!timerRunning) {
        hintDivId = id;
        timerId = setTimeout("showHintWorker()", 1000);
        timerTunning = true;
    }
}

function hideHint(id) {
    document.getElementById(id).className = "hidden";
    hintDivId = "";
    timerId = null;
    timerRunning = false;
}