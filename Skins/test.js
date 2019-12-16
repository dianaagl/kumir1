var lastAngle;

function switchPressed(x, y) {
  var r = Math.sqrt(x*x+y*y);
  lastAngle = Math.acos(x/r)/(2*3.14159)*360;
  if (y>0)
    lastAngle = 360-lastAngle;
}

function switchReleased(x, y) {
  var value = TurnSwitch.rotation;
  while (value<0)
    value += 360;
  while (value>=360)
    value -= 360;
  Degrees.widget.setValue(value); 
}

function switchMoved(x, y) {
  var r = Math.sqrt(x*x+y*y);
  var curAngle = Math.acos(x/r)/(2*3.14159)*360;
  if (y>0)
    curAngle = 360-curAngle;
  var dAngle = lastAngle-curAngle;
  lastAngle = curAngle;
  TurnSwitch.rotation += dAngle;

  var value = TurnSwitch.rotation;
  while (value<0)
    value += 360;
  while (value>=360)
    value -= 360;
  Degrees.widget.setValue(value);
}

function InitializeDegrees() {
  Degrees.widget.maximum = 359;
  Degrees.widget.minimum = 0;
}

function InitializeDistance() {
  Distance.widget.maximum = 500;
  Distance.widget.minimum = 1;
  Distance.widget.value = 1;
}

function CreateConnections() {
  connectWidgetToItem("Degrees", "valueChanged(int)", 
		      "TurnSwitch", "setRotation(int)");
}

function btnTurtlePressed(x, y) {
    TurtleButtonTopLeft.fill = "#009600";
    TurtleButtonBottomRight.fill = "#96FF96";
    TurtleBody.transform = "translate(2,2)";
}

function btnTurtleReleased(x, y) {
    TurtleButtonTopLeft.fill = "#96FF96";
    TurtleButtonBottomRight.fill = "#009600";
    TurtleBody.transform = "";
}
function btnPenUpClicked()
{
 tailUp();
}
function btnPenDownClicked()
{
 tailDown();
}

function btnUpClicked()
{
Fwd();
}
function btnDownClicked()
{
Back();
}
function btnLeftClicked()
{
Left();
} 
function btnRightClicked()
{
Right();
}
function btnScrollDownPressed()
{

 LogDown();
};
function btnScrollUpPressed()
{LogUp();
};
function btnTurtleClicked()
{
    ResetTurtle();
};
function btnSendToTurtleClicked()
{
    CopyLog();
};
