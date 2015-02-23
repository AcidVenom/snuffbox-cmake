require("require");

Game.renderTarget = new RenderTarget("Default");

Game.Initialise = function()
{
	Game.quad = new Quad();
	Game.quad.spawn("Default");
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

Game.OnReload = function(path)
{
	
}
