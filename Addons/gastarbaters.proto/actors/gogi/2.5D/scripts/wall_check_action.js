function isWallAhead()
{
    var direction = this.attribute("direction");
    if (direction=="north") {
        return G.Scene.cellBorderState(this.x, this.y, 0, "front") > 0;
    }
    else if (direction=="south") {
        return G.Scene.cellBorderState(this.x, this.y, 0, "back") > 0;
    }
    else if (direction=="east") {
        return G.Scene.cellBorderState(this.x, this.y, 0, "right") > 0;
    }
    else if (direction=="west") {
        return G.Scene.cellBorderState(this.x, this.y, 0, "left") > 0;
    }
    return false; // unknown direction
}
