require("require");

Game.renderTarget = Game.renderTarget || new RenderTarget("Default");
timer = 0;

Game.Initialise = function()
{
	Game.quad1 = new Quad();
	Game.quad1.spawn("Default");

	Game.quad2 = new Quad();
	Game.quad2.spawn("Default");

	Game.quad2.setTranslation(2, 0, 0);
	Game.camera = new Camera(CameraType.Perspective);
}

Game.Update = function(dt)
{
	if (Keyboard.isReleased(Key.Q))
	{
		Game.quit();
	}

	if (Keyboard.isDown(Key.W))
	{
		Game.camera.translateBy(0, 0, -dt * 30);
	}

	if (Keyboard.isDown(Key.S))
	{
		Game.camera.translateBy(0, 0, dt * 30);
	}

	if (Keyboard.isDown(Key.A))
	{
		Game.camera.translateBy(-dt * 30, 0, 0);
	}

	if (Keyboard.isDown(Key.D))
	{
		Game.camera.translateBy(dt * 30, 0, 0);
	}

	if (Mouse.isDown(MouseButton.Left))
	{
		var movement = Mouse.movement();
		Game.camera.rotateBy(movement.y * dt * 10, -movement.x * dt * 10, 0);
	}
	timer = timer || 0;

	timer += dt*3;
	Game.quad2.setRotation(Math.cos(timer), 0, Math.sin(timer));
	Game.quad2.setTranslation(Math.sin(timer) * 2, Math.cos(timer) * 3, 0);
}

Game.FixedUpdate = function(timeSteps, fixedDelta)
{
	
}

Game.Draw = function(dt)
{
	Game.render(Game.camera);
}

Game.Shutdown = function()
{

}

Game.OnReload = function(path)
{
	
}
