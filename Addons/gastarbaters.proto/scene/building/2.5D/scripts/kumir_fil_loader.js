function setActorDirection(actor, direction)
{
    actor.setAttribute("direction", direction);
    if (direction=="south") {
        actor.setCurrentFrame(1);
    }
    else if (direction=="east") {
        actor.setCurrentFrame(5);
    }
    else if (direction=="north") {
        actor.setCurrentFrame(9);
    }
    else if (direction=="west") {
        actor.setCurrentFrame(13);
    }
}

function loadEnvironmentFromKumRobotFile(fileName)
{
    var gogi = G.actor("gogi");
    var said = G.actor("said");
    var Scene = G.Scene;

    
    // Парсировка .fil-файла
    
    var data = G.readText(fileName, "UTF-8");
    var w, h;
    var gogiX = 0;
    var gogiY = 0;
    var saidX = 0;
    var saidY = 0;
    var gogiDirection = "south";
    var saidDirection = "south";
    var sizeRead = false;
    var actorPositionRead = false;
    var lines = data.split("\n");
    var cells = [];   
    var aliensPos = [];
     
    for (var lineNo=0; lineNo<lines.length; lineNo++) {
        var line = lines[lineNo];
        if (line.length==0 || line[0]==';')
            continue;
        if (!sizeRead) {
            var values = line.split(" ");
            w = Number(values[0]);
            h = Number(values[1]);
            sizeRead = true;
        }
        else if (!actorPositionRead) {
            actorPositionRead = true;
        }
        else {
            var values = line.split(" ");
            var wall = Number(values[2]);              
            var cell = {
                x: Number(values[0]),
                y: h - 1 - Number(values[1]),
                state: 0,
                wallLeft: (wall & 0x1) > 0? 1 : 0,
                wallRight: (wall & 0x2) > 0? 1 : 0,
                wallBack: (wall & 0x4) > 0? 1 : 0,
                wallFront: (wall & 0x8) > 0? 1 : 0,
                garbage: false,
                parquet: false
            };
            if (values.length > 8 && values[8]=="1") {
                cell.garbage = true;
            }
            if (Number(values[3])>0 && !cell.garbage) {
                cell.parquet = true;
            }
            cells.push(cell);
            if (values[6]=="g") {
                gogiX = cell.x;
                gogiY = cell.y;
            }
            if (values[6]=="s") {
                saidX = cell.x;
                saidY = cell.y;
            }
        }
    }
    
    // Инициализация сцены
    
    Scene.setSize(w, h, 1);
    for (var y=0; y<h; y++) {
        for (var x=0; x<w; x++) {
            var cell = {
                x: x,
                y: y,
                state: 0,
                wallLeft: 0,
                wallRight: 0,
                wallBack: 0,
                wallFront: 0,
                garbage: false,
                parquet: false
            };
            for (var i=0; i<cells.length; i++) {
                if (cells[i].x==x && cells[i].y==y) {
                    cell = cells[i];
                }
            }
            if (cell.garbage)
                cell.state = 5;
            else if (cell.parquet)
                cell.state = 4;
            else
                cell.state = 0;
            Scene.setCellAttribute(x,y,0,"painted",cell.state);
            Scene.setCellBorderState(x,y,0,"bottom",cell.state);
            if (cell.wallLeft>0)
                Scene.setCellBorderState(x,y,0,"left",cell.wallLeft);
            if (cell.wallRight>0)
                Scene.setCellBorderState(x,y,0,"right",cell.wallRight);
            if (cell.wallFront>0)
                Scene.setCellBorderState(x,y,0,"front",cell.wallFront);
            if (cell.wallBack>0)
                Scene.setCellBorderState(x,y,0,"back",cell.wallBack);
        }
    }
    
    // Стены вокруг всей сцены
    
    
    for (var y=0; y<h; y++) {
        Scene.setCellBorderState(0,y,0,"left",2);
        Scene.setCellBorderState(w-1,y,0,"right",1);
    }
    
    for (var x=0; x<w; x++) {
        Scene.setCellBorderState(x,0,0,"back",1);
        Scene.setCellBorderState(x,h-1,0,"front",2);
    }
    

    // Инициализация гастарбайтеров
    
    said.x = saidX;
    said.y = saidY;
    said.z = 0;
    gogi.x = gogiX;
    gogi.y = gogiY;
    gogi.z = 0;
    setActorDirection(said, "south");
    setActorDirection(gogi, "south");
    said.visible = true;
    gogi.visible = true;
    
    
}

