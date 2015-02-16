require("require");

Game.initialise = function()
{

}

Game.update = function(dt)
{
	if (Keyboard.isReleased(Key.Q))
	{
		Game.quit();
	}
}
