var http_request = false;

function makeRequestToKumir(param, asynch) {
    if (typeof asynch == "undefined")
        asynch = true;
    http_request = false;
    if (window.XMLHttpRequest) { // Мозилла, опера и т.д.
        http_request = new XMLHttpRequest();
        if (http_request.overrideMimeType) {
            // Выставляем тип странички
            http_request.overrideMimeType('text/html');
        }
    } else if (window.ActiveXObject) { // Интернет эксплорер
        try {
            http_request = new ActiveXObject("Msxml2.XMLHTTP");
        } catch (e) {
            try {
                http_request = new ActiveXObject("Microsoft.XMLHTTP");
            } catch (e) {}
        }
    }
    if (!http_request) {
        alert('Cannot create XMLHTTP instance');
        return false;
    }
    http_request.onreadystatechange = alertContents; //Тут написать функцию которая вызовется после получения странички
    http_request.open('POST', 'browser2kumir', asynch);
    http_request.setRequestHeader("Content-type", "application/x-www-form-urlencoded");
    http_request.setRequestHeader("Content-length", param.length);
    http_request.setRequestHeader("Connection", "close");
    http_request.send(param);
    return http_request.responseText;
}

function alertContents() {
    if (http_request.readyState == 4) { // Говорит о том что все загружено
        if (http_request.status == 200) { // Статут загрузки страницы, если 200 то все нормально
            //if (http_request.responseText != '') {
            //    alert(http_request.responseText);
            //}
        } else {
            if (http_request.status == 0) {
                alert('Ошибка соединения. Проверьте, запущен ли Кумир.');
            } else {
                alert('Ошибка: ' + http_request.status + ' ' + http_request.responseText);
            }
        }
    }
}

// -----------------------------------------------------------------------------------

function setProgramText(text) {
    makeRequestToKumir('Текст программы\r\n'+text);
}

function setProgramFile(filename) {
    makeRequestToKumir('setProgramFile\r\n'+filename);
}

function runProgram() {
    makeRequestToKumir('Выполнить');
}

function STEP() {
    makeRequestToKumir('Большой Шаг');
}

function step() {
    makeRequestToKumir('Малый Шаг');
}

function stop() {
    makeRequestToKumir('Stop');
}

function runAlg(name) {
    makeRequestToKumir('Выполнить алгоритм\r\n'+name);
}

function val(module, alg, name) {
    return makeRequestToKumir('Значение\r\n'+module+'\r\n'+alg+'\r\n'+name, false);
}

function checkConnect() {
    makeRequestToKumir('checkConnect', false);
    if (http_request.responseText != '') {
        alert(http_request.responseText);
    }
}

function setRobotField(file) {
    makeRequestToKumir('setRobotField\r\n'+file);
}
