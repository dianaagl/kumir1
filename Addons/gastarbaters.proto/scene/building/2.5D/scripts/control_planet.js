function cellPainted(x,y)
{
   return Scene.cellBorderState(x,y,0)>0;
}

function paintCell(x,y)
{
   Scene.setCellAttribute(x,y,0,"painted",1);
   Scene.setBorderState(x,y,0,"bottom",1);
}

function isWallAtNorth(x,y)
{
   return Scene.borderState(x,y,0,"front")>0;
}

function isWallAtSouth(x,y)
{
   return Scene.borderState(x,y,0,"back")>0;
}

function isWallAtLeft(x,y)
{
   return Scene.borderState(x,y,0,"left")>0;
}

function isWallAtRight(x,y)
{
   return Scene.borderState(x,y,0,"right")>0;
}
