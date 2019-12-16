#ifndef ROBOTERRORS_H
#define ROBOTERRORS_H

namespace RobotErrors {

    enum RuntimeError {
        NoRuntimeError,
        WallCollisionError
    };

    enum EnvironmentError {
        NoFileError,
        FileNotExists,
        FileAccessDenied,
        FileDamaged
    };

}



#endif // ROBOTERRORS_H
