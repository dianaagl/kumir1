var isInModule = false;
var elseStack = [];
var loopStack = [];
var switchIfStack = []
var algBegin = -1;
var nado = [];
var mainDefined = false;
var noIndentAfterSwitch = [];
var dano = -1;
var moduleBegin = -1;

var IF = 1;
var THEN = 2;
var SWITCH = 3;
var CASE = 4;
var LOOP = 5;
var BEGIN = 6;
var MODULE = 7;
var DANO = 8;
var NADO = 9;
var DOC = 10;
var FI = 11;
var LOOPEND = 12;
var LOOPCONDEND = 13;
var END = 14;
var ELSE = 15;

var globalObject = this;

function processCorrectEndOfLoop() {
	var cp = globalObject.currentPosition; 
	globalObject.setIndentRank(cp, -1, 0);
	var cpm = globalObject.currentModuleSize;
	globalObject.addToCurrentModule(cp); 
	if ( loopStack.length > 0 ) {
		var lb = loopStack.pop();
		globalObject.setJump(lb,cpm); 
	}
}

function processAlgEndInsteadOfLoopEnd() {
	var cp = globalObject.currentPosition;
	globalObject.setIndentRank(cp, -1, 0);
	globalObject.setError(cp, 2200903);
	var cpm = globalObject.currentModuleSize;
	globalObject.addToCurrentModule(cp);
	globalObject.changeInstructionType(cpm, "кц");
	if ( loopStack.length > 0 ) {
		var lb = loopStack.pop();
		globalObject.setJump(lb,cpm); 
	}
}

function processCorrectCase() {
	var cp = globalObject.currentPosition; 
	var cpm = globalObject.currentModuleSize;
	globalObject.setIndentRank(cp,-1,+1);
	globalObject.addToCurrentModule(cp); 
	if ( elseStack.length > 0 ) {
		var sb = elseStack.pop(); 
		globalObject.setJump(sb,cpm); 
	}
	elseStack.push(cpm);
}

function processCorrectIf() {
	var cp = globalObject.currentPosition; 
	var cpm = globalObject.currentModuleSize;
	globalObject.setIndentRank(cp, 0, +2 );
	globalObject.addToCurrentModule(cp); 
	elseStack.push(cpm);
	switchIfStack.push(IF);
}

function processCorrectThen() {
	var cp = globalObject.currentPosition;
	globalObject.addToCurrentModule(cp);
	globalObject.setIndentRank(cp, -1, +1);
}

function processCorrectFi() {
	var cp = globalObject.currentPosition; 
	var cpm = globalObject.currentModuleSize;
	globalObject.setIndentRank(cp,-2,0);
	if ( switchIfStack.length > 0 ) {
		var last = switchIfStack.pop();
	}
	else {
		globalObject.setIndentRank(cp,0,0);
		globalObject.setError(cp,1175);
	}
	globalObject.addToCurrentModule(cp); 
	if ( elseStack.length > 0 ) {
		var sb = elseStack.pop(); 
		globalObject.setJump(sb,cpm); 
	}
}

function processCorrectElse() {
	var cp = globalObject.currentPosition; 
	var cpm = globalObject.currentModuleSize;
	globalObject.setIndentRank(cp,-1,+1);
	globalObject.addToCurrentModule(cp); 
	if ( elseStack.length > 0 ) {
		var sb = elseStack.pop(); 
		globalObject.setJump(sb,cpm); 
	}
	elseStack.push(cpm);
}

function processCorrectSwitch() {
	var cp = globalObject.currentPosition; 
	var cpm = globalObject.currentModuleSize;
	globalObject.setIndentRank(cp, 0,+2);
	globalObject.addToCurrentModule(cp); 
	elseStack.push(cpm);
	switchIfStack.push(SWITCH);
}

function processCorrectBeginOfLoop() {
	var cp = globalObject.currentPosition; 
	var cpm = globalObject.currentModuleSize;
	globalObject.setIndentRank(cp, 0, +1);
	globalObject.addToCurrentModule(cp); 
	loopStack.push(cpm);
}

function processCorrectAlgHeader() {
	var cp = globalObject.currentPosition;
	dano = globalObject.currentModuleSize;
	globalObject.addToCurrentModule(cp); 
}

function processCorrectAlgBegin() {
	var cp = globalObject.currentPosition; 
	globalObject.setIndentRank(cp, 0, +1);
	algBegin = globalObject.currentModuleSize;
	globalObject.addToCurrentModule(cp); 
	if ( dano != -1 )
		globalObject.setJump(dano, algBegin);
	dano = -1;
}

function processCorrectDocLine() {
	globalObject.setIndentRank(globalObject.currentPosition, +1, -1);
	globalObject.addToCurrentModule(globalObject.currentPosition); 
}

function processCorrectInputRestrictionLine() {
	var cpm = globalObject.currentModuleSize;
	globalObject.setIndentRank(globalObject.currentPosition, +1, -1);
	globalObject.addToCurrentModule(globalObject.currentPosition); 
	if ( dano != -1 )
		globalObject.setJump(dano, cpm);
	dano = cpm;
}

function processCorrectOutputRestrictionLine() {
	var cp = globalObject.currentPosition; 
	var cpm = globalObject.currentModuleSize;
	globalObject.setIndentRank(cp, +1, -1);
	globalObject.addToCurrentModule(cp); 
	if ( nado.length > 0 ) {
		var ln = nado.pop();
		globalObject.setJump(ln, cpm);
		nado.push(ln);
	}
	nado.push(cpm); 
}

function processCorrectAlgEnd() {
	var cp = globalObject.currentPosition; 
	globalObject.setIndentRank(cp, -1, 0);
	var cpm = globalObject.currentModuleSize;
	globalObject.addToCurrentModule(cp); 
	if ( algBegin != -1 )
		globalObject.setJump(algBegin,cpm); 
	if ( nado.length > 0 ) {
		var ln = nado.pop();
		globalObject.setJump(ln, -1);
		nado.push(ln);
		var fn = nado[0];
		globalObject.setJump(cpm, fn);
	}
// 	for ( i=0; i<nado.length; i++ ) {
// 		globalObject.setJump(nado[i],cpm);
// 	}
	algBegin = -1;
	nado = [];
	switchIfStack = [];
	loopStack = [];
	elseStack = [];
}

function processCorrectModuleBegin() {
	if ( globalObject.currentModuleSize > 0 ) {
		globalObject.pushMainIsp(); 
		mainDefined = true;
	}
	var cp = globalObject.currentPosition;
	globalObject.setIndentRank(cp, 0, +1);
	globalObject.addToCurrentModule(cp);
	isInModule = true;
	moduleBegin = cp;
}

function processCorrectModuleEnd() {
	var cp = globalObject.currentPosition;
	globalObject.setIndentRank(cp, -1, 0);
	globalObject.addToCurrentModule(cp);
	globalObject.pushIsp(); 
	isInModule = false;
	moduleBegin = -1;
}

function processCorrectLoad() {
	globalObject.addLoad(globalObject.currentPosition);
}

function appendSimpleLine() {
	globalObject.addToCurrentModule(globalObject.currentPosition);
}

function setGarbageAlgError() {
	globalObject.setError(globalObject.currentPosition,2200601); 
	globalObject.addToCurrentModule(globalObject.currentPosition);
}

function setGarbageIfThenError() {
	globalObject.setError(globalObject.currentPosition,2200602); 
	globalObject.addToCurrentModule(globalObject.currentPosition);
}

function setGarbageSwitchCaseError() {
	globalObject.setError(globalObject.currentPosition,2200603); 
	globalObject.addToCurrentModule(globalObject.currentPosition);
}


function setCorrespondingIfBroken() {
    var els = -1;
    globalObject.debug("setCorrespondingIfBroken())");
    //globalObject.debug("elseStack.length = "+elseStack.length);
    if (elseStack.length > 0) {
        var cp = globalObject.currentPosition;
        var cpm = globalObject.currentModuleSize;
        var offset = cp - cpm;
        var elsePos = elseStack.pop();
        globalObject.debug("elsePos = "+elsePos);
        globalObject.debug("cp = "+cp);
        globalObject.debug("cpm = "+cpm);
        globalObject.debug("offset = "+offset);
        elseStack.push(elsePos);
        globalObject.changeError(elsePos, offset, 2200309);
    }
}


function setExtraOpenKeywordError(kw) {
	var cp = globalObject.currentPosition; 
	var cpm = globalObject.currentModuleSize;
	if ( kw == IF ) {
		globalObject.setIndentRank(cp, 0, 0 );
		globalObject.setError(cp,1133);
		elseStack.push(cpm);
	}
	else if ( kw == THEN ) {
		globalObject.setIndentRank(cp, -1, -1);
		globalObject.setError(cp,12019);
	}
	else if ( kw == SWITCH ) {
		globalObject.setError(cp, 12021);
		elseStack.push(cpm);
		switchIfStack.push(SWITCH);
	}
	else if ( kw == CASE ) {
		globalObject.setError(cp, 12021);
		elseStack.push(cpm);
	}
	else if ( kw == LOOP ) {
		globalObject.setIndentRank(cp, 0, 0);
		globalObject.setError( cp, 12018 );
		loopStack.push(cpm);
	}
	else if ( kw == BEGIN ) { 
		globalObject.setError( cp, 2200308);
		if ( dano != -1 )
			globalObject.setJump(dano, globalObject.currentModuleSize);
		dano = -1;
	}
	else if ( kw == ELSE ) {
		globalObject.setError( cp, 2200309);
		globalObject.setIndentRank(cp, -1, -1);
	}
	else if ( kw == MODULE ) { 
		if ( !isInModule && !mainDefined ) {
			globalObject.pushMainIsp(); 
		}
		else {
			globalObject.pushIsp();
		}
		globalObject.setError( cp, 1140 );
		globalObject.setIndentRank(cp, 0, 0);
		isInModule = false;
	}
	else {
		globalObject.setError(cp, 2200300);
	}
	globalObject.addToCurrentModule(cp); 
}

function setExtraCloseKeywordError(kw) {
	var cp = globalObject.currentPosition;
	var cpm = globalObject.currentModuleSize;
	if ( kw == FI ) {
// 		if ( switchIfStack.length > 0 ) {
// 			if ( elseStack.length > 0 ) {
// 				var sb = elseStack.pop();
// 				globalObject.setJump(sb, cpm);
// 				globalObject.setIndentRank(cp, -2, 0);
// 			} 
// 			else globalObject.setError(cp, 2200205);
// 		}
// 		else 
			globalObject.setError(cp, 1175);
	}
	else if ( kw == LOOPEND ) {
// 		if ( loopStack.length > 0 ) {
// 			var lb = loopStack.pop();
// 			globalObject.setJump(lb,cpm); 
// 			var offset = cp - cpm;
// 			var beginPos = lb + offset;
// 			var beginError = globalObject.getError(beginPos);
// 			if ( beginError == 2200302 ) {
// 				globalObject.changeError(lb, offset, 0);
// 				globalObject.setIndentRank(beginPos, 0, +1);
// 				globalObject.setIndentRank(cp, -1, 0);
// 			}
// 			else globalObject.setError(cp, 2200210);
// 		}
// 		else {
			globalObject.setError(cp, 1164);
// 		}
	}
	else if ( kw == LOOPCONDEND ) {
// 		if ( loopStack.length > 0 ) {
// 			var lb = loopStack.pop();
// 			globalObject.setJump(lb,cpm); 
// 			var offset = cp - cpm;
// 			var beginPos = lb + offset;
// 			var beginError = globalObject.getError(beginPos);
// 			if ( beginError == 2200302 ) {
// 				globalObject.changeError(lb, offset, 0);
// 				globalObject.setIndentRank(beginPos, 0, +1);
// 				globalObject.setIndentRank(cp, -1, 0);
// 			}
// 			else globalObject.setError(cp, 2200211);
// 		}
// 		else {
			globalObject.setError(cp, 1188);
// 		}
	}
	else if ( kw == END ) {
		globalObject.setError(cp, 2200201);
	}
	else if ( kw == ELSE ) {
// 		if ( switchIfStack.length > 0 ) {
// 			var elem = switchIfStack.pop();
// 			switchIfStack.push(elem);
// 			if ( elem == SWITCH ) {
// 				if ( elseStack.length > 0 ) {
// 					var sb = elseStack.pop(); 
// 					globalObject.setJump(sb,cpm); 
// 					var offset = cp - cpm;
// 					var switchPos = sb + offset;
// 					var switchError = globalObject.getError(switchPos);
// 					if ( switchError == 2200307 ) {
// 						globalObject.setIndentRank(cp, -1, +1);
// 						globalObject.setIndentRank(switchPos, 0, +2);
// 						globalObject.changeError(sb, offset, 2200401);
// 						elseStack.push(cpm);
// 					}
// 					else globalObject.setError(cp, 2200214);
// 				}
// 				else 
// 					globalObject.setError(cp, 2200214);
// 			}
// 		}
// 		else {
			globalObject.setError(cp, 2200214);
// 		}
	}
	else if ( kw == NADO ) {
		globalObject.setError(cp, 1117);
	}
	else if ( kw == DANO ) {
		globalObject.setError(cp, 2200216);
	}
	else if ( kw == DOC ) {
		globalObject.setError(cp, 11100);
	}
	else if ( kw == SWITCH ) {
		globalObject.setError(cp, 2200218);
	}
	else if ( kw == CASE ) {
		globalObject.setError(cp, 1113);
	}
	else if ( kw == IF ) {
		globalObject.setError(cp, 2200220);
	}
	else if ( kw == THEN ) {
		globalObject.setError(cp, 1109);
	}
	else {
		globalObject.setError(cp, 2200200);
	}
	globalObject.addToCurrentModule(cp); 
}
