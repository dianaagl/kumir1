function isParquet()
{
    var scene = G.Scene;
    var x = this.x;
    var y = this.y;
    return scene.cellBorderState(x, y, 0, "bottom")==4;
}

function isGarbage()
{
    var scene = G.Scene;
    var x = this.x;
    var y = this.y;
    return scene.cellBorderState(x, y, 0, "bottom")==5;
}

function doParquete()
{
    var gogi = G.actor("gogi");
    var scene = G.Scene;
    var x = gogi.x;
    var y = gogi.y;
    Log = "Паркет";
    scene.setCellBorderState(x, y, 0, "bottom", 1);
    gogi.sleep_msec(100);
    scene.setCellBorderState(x, y, 0, "bottom", 2);
    gogi.sleep_msec(100);
    scene.setCellBorderState(x, y, 0, "bottom", 3);
    gogi.sleep_msec(100);
    scene.setCellBorderState(x, y, 0, "bottom", 4);
    gogi.sleep_msec(100);
}

function doCleanup()
{
    var said = G.actor("said");
    var scene = G.Scene;
    var x = said.x;
    var y = said.y;
    scene.setCellBorderState(x, y, 0, "bottom", 0);
    said.sleep_msec(500);
}
