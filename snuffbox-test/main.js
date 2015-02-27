Game.targets = Game.targets || {
	default: new RenderTarget("Default")
}

Game.Initialise = function()
{
	ContentManager.load("texture", "wood.png");
	ContentManager.load("texture", "wood_normal.png");
	ContentManager.load("texture", "wood_bump.png");

	ContentManager.load("effect", "test.effect");
	ContentManager.load("effect", "post_processing.effect");
	ContentManager.load("material", "test.material");

	Game.quad = new Quad();
	Game.quad.spawn("Default");
	Game.quad.setMaterial("test.material");

	Game.targets.default.setPostProcessing("post_processing.effect");

	Game.camera = new Camera(CameraType.Perspective);
}

Game.Update = function(dt)
{
	if (Keyboard.isReleased(Key.Q))
	{
		Game.quit();
	}

	var mz = 0,
		mx = 0,
		rx = 0,
		ry = 0,
		speed = 5;

	if (Keyboard.isDown(Key.W))
	{
		mz = -dt * speed;
	}
	else if (Keyboard.isDown(Key.S))
	{
		mz = dt * speed;
	}

	if (Keyboard.isDown(Key.A))
	{
		mx = -dt * speed;
	}
	else if (Keyboard.isDown(Key.D))
	{
		mx = dt * speed;
	}

	if (Mouse.isDown(MouseButton.Left))
	{
		var movement = Mouse.movement();
		rx = movement.y / 100;
		ry = -movement.x / 100;
	}

	Game.camera.translateBy(mx, 0, mz);
	Game.camera.rotateBy(rx, ry, 0);
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
