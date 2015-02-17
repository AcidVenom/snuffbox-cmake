require("require");

Game.Initialise = function()
{

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
	Log.info(Game.fixedStep())
}

Game.Shutdown = function()
{

}

Game.OnReload = function()
{

}
