var loadedEnv = G.ModuleRoot+"/modules/defaults/gastarbaters/default.fil";

function loadDefaultEnvironment()
{
    var defaultEnv = G.ModuleRoot+
                        "/modules/defaults/gastarbaters/default.fil";
    G.Log = "Default env: " + defaultEnv;
    loadEnvironment(defaultEnv);
}

function loadEnvironment(filename)
{
    loadedEnv = filename;
    loadEnvironmentFromKumRobotFile(filename);
}

function reloadEnvironment(filename)
{
    loadEnvironment(loadedEnv);
}