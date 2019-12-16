function turnLeft()
{
    var direction = this.attribute("direction");
    if (direction=="north") {
        this.setAttribute("direction","west");
        this.setCurrentFrame(13);
    }
    if (direction=="west") {
        this.setAttribute("direction","south");
        this.setCurrentFrame(1);
    }
    if (direction=="south") {
        this.setAttribute("direction","east");
        this.setCurrentFrame(5);
    }
    if (direction=="east") {
        this.setAttribute("direction","north");
        this.setCurrentFrame(9);
    }
    this.sleep_msec(200);
}

function turnRight()
{
    var direction = this.attribute("direction");
    if (direction=="north") {
        this.setAttribute("direction","east");
        this.setCurrentFrame(5);
    }
    if (direction=="east") {
        this.setAttribute("direction","south");
        this.setCurrentFrame(1);
    }
    if (direction=="south") {
        this.setAttribute("direction","west");
        this.setCurrentFrame(13);
    }
    if (direction=="west") {
        this.setAttribute("direction","north");
        this.setCurrentFrame(9);
    }
    this.sleep_msec(200);
}
