require("require");

var stack3 = function()
{
	CVar.register();
}

var stack2 = function()
{
	stack3();
}

var stack1 = function()
{
	stack2();
}

Game.Initialise = function()
{
	stack1();
}

Game.Update = function(dt)
{
	if (Keyboard.isReleased(Key.Q))
	{
		Game.quit();
	}
}

Game.FixedUpdate = function(timeSteps, fixedDelta)
{
	
}

Game.Shutdown = function()
{

}

Game.OnReload = function()
{

}
