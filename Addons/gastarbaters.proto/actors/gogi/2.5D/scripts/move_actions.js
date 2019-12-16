function processMove(actor)
{
    // функция физически реалистичного движения робота
    /* Будем вычислять траекторию в соответствии с законом физики:
    
    X(t) - X(t_0) = V(t) * ( t - t_0 )
    
             {  max( V_max, V(t_0) + A_acc * (t - t_0) ), если X(t_0) < X_th 
    V(t) = {
             {  max( V_min, V(t_0) + A_br * (t - t_0) ),  если X_th <= X(t_0) < 1.0
             
    
    Здесь параметрами являются:
        V_max -- максимально допустимая скорость
        V_min -- скорость робота при докатывании по инерции
        A_acc -- мощность двигателя
        A_br  -- сила тормозов
        X_th  -- точка \in[0:1] , когда нужно тормозить
    
    */ 
    
    const V_max = 0.001; // процентов/мсек
    const V_min = 0.0001; // процентов/мсек
    const A_acc = 0.0001; // процентов/мсек**2
    const A_br  = -0.00003; // процентов/мсек**2
    const X_th = 0.5; // процентов от всего пути
    
    var V_t0 = 0;
    var V_t = 0;
    var X_t = 0;
    var X_t0 = 0;
    
    while (X_t<1.0) { // цикл пока не приехали
        
        const deltaT = 30;
        V_t0 = V_t;
        X_t0 = X_t;
        
        if (X_t0 < X_th) {
            // ускорение
            var variant_1 = V_max;
            var variant_2 = V_t0 + A_acc*deltaT;
            V_t = variant_1>variant_2? variant_1 : variant_2;
        }
        else {
            // торможение
            var variant_1 = V_min;
            var variant_2 = V_t0 + A_br*deltaT;
            V_t = variant_1>variant_2? variant_1 : variant_2;
        }
        
        var deltaX = V_t * deltaT;
        X_t = X_t0 + deltaX;
        
        actor.moveBy(deltaX);
        actor.sleep_msec(deltaT);
    }
    
    
    
}

function moveForward()
{
    // Движение вперед на одну клетку
    var direction = this.attribute("direction");
    var canMove = true;
    var wall = false;
    var targetCellX, targetCellY;
    
    // Определение направления движения и наличия стены
    if (direction=="north") {
        targetCellX = this.x;
        targetCellY = this.y + 1;
        wall = G.Scene.cellBorderState(this.x, this.y, 0, "front") > 0;
    }
    else if (direction=="south") {
        targetCellX = this.x;
        targetCellY = this.y - 1;
        wall = G.Scene.cellBorderState(this.x, this.y, 0, "back") > 0;
    }
    else if (direction=="east") {
        targetCellX = this.x + 1;
        targetCellY = this.y;
        wall = G.Scene.cellBorderState(this.x, this.y, 0, "right") > 0;
    }
    else if (direction=="west") {
        targetCellX = this.x - 1;
        targetCellY = this.y;
        wall = G.Scene.cellBorderState(this.x, this.y, 0, "left") > 0;
    }
    // Проверяем, что робот не вышел за границы поля
    if (targetCellX<0 || targetCellY<0)
        canMove = false;
    if (targetCellX>=G.Scene.width || targetCellY>=G.Scene.length)
        canMove = false;
    
    canMove = canMove && !wall;
    
    if (!canMove) {
        // TODO вывод сообщения об ошибке
        return;
    }
        
    // Начало анимации движения
    // -- здесь указываем клетку назначения
    this.beginMove(targetCellX, targetCellY, 0);    
    // -- собственно, само движение (см. функцию processMove)
    processMove(this);
    // Конец анимации движения
    this.endMove();
}
